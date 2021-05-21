#!/usr/bin/env bash
set -e
set -x

TF_SHARED_LIBRARY_NAME=$(grep -r TF_SHARED_LIBRARY_NAME .bazelrc | awk -F= '{print$2}')
POLICY_JSON=$(find /opt -name policy.json)
sed -i "s/libresolv.so.2\"/libresolv.so.2\", $TF_SHARED_LIBRARY_NAME/g" $POLICY_JSON
cat $POLICY_JSON
