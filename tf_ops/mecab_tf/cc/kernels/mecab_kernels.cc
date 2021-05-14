// Refernence
//
// https://github.com/tensorflow/text/blob/master/tensorflow_text/core/kernels/sentencepiece_kernels.cc
// https://github.com/tensorflow/text/blob/master/tensorflow_text/core/ops/sentencepiece_ops.cc

#include "absl/base/thread_annotations.h"
#include "absl/synchronization/mutex.h"
#include "tensorflow/core/framework/dataset_stateful_op_allowlist.h"
#include "tensorflow/core/framework/op_kernel.h"
#include "tensorflow/core/framework/resource_mgr.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/framework/tensor_types.h"
#include "tensorflow/core/framework/types.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/lib/core/refcount.h"
#include "tensorflow/core/util/work_sharder.h"
#include <mecab.h>

using namespace tensorflow;

namespace {

struct MecabResource : public ResourceBase {
    MeCab::Model* model = nullptr;
    MeCab::Tagger* tagger = nullptr;

    ~MecabResource() {
        if (tagger) delete tagger;
        if (model) delete model;
    }
    string DebugString() const override { return "Mecab Resource"; }
};

// https://github.com/tensorflow/text/blob/045ce4afddfe0cd89b156f2abbf8af730a558e48/tensorflow_text/core/kernels/sentencepiece_kernels.cc#L69-L73
// 100us
constexpr int64 kCostPerUnit = 100000;

template <typename T>
::tensorflow::Status MecabStatusToTFStatus(T eval) {
    if (!eval) {
        const char *e = MeCab::getTaggerError();
        return ::tensorflow::Status(::tensorflow::error::Code::UNKNOWN, ::tensorflow::string("Exception:") + e);
    }
    return ::tensorflow::Status();
}

}

class MecabLoadOp : public OpKernel {
public:
    explicit MecabLoadOp(OpKernelConstruction* context) : OpKernel(context), mecab_set(false) {
        // OP_REQUIRES_OK(ctx, ctx->allocate_temp(tensorflow::DT_STRING, tensorflow::TensorShape({2}), &sp_));
    }

    ~MecabLoadOp() override {
        // If the table object was not shared, delete it.
        if (mecab_set && containerInfo.resource_is_private_to_kernel()) {
            if (!containerInfo.resource_manager()->template Delete<MecabResource>(containerInfo.container(), containerInfo.name()).ok()) {
                // Do nothing; the resource may have been deleted by session resets.
            }
        }
    }

    void Compute(OpKernelContext* ctx) override {
        absl::MutexLock lock(&mu_);

        if (!mecab_set) {
            OP_REQUIRES_OK(ctx, containerInfo.Init(ctx->resource_manager(), def()));
        }

        auto creator = [this](MecabResource** resource) ABSL_EXCLUSIVE_LOCKS_REQUIRED(mu_) {
            MecabResource* mr = new MecabResource();

            tensorflow::string model_path;
            TF_RETURN_IF_ERROR(GetNodeAttr(this->def(), "model_path", &model_path));

            if (TF_PREDICT_FALSE(model_path.empty())) {
                return tensorflow::Status(tensorflow::errors::InvalidArgument("Model path must be specified."));
            }

            mr->model = MeCab::createModel(("-C -r /dev/null -d " + model_path).c_str());
            TF_RETURN_IF_ERROR(MecabStatusToTFStatus(mr->model));
            mr->tagger = mr->model->createTagger();
            TF_RETURN_IF_ERROR(MecabStatusToTFStatus(mr->tagger));

            // TODO: memory tracking
            *resource = mr;
            return Status::OK();
        };

        MecabResource* resource = nullptr;
        OP_REQUIRES_OK(ctx, containerInfo.resource_manager()->template LookupOrCreate<MecabResource>(containerInfo.container(), containerInfo.name(), &resource, creator));
        core::ScopedUnref unref_me(resource);

        Tensor* handle;
        OP_REQUIRES_OK(ctx, ctx->allocate_output(0, TensorShape({}), &handle));
        handle->scalar<ResourceHandle>()() = MakeResourceHandle<MecabResource>(ctx, containerInfo.container(), containerInfo.name()); // resource_mgr.h

        mecab_set = true;
    }

private:
    absl::Mutex mu_;
    // https://github.com/tensorflow/tensorflow/blob/85c8b2a817f95a3e979ecd1ed95bff1dc1335cff/tensorflow/core/framework/resource_mgr.h#L387
    ContainerInfo containerInfo;
    bool mecab_set ABSL_GUARDED_BY(mu_);
};

