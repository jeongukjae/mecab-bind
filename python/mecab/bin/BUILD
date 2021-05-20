package(default_visibility = ["//visibility:public"])

cc_binary(
    name = "mecab-dict-index",
    srcs = ["@libmecab//:mecab_dict_index"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": [],
    }),
)
cc_binary(
    name = "mecab-dict-gen",
    srcs = ["@libmecab//:mecab_dict_gen"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": [],
    }),
)
cc_binary(
    name = "mecab-system-eval",
    srcs = ["@libmecab//:mecab_system_eval"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": [],
    }),
)
cc_binary(
    name = "mecab-cost-train",
    srcs = ["@libmecab//:mecab_cost_train"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": ["-lpthread"],
    }),
)
cc_binary(
    name = "mecab-test-gen",
    srcs = ["@libmecab//:mecab_test_gen"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": [],
    }),
)
cc_binary(
    name = "mecab",
    srcs = ["@libmecab//:mecab"],
    deps = ["//:libmecab"],
    linkopts = select({
        ":osx": ['-liconv'],
        "//conditions:default": [],
    }),
)

py_library(
    name = 'bin',
    srcs = ['__init__.py', 'cli_main.py'],
    data = [
        ":mecab-dict-index",
        ":mecab-dict-gen",
        ":mecab-system-eval",
        ":mecab-cost-train",
        ":mecab-test-gen",
        ":mecab",
    ]
)
