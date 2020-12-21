#!/bin/bash

rm -rf ${HOME}/.cache/deepin/deepin-music/*

mkdir build
cd build
cmake ..
make test -j8

cd ./../../
rm -rf ./build-ut
mkdir build-ut
cd build-ut

cp -r ./../tests/build/coverageResult/report/ ./
mv report html
cd html
mv index.html cov_deepin-music.html

cd ..
mkdir report
cd report
cp ./../../tests/build/report/report_deepin-music.xml ./

#cd ..
#cp ./../tests/build/asan_deepin-music.log* ./

exit 0
