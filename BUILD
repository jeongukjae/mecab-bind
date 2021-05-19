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
    configure_env_vars = select({
        ":osx": {"AR": "/usr/bin/ar"},
        "//conditions:default": {"AR": "/usr/bin/ar", "CC": "gcc -fPIC", "CXX": "gcc -fPIC"},
    }),
    make_commands = ["make -j3", "make install"],
    lib_source = "@libmecab//:all_src",
    out_static_libs = ["libmecab.a"],
)

filegroup(
    name = "test-data",
    srcs = glob(["test-data/**/*"]),
)
