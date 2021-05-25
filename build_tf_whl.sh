#!/usr/bin/env bash

set -e
set -x

if [[ -z ${PYTHON} ]]; then
  PYTHON="python3"
fi

if [[ -z ${TF_VERSION} ]]; then
  TF_VERSION="2.5.0"
fi

mkdir -p .tmp-py-bin
rm -f .tmp-py-bin/python3
ln -s `which $PYTHON` .tmp-py-bin/python3

$PYTHON -m pip install tensorflow-cpu==${TF_VERSION}
PIP="$PYTHON -m pip" PYTHON=$PYTHON ./tf_configure.sh
bazel build -s \
    --action_env=PATH=$PATH:`pwd`/.tmp-py-bin \
    //tf_ops:build_pip_pkg \
    --verbose_failures

PATH=$PATH:`pwd`/.tmp-py-bin ./bazel-bin/tf_ops/build_pip_pkg artifacts
