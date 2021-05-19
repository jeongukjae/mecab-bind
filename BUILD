package(default_visibility = [":__subpackages__"])

load("@rules_foreign_cc//foreign_cc:defs.bzl", "configure_make")

config_setting(
    name = "osx",
    constraint_values = ["@platforms//os:osx"],
)

# https://github.com/bazelbuild/rules_foreign_cc/blob/main/docs/README.md#configure_make
# http://taku910.github.io/mecab/#install
configure_make(
    name = "libmecab",
    configure_env_vars = {"AR": "/usr/bin/ar"},
    lib_source = "@libmecab//:all_src",
    out_static_libs = select({
        ":osx": ["libmecab.a"],
        "//conditions:default": [],
    }),
    # TODO: Building libmecab as a static library in Linux raises error like `recompile with -fPIC`.
    # I want to build libmecab as a static library in all platform.
    out_shared_libs = select({
        ":osx": [],
        "//conditions:default": ["libmecab.so.2"],
    }),
)

filegroup(
    name = "test-data",
    srcs = glob(["test-data/**/*"]),
)
