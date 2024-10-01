#!/usr/bin/env bash

for ARCH in x86 x64 armv6m armv7m armv7emsp armv7emdp xtensa xtensawin; do
  echo "--- Building $ARCH ---"
  rm -f lbm.$ARCH.mpy
  make clean all ARCH=$ARCH 2>&1 >/dev/null
  mv lbm.mpy lbm.$ARCH.mpy
done
