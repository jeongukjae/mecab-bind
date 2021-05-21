#!/bin/bash
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
# I modified this script not to use CUDA settings for this project.
#
PLATFORM="$(uname -s | tr 'A-Z' 'a-z')"

if [[ -z ${PIP} ]]; then
  PIP="pip3"
fi

if [[ -z ${PYTHON} ]]; then
  PYTHON="python3"
fi

function write_to_bazelrc() {
  echo "$1" >> .bazelrc
}

function write_action_env_to_bazelrc() {
  write_to_bazelrc "build --action_env $1=\"$2\""
}

function is_linux() {
  [[ "${PLATFORM}" == "linux" ]]
}

function is_macos() {
  [[ "${PLATFORM}" == "darwin" ]]
}

function is_windows() {
  # On windows, the shell script is actually running in msys
  [[ "${PLATFORM}" =~ msys_nt*|mingw*|cygwin*|uwin* ]]
}

function is_ppc64le() {
  [[ "$(uname -m)" == "ppc64le" ]]
}


# Remove .bazelrc if it already exist
[ -e .bazelrc ] && rm .bazelrc

# Check if it's installed
if [[ $(${PIP} show tensorflow-cpu) == *tensorflow-cpu* ]] || [[ $(${PIP} show tf-nightly-cpu) == *tf-nightly-cpu* ]] ; then
  echo 'Using installed tensorflow'
else
  # Uninstall GPU version if it is installed.
  if [[ $(${PIP} show tensorflow) == *tensorflow* ]]; then
    echo 'Already have gpu version of tensorflow installed. Uninstalling......\n'
    ${PIP} uninstall tensorflow
  elif [[ $(${PIP} show tf-nightly) == *tf-nightly* ]]; then
    echo 'Already have gpu version of tensorflow installed. Uninstalling......\n'
    ${PIP} uninstall tf-nightly
  fi
  # Install CPU version
  echo 'Installing tensorflow-cpu......\n'
  ${PIP} install tensorflow-cpu
fi


TF_CFLAGS=( $(${PYTHON} -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_compile_flags()))') )
TF_LFLAGS="$(${PYTHON} -c 'import tensorflow as tf; print(" ".join(tf.sysconfig.get_link_flags()))')"

write_to_bazelrc "build -c opt"

# MSVC (Windows): Standards-conformant preprocessor mode
# See https://docs.microsoft.com/en-us/cpp/preprocessor/preprocessor-experimental-overview
if is_windows; then
  write_to_bazelrc "build --copt=/experimental:preprocessor"
  write_to_bazelrc "build --host_copt=/experimental:preprocessor"
fi

if is_windows; then
  # Use pywrap_tensorflow instead of tensorflow_framework on Windows
  SHARED_LIBRARY_DIR=${TF_CFLAGS:2:-7}"python"
else
  SHARED_LIBRARY_DIR=${TF_LFLAGS:2}
fi
SHARED_LIBRARY_NAME=$(echo $TF_LFLAGS | rev | cut -d":" -f1 | rev)
if ! [[ $TF_LFLAGS =~ .*:.* ]]; then
  if is_macos; then
    SHARED_LIBRARY_NAME="libtensorflow_framework.dylib"
  elif is_windows; then
    # Use pywrap_tensorflow's import library on Windows. It is in the same dir as the dll/pyd.
    SHARED_LIBRARY_NAME="_pywrap_tensorflow_internal.lib"
  else
    SHARED_LIBRARY_NAME="libtensorflow_framework.so"
  fi
fi

HEADER_DIR=${TF_CFLAGS:2}
if is_windows; then
  SHARED_LIBRARY_DIR=${SHARED_LIBRARY_DIR//\\//}
  SHARED_LIBRARY_NAME=${SHARED_LIBRARY_NAME//\\//}
  HEADER_DIR=${HEADER_DIR//\\//}
fi
write_action_env_to_bazelrc "TF_HEADER_DIR" ${HEADER_DIR}
write_action_env_to_bazelrc "TF_SHARED_LIBRARY_DIR" ${SHARED_LIBRARY_DIR}
write_action_env_to_bazelrc "TF_SHARED_LIBRARY_NAME" ${SHARED_LIBRARY_NAME}
write_action_env_to_bazelrc "TF_NEED_CUDA" "0"
