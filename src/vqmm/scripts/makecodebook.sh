#!/bin/bash

# TODO: the script should give an error if less than four args is given.

RANDOMSEED=$1
CODEBOOKSIZE=$2
DATASET=$3 # a file that contains a list of filenames, one per
           # line. Each filename contains the sequence of feature
           # vectors for an audio piece.  
CODEBOOKNAME=$4 # the file where the codebook is written. TIP: in the
                # filename it is a good idea to include the parameters
                # that were used to make the codebook ex:
                # CAL500.r34.s100.cbk (random seed = 34, size = 100).

#make vqmm
#if [ "$?" -eq 0 ] 
#then
    vqmm -quiet y -list-of-files $DATASET -random $RANDOMSEED -codebook-size $CODEBOOKSIZE -codebook $CODEBOOKNAME
#fi

# Example:
# ./makecodebook.sh 1 25 /tmp/ALLCAL500.YMFCC2.csv /home/tl/Projects/VQMM/cbk/ALLCAL500.YMFCC2.r1.s25.cbk
