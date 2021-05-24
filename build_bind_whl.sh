#!/usr/bin/env bash

set -e
set -x

if [[ -z ${PIP} ]]; then
  PIP="pip3"
fi

if [[ -z ${PYTHON} ]]; then
  PYTHON="python3"
fi

mkdir -p .tmp-py-bin
rm -f .tmp-py-bin/python3
ln -s `which $PYTHON` .tmp-py-bin/python3

bazel build -s \
    --action_env=PATH=$PATH:`pwd`/.tmp-py-bin \
    //python:build_pip_pkg \
    --verbose_failures

PATH=$PATH:`pwd`/.tmp-py-bin ./bazel-bin/python/build_pip_pkg artifacts
