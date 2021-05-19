#!/bin/bash

set -e

RUNFILES_DIR=$(pwd)

cp -pf "${RUNFILES_DIR}/python/mecab/bind.so" "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/bind.so"
cp -pf ${BUILD_WORKSPACE_DIRECTORY}/bazel-bin/libmecab/lib/* "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/"
mkdir -p "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/include/"
cp -pf ${BUILD_WORKSPACE_DIRECTORY}/bazel-bin/libmecab/include/* "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/include/"
