#!/bin/bash

TARGET_DIR=dist
TARGET_EXTS='*.map *.woff'

echo "====================================================="
echo "cleaning unused files to reduce size of dist"
echo "====================================================="

for ext in $TARGET_EXTS
do
  echo "removing ${ext}..."
  find ${TARGET_DIR} -name "${ext}" -exec rm -f {} \;
done

echo "done cleaning unused files"
