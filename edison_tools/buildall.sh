#!/bin/sh

CUR=$PWD
OUTPUT=$CUR/all_built

mkdir -p $OUTPUT

echo $CUR
echo "building check5611..."
cd check5611
mkdir -p build
cd build
cmake ..
make
cp check5611 $OUTPUT
cd $CUR
echo "check5611 done."


echo "buildiing 5883..."
cd check5883
mkdir -p build
cd build
cmake ..
make
cp check5883 $OUTPUT
cd $CUR
echo "check5883 done."

echo "buildiing 6050..."
cd check6050
mkdir -p build
cd build
cmake ..
make
cp check6050 $OUTPUT
cd $CUR
echo "check6050 done."

echo "buildiing mraaio..."
cd mraaio
mkdir -p build
cd build
cmake ..
make
cp mraaio $OUTPUT
cd $CUR
echo "mraaio done."
echo "all done."
