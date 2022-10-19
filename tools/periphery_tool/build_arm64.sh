#!/bin/sh

if [ ! -d out ]
then
	cmake -S . -B out -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
fi

cmake --build out

