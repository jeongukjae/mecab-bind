workspace(name = "mecab-bind")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Load rules_foreign_cc to build mecab
http_archive(
    name = "rules_foreign_cc",
    sha256 = "d54742ffbdc6924f222d2179f0e10e911c5c659c4ae74158e9fe827aad862ac6",
    strip_prefix = "rules_foreign_cc-0.2.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.2.0.tar.gz",
)
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies()

# Load pybind
http_archive(
    name = "pybind11_bazel",
    strip_prefix = "pybind11_bazel-26973c0ff320cb4b39e45bc3e4297b82bc3a6c09",
    sha256 = "a5666d950c3344a8b0d3892a88dc6b55c8e0c78764f9294e806d69213c03f19d",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/26973c0ff320cb4b39e45bc3e4297b82bc3a6c09.zip"],
)
http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    strip_prefix = "pybind11-2.6.2",
    sha256 = "8ff2fff22df038f5cd02cea8af56622bc67f5b64534f1b83b9f133b8366acff2",
    urls = ["https://github.com/pybind/pybind11/archive/v2.6.2.tar.gz"],
)
load("@pybind11_bazel//:python_configure.bzl", "python_configure")
python_configure(name = "local_config_python", python_version = "3")

# mecab
http_archive(
    name = "libmecab",
    urls = ["https://github.com/taku910/mecab/archive/046fa78b2ed56fbd4fac312040f6d62fc1bc31e3.tar.gz"],
    strip_prefix = "mecab-046fa78b2ed56fbd4fac312040f6d62fc1bc31e3/mecab",
    sha256 = "1c14f66ead753f80f6ff139f453ba9e5b79e7d267e81c4a51b669d354b48e851",
    build_file_content = """
filegroup(name = "all_src", srcs = glob(["**"]), visibility = ["//visibility:public"])

# clis
filegroup(name = "mecab_dict_index", srcs = ["src/mecab-dict-index.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
filegroup(name = "mecab_dict_gen", srcs = ["src/mecab-dict-gen.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
filegroup(name = "mecab_system_eval", srcs = ["src/mecab-system-eval.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
filegroup(name = "mecab_cost_train", srcs = ["src/mecab-cost-train.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
filegroup(name = "mecab_test_gen", srcs = ["src/mecab-test-gen.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
filegroup(name = "mecab", srcs = ["src/mecab.cpp", "src/winmain.h"], visibility = ["//visibility:public"])
"""
)

# Load TensorFlow
load("//third_party/tf:tf_configure.bzl", "tf_configure")
tf_configure(name = "local_config_tf")
