import subprocess
import os
import sys


def mecab_dict_index_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab-dict-index")] + sys.argv)


def mecab_dict_gen_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab-dict-gen")] + sys.argv)


def mecab_system_eval_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab-system-eval")] + sys.argv)


def mecab_cost_train_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab-cost-train")] + sys.argv)


def mecab_test_gen_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab-test-gen")] + sys.argv)


def mecab_main():
    return subprocess.call([os.path.join(os.path.dirname(__file__), "mecab")] + sys.argv)
