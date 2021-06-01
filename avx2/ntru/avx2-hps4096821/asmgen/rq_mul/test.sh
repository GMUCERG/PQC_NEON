#!/bin/bash

python3 test.py > out_test
python3 schoolbook_64x13.py > out 

diff out out_test