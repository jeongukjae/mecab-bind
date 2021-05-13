#include <iostream>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <string>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <mecab.h>

namespace py = pybind11;

#define CHECK_MECAB_ERROR(eval) if (! eval) { \
   const char *e = MeCab::getTaggerError(); \
   std::cerr << "Exception:" << e << std::endl; \
   throw std::runtime_error(std::string("Exception:") + e); }

struct Node {
    const std::string surface;
    const std::string feature;
    unsigned int id;
    unsigned short length;
    unsigned short rlength;
    unsigned short rcAttr;
    unsigned short lcAttr;
    unsigned short posid;
    unsigned char char_type;
    unsigned char stat;
    unsigned char isbest;
    float alpha;
    float beta;
    float prob;
    short wcost;
    long cost;
};

struct DictionaryInfo {
    const std::string filename;
    const std::string charset;
    unsigned int size;
    int type;
    unsigned int lsize;
    unsigned int rsize;
    unsigned short version;
};

class Tagger {
private:
    MeCab::Model *model = nullptr;
    MeCab::Tagger *tagger = nullptr;

public:
    Tagger(const std::string &modelArgument) {
        model = MeCab::createModel(modelArgument.c_str());
        CHECK_MECAB_ERROR(model);
        tagger = model->createTagger();
        CHECK_MECAB_ERROR(tagger);
    }
    ~Tagger() {
        if (tagger != nullptr) delete tagger;
        if (model != nullptr) delete model;
    }

    const std::string parse(const std::string& input_sentence) {
        MeCab::Lattice *lattice = model->createLattice();
        lattice->set_sentence(input_sentence.c_str());
        CHECK_MECAB_ERROR(tagger->parse(lattice));
        std::string result = lattice->toString();
        delete lattice;
        return result;
    }

    const std::vector<Node> parse_node_with_lattice(const std::string& input_sentence) {
        MeCab::Lattice *lattice = model->createLattice();
        lattice->set_sentence(input_sentence.c_str());
        CHECK_MECAB_ERROR(tagger->parse(lattice));

        const MeCab::Node* node = lattice->bos_node();
        CHECK_MECAB_ERROR(node);

        std::vector<Node> results;
        for (; node; node = node->next)
            results.emplace_back(Node{
                std::string(node->surface, node->length), node->feature, node->id, node->length, node->rlength, node->rcAttr,
                node->lcAttr, node->posid, node->char_type, node->stat, node->isbest, node->alpha,
                node->beta, node->prob, node->wcost, node->cost
            });
        delete lattice;
        return results;
    }

    const std::vector<std::vector<Node>> parse_nbest_with_lattice(const std::string& input_sentence) {
        MeCab::Lattice *lattice = model->createLattice();
        lattice->set_request_type(MECAB_NBEST);
        lattice->set_sentence(input_sentence.c_str());
        CHECK_MECAB_ERROR(tagger->parse(lattice));

        std::vector<std::vector<Node>> results;
        for (int i = 0; i < 10; ++i) {
            const MeCab::Node* node = lattice->bos_node();
            CHECK_MECAB_ERROR(node);

            std::vector<Node> local_result;
            for (; node; node = node->next)
                local_result.emplace_back(Node{
                    std::string(node->surface, node->length), node->feature, node->id, node->length, node->rlength, node->rcAttr,
                    node->lcAttr, node->posid, node->char_type, node->stat, node->isbest, node->alpha,
                    node->beta, node->prob, node->wcost, node->cost
                });
            results.emplace_back(local_result);

            if (!lattice->next()) // No more results
                break;
        }
        delete lattice;
        return results;
    }

    std::vector<DictionaryInfo> get_dictionary_info() const {
        std::vector<DictionaryInfo> results;

        const MeCab::DictionaryInfo *d = tagger->dictionary_info();
        for (; d; d = d->next)
            results.emplace_back(DictionaryInfo{
                d->filename, d->charset, d->size, d->type, d->lsize, d->rsize, d->version
            });

        return results;
    }
};

const std::string get_model_args(const std::string dic_path) {
    return std::string("-C -r /dev/null -d ") + dic_path;
}

PYBIND11_MODULE(bind, m) {
    m.attr("__name__") = "mecab.bind";
    m.def("get_model_args", &get_model_args);

    py::class_<DictionaryInfo>(m, "DictionaryInfo")
        .def_readonly("filename", &DictionaryInfo::filename)
        .def_readonly("charset", &DictionaryInfo::charset)
        .def_readonly("size", &DictionaryInfo::size)
        .def_readonly("type", &DictionaryInfo::type)
        .def_readonly("lsize", &DictionaryInfo::lsize)
        .def_readonly("rsize", &DictionaryInfo::rsize)
        .def_readonly("version", &DictionaryInfo::version)
        .def("__repr__", [](const DictionaryInfo &info) {
            std::stringstream result;
            result << "<DictionaryInfo filename=" << info.filename << ", charset=" << info.charset << ", size=" << info.size
                << ", type=" << info.type << ", lsize=" << info.lsize << ", rsize=" << info.rsize << ", version=" << info.version << ">";
            return result.str();
        });

    py::class_<Node>(m, "Node")
        .def_readonly("surface", &Node::surface)
        .def_readonly("feature", &Node::feature)
        .def_readonly("id_", &Node::id)
        .def_readonly("length", &Node::length)
        .def_readonly("rlength", &Node::rlength)
        .def_readonly("rcAttr", &Node::rcAttr)
        .def_readonly("lcAttr", &Node::lcAttr)
        .def_readonly("posid", &Node::posid)
        .def_readonly("char_type", &Node::char_type)
        .def_readonly("stat", &Node::stat)
        .def_readonly("isbest", &Node::isbest)
        .def_readonly("alpha", &Node::alpha)
        .def_readonly("beta", &Node::beta)
        .def_readonly("prob", &Node::prob)
        .def_readonly("wcost", &Node::wcost)
        .def_readonly("cost", &Node::cost)
        .def("__repr__", [](const Node &node) {
            std::stringstream result;
            result << "<Node surface=\"" << node.surface << "\", feature=\"" << node.feature << "\">";
            return result.str();
        });

    py::class_<Tagger>(m, "Tagger")
        .def(py::init<const std::string &>())
        .def("get_dictionary_info", &Tagger::get_dictionary_info)
        .def("parse", &Tagger::parse)
        .def("parse_node_with_lattice", &Tagger::parse_node_with_lattice)
        .def("parse_nbest_with_lattice", &Tagger::parse_nbest_with_lattice);
}
