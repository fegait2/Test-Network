#!/bin/sh

./configure --prefix=../sdk

make install

mkdir -p ../sdk/include
cp rtp_api.h ../src/inc
cp rtp_highlevel.h ../src/inc
