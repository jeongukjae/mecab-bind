#!/bin/bash

RUNFILES_DIR=$(pwd)

cp -pf "${RUNFILES_DIR}/python/mecab/bind.so" "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/bind.so"
cp -pf ${BUILD_WORKSPACE_DIRECTORY}/bazel-bin/libmecab/lib/* "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/"
cp -pf ${BUILD_WORKSPACE_DIRECTORY}/bazel-bin/libmecab/include/* "${BUILD_WORKSPACE_DIRECTORY}/python/mecab/include/"
