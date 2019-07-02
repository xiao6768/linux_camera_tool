#!/bin/bash
DIR=build
mkdir -p $DIR
cd $DIR
cmake ..
make