REGISTER_KERNEL_BUILDER(Name("MecabLoad").Device(DEVICE_CPU), MecabLoadOp);
ALLOW_STATEFUL_OP_FOR_DATASET_FUNCTIONS("MecabLoad");


class MecabTagOp : public OpKernel {
public:
    explicit MecabTagOp(OpKernelConstruction* ctx) : OpKernel(ctx) {
        // TODO nbest
    }

    void Compute(OpKernelContext* ctx) override {
        MecabResource* mr;
        const Tensor& resource_tensor = ctx->input(0);
        ResourceHandle resource_handle(resource_tensor.scalar<ResourceHandle>()());
        OP_REQUIRES_OK(ctx, ctx->resource_manager()->Lookup<MecabResource, true>(resource_handle.container(), resource_handle.name(), &mr));
        core::ScopedUnref unref_me(mr);

        const Tensor& input_values_tensor = ctx->input(1);
        const auto input_values_flat = input_values_tensor.flat<tensorflow::tstring>();
        const int64 num_of_input_values = input_values_flat.size();

        std::vector<std::vector<std::string>> tokens(num_of_input_values);
        std::vector<std::vector<std::string>> features(num_of_input_values);
        const auto& worker_threads = *(ctx->device()->tensorflow_cpu_worker_threads());

        ::tensorflow::Shard(
            worker_threads.num_threads,  // max parallelism
            worker_threads.workers,      // thread pool
            num_of_input_values,         // total number of data to process.
            kCostPerUnit,                // cost per unit
            [ctx, mr, &input_values_flat, &tokens, &features](int64 start, int64 limit) {
                // absl::ReaderMutexLock lock(&sp->mu);
                MeCab::Lattice *lattice = mr->model->createLattice();

                for (int i = start; i < limit; ++i) {
                    lattice->set_sentence(input_values_flat(i).c_str());
                    OP_REQUIRES_OK(ctx, MecabStatusToTFStatus(mr->tagger->parse(lattice)));

                    const MeCab::Node* node = lattice->bos_node();
                    OP_REQUIRES_OK(ctx, MecabStatusToTFStatus(node));

                    for (; node; node = node->next) {
                        tokens[i].emplace_back(std::string(node->surface, node->length));
                        features[i].emplace_back(std::string(node->feature));
                    }
                }
                delete lattice;
            });

        int64 total_tokens = 0;
        for (auto& tokens_row : tokens)
            total_tokens += tokens_row.size();

        Tensor* output_values_tensor = nullptr;
        Tensor* output_features_tensor = nullptr;
        Tensor* output_splits_tensor = nullptr;

        OP_REQUIRES_OK(ctx, ctx->allocate_output(0, {total_tokens}, &output_values_tensor));
        OP_REQUIRES_OK(ctx, ctx->allocate_output(1, {total_tokens}, &output_features_tensor));
        int64 splits_size = tokens.size() + 1;
        OP_REQUIRES_OK(ctx, ctx->allocate_output(2, {splits_size}, &output_splits_tensor));

        auto values_tensor_flat = output_values_tensor->vec<tensorflow::tstring>();
        auto features_tensor_flat = output_features_tensor->vec<tensorflow::tstring>();
        auto splits_tensor_flat = output_splits_tensor->vec<int64>();

        int i = 0;
        splits_tensor_flat(0) = 0;
        for (int row = 0; row < tokens.size(); ++row) {
            for (int col = 0; col < tokens[row].size(); ++col, ++i) {
                values_tensor_flat(i) = tokens[row][col];
                features_tensor_flat(i) = features[row][col];
            }
            splits_tensor_flat(row + 1) = i;
        }
    }
};

REGISTER_KERNEL_BUILDER(Name("MecabTag").Device(DEVICE_CPU), MecabTagOp);
ALLOW_STATEFUL_OP_FOR_DATASET_FUNCTIONS("MecabTag");
