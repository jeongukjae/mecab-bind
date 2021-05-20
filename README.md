# mecab-bind

* mecab-bind: ![PyPI - License](https://img.shields.io/pypi/l/mecab-bind) ![PyPI](https://img.shields.io/pypi/v/mecab-bind)
* mecab-tf: ![PyPI - License](https://img.shields.io/pypi/l/mecab-tf) ![PyPI](https://img.shields.io/pypi/v/mecab-tf)

[![Build and test binding modules](https://github.com/jeongukjae/mecab-bind/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/jeongukjae/mecab-bind/actions/workflows/build-and-test.yml)

Binding MeCab Tagger to python and tensorflow

## Installation

* Python binding: `pip install mecab-bind`
* TensorFlow binding: `pip install mecab-tf`

### Compatible TensorFlow version

|mecab-tf|tensorflow version|python version|
|---|---|---|

## Usage

### Python Binding

```python
>>> import mecab
>>> tagger = mecab.Tagger(mecab.get_model_args("./test-data/dic")) # pass dictionary path instead of "./test-data/dic"
>>> dic_infos = tagger.get_dictionary_info()
>>> tagger.get_dictionary_info()
[<DictionaryInfo filename=./test-data/dic/sys.dic, charset=UTF-8, size=4335, type=0, lsize=346, rsize=346, version=102>]
>>> tagger.parse_node_with_lattice("シリーズ中、カンフーシーンが一番多い。")
[
    <Node surface="", feature="BOS/EOS,*,*,*,*,*,*,*,*">,
    <Node surface="シリーズ", feature="名詞,一般,*,*,*,*,*">,
    <Node surface="中", feature="接頭詞,数接続,*,*,*,*,中,ナカ,ナカ">,
    <Node surface="、", feature="記号,読点,*,*,*,*,、,、,、">,
    <Node surface="カンフーシーン", feature="名詞,一般,*,*,*,*,*">,
    <Node surface="が", feature="助詞,格助詞,一般,*,*,*,が,ガ,ガ">,
    <Node surface="一番", feature="名詞,副詞可能,*,*,*,*,一番,イチバン,イチバン">,
    <Node surface="多い", feature="形容詞,自立,*,*,形容詞・アウオ段,基本形,多い,オオイ,オーイ">,
    <Node surface="。", feature="記号,句点,*,*,*,*,。,。,。">,
    <Node surface="", feature="BOS/EOS,*,*,*,*,*,*,*,*">
]
>>> tagger.parse_nbest_with_lattice("シリーズ中、カンフーシーンが一番多い。")
[
    [
        <Node surface="", feature="BOS/EOS,*,*,*,*,*,*,*,*">,
        <Node surface="シリーズ", feature="名詞,一般,*,*,*,*,*">,
        <Node surface="中", feature="接頭詞,数接続,*,*,*,*,中,ナカ,ナカ">,
        ...
    ],
    [
        <Node surface="", feature="BOS/EOS,*,*,*,*,*,*,*,*">,
        <Node surface="シリーズ", feature="名詞,一般,*,*,*,*,*">,
        <Node surface="中", feature="接頭詞,数接続,*,*,*,*,中,ナカ,ナカ">,
        <Node surface="、", feature="記号,読点,*,*,*,*,、,、,、">,
        ...
    ],
    ...
]
>>> print(tagger.parse("シリーズ中、カンフーシーンが一番多い。"))
シリーズ        名詞,一般,*,*,*,*,*
中      接頭詞,数接続,*,*,*,*,中,ナカ,ナカ
、      記号,読点,*,*,*,*,、,、,、
カンフーシーン  名詞,一般,*,*,*,*,*
が      助詞,格助詞,一般,*,*,*,が,ガ,ガ
一番    名詞,副詞可能,*,*,*,*,一番,イチバン,イチバン
多い    形容詞,自立,*,*,形容詞・アウオ段,基本形,多い,オオイ,オーイ
。      記号,句点,*,*,*,*,。,。,。
EOS

```

Bound commands

* `mecab-dict-index`
* `mecab-dict-gen`
* `mecab-system-eval`
* `mecab-cost-train`
* `mecab-test-gen`
* `mecab`

### TensorFlow Binding

```python
>>> import tensorflow as tf
>>> from mecab_tf.python.ops.mecab_ops import MecabTagger
>>> tagger = MecabTagger("./test-data/dic")
2021-05-20 05:35:48.759933: I tensorflow/core/platform/cpu_feature_guard.cc:142] This TensorFlow binary is optimized with oneAPI Deep Neural Network Library (oneDNN) to use the following CPU instructions in performance-critical operations:  AVX2 FMA
To enable them in other operations, rebuild TensorFlow with the appropriate compiler flags.
>>> surfaces, features = tagger.tag(["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"])
>>> surfaces.shape
TensorShape([2, None])
>>> features.shape
TensorShape([2, None])
>>> for surface, feature in zip(surfaces[0], features[0]):  # print first sentence
...     print(surface.numpy().decode('utf8'), feature.numpy().decode('utf8'))
...
 BOS/EOS,*,*,*,*,*,*,*,*
シリーズ 名詞,一般,*,*,*,*,*
中 接頭詞,数接続,*,*,*,*,中,ナカ,ナカ
、 記号,読点,*,*,*,*,、,、,、
カンフーシーン 名詞,一般,*,*,*,*,*
が 助詞,格助詞,一般,*,*,*,が,ガ,ガ
一番 名詞,副詞可能,*,*,*,*,一番,イチバン,イチバン
多い 形容詞,自立,*,*,形容詞・アウオ段,基本形,多い,オオイ,オーイ
。 記号,句点,*,*,*,*,。,。,。
 BOS/EOS,*,*,*,*,*,*,*,*
>>> # you can pass any shape of string tensor
>>> _ = tagger.tag("シリーズ中、カンフーシーンが一番多い。")
>>> _ = tagger.tag([["シリーズ中、カンフーシーンが一番多い。", "※撮影中に、ジェット・リーが失踪。"]])
```

Note: If you use this Module in SavedModel format, it is recommended to use model_path as absolute path.
      The `model_path` is serialized, not the dictionary data.
