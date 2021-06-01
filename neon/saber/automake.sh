#!/bin/bash

git pull origin master
make clean 
make test/test_speed
