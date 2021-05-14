#include "tensorflow/core/framework/op.h"
#include "tensorflow/core/framework/shape_inference.h"

using namespace tensorflow;

REGISTER_OP("MecabLoad")
    .Attr("model_path: string = ''")
    .Attr("container: string = ''")
    .Attr("shared_name: string = ''")
    .Output("handle: resource")
    .SetIsStateful()
    .SetShapeFn([](shape_inference::InferenceContext* c) {
        c->set_output(0, c->Scalar());
        return Status::OK();
    });

REGISTER_OP("MecabTag")
    .Input("handle: resource")
    .Input("input: string")
    .Output("output_values: string")
    .Output("output_features: string")
    .Output("output_splits: int64")
    .SetShapeFn([](shape_inference::InferenceContext* c) {
        shape_inference::ShapeHandle unused;
        TF_RETURN_IF_ERROR(c->WithRank(c->input(0), 0, &unused));
        TF_RETURN_IF_ERROR(c->WithRank(c->input(1), 1, &unused));

        c->set_output(0, c->Vector(shape_inference::InferenceContext::kUnknownDim));
        c->set_output(1, c->Vector(shape_inference::InferenceContext::kUnknownDim));
        shape_inference::DimensionHandle num_splits;
        TF_RETURN_IF_ERROR(c->Add(c->NumElements(c->input(1)), 1, &num_splits));
        c->set_output(2, c->Vector(num_splits));
        return Status::OK();
    });
