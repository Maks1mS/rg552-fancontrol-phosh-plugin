#!/bin/bash

pc_file=$(find "/usr/lib64/pkgconfig" -maxdepth 1 -type f -name "libphosh-*.pc" | head -n 1)

if [ -z "$pc_file" ]; then
  echo "File libphosh-*.pc not found."
  exit 1
fi

version=$(basename "$pc_file" | sed -E 's/^libphosh-([0-9.]+)\.pc$/\1/')

if [ -z "$version" ]; then
  echo "Can't detect version of $pc_file."
  exit 1
fi

echo $version
exit 0
