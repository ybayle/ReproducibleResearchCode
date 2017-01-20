#!/bin/bash
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   19/01/2017
# Updated   19/01/2017
# Version   1.0.0
# Object    Clean all files generated by reproduciblity.py
#

if [ -d "./tmp" ]; then
    rm -r ./tmp
fi
if [ -d "./__pycache__" ]; then
    rm -r ./__pycache__
fi
if [ -d "./features" ]; then
    rm -r ./features
fi
if [ -d "./results" ]; then
    rm -r ./results
fi
if [ -f bextract_single.mf ]; then
    rm bextract_single.mf
fi
if [ -f track_fn.mf ]; then
    rm track_fn.mf
fi
if [ -f *.pyc ]; then
    rm *.pyc
fi
