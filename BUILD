package(default_visibility = [":__subpackages__"])

load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")

# https://github.com/bazelbuild/rules_foreign_cc/blob/main/docs/README.md#configure_make
configure_make(
    name = "libmecab",
    configure_env_vars = {"AR": "/usr/bin/ar"},
    lib_source = "@libmecab//:all_src",
    out_static_libs = ["libmecab.a"],
)

filegroup(
    name = "test-data",
    srcs = glob(["test-data/**/*"]),
)
