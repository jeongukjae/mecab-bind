#!/usr/bin/env bash
# Copyright 2018 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

#
# Modified version to build python binding module
#
set -e
set -x

PLATFORM="$(uname -s | tr 'A-Z' 'a-z')"
function is_windows() {
  if [[ "${PLATFORM}" =~ (cygwin|mingw32|mingw64|msys)_nt* ]]; then
    true
  else
    false
  fi
}

LIBMECAB_PREFIX="bazel-bin/libmecab/"
if is_windows; then
  PIP_FILE_PREFIX="bazel-bin/python/build_pip_pkg.exe.runfiles/mecab-bind/"
else
  PIP_FILE_PREFIX="bazel-bin/python/build_pip_pkg.runfiles/mecab-bind/"
fi

function main() {
  while [[ ! -z "${1}" ]]; do
    if [[ ${1} == "make" ]]; then
      echo "Using Makefile to build pip package."
      PIP_FILE_PREFIX=""
    else
      DEST=${1}
    fi
    shift
  done

  if [[ -z ${DEST} ]]; then
    echo "No destination dir provided"
    exit 1
  fi

  # Create the directory, then do dirname on a non-existent file inside it to
  # give us an absolute paths with tilde characters resolved to the destination
  # directory.
  mkdir -p ${DEST}
  if [[ ${PLATFORM} == "darwin" ]]; then
    DEST=$(pwd -P)/${DEST}
  else
    DEST=$(readlink -f "${DEST}")
  fi
  echo "=== destination directory: ${DEST}"

  TMPDIR=$(mktemp -d -t tmp.XXXXXXXXXX)

  echo $(date) : "=== Using tmpdir: ${TMPDIR}"

  echo "=== Copy Python binding files"

  cp ${PIP_FILE_PREFIX}python/setup.py "${TMPDIR}"
  cp ${PIP_FILE_PREFIX}python/LICENSE "${TMPDIR}"
  cp ${PIP_FILE_PREFIX}python/README.md "${TMPDIR}"
  cp ${PIP_FILE_PREFIX}python/MANIFEST.in "${TMPDIR}"
  rsync -avm -L --exclude='test_*.py' ${PIP_FILE_PREFIX}python/mecab "${TMPDIR}"

  pushd ${TMPDIR}
  echo $(date) : "=== Building wheel"

  python3 setup.py bdist_wheel

  cp dist/*.whl "${DEST}"
  popd
  rm -rf ${TMPDIR}
  echo $(date) : "=== Output wheel file is in: ${DEST}"
}

main "$@"
