#!/bin/bash
DIR=build_reg
mkdir -p $DIR
cd $DIR
cmake ../reg
make
