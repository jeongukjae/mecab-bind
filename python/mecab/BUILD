package(default_visibility = ["//python:__pkg__"])
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
