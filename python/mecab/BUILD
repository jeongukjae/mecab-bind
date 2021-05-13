package(default_visibility = ["//python:__pkg__"])
load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

pybind_extension(
    name = "bind",
    srcs = ["bind.cpp"],
    deps = ["//:libmecab"],
)

py_library(
    name = "mecab",
    srcs = ['__init__.py'],
    data = [":bind.so"],
)
