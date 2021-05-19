package(default_visibility = ["//visibility:public"])
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

# set rpath in linux
# because libmecab is built as a shared library in Linux, and as a static library in macOS
pybind_extension(
    name = "bind",
    srcs = ["bind.cpp"],
    deps = ["//:libmecab"],
    linkopts = select({
        "//:osx": [],
        "//conditions:default": ["-Wl,-rpath='$$ORIGIN'"],
    })
)

py_library(
    name = "mecab",
    srcs = [
        "__init__.py",
        "c.py",
    ],
    data = [":bind.so"],
)

py_test(
    name = "test_mecab",
    srcs = ["test_mecab.py"],
    deps = ["//python/mecab:mecab"],
    data = [
        "//python/mecab:bind.so",
        "//:libmecab",
        "//:test-data",
    ],
    imports = [".."],
)
