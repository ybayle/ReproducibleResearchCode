#!/bin/bash

# TODO: the script should give an error if less than three args are given.

MODELS=$1 # a string that represent models filenames separated by
          # `:'. Models filenames should start with the model name
          # (class or tag name) followed by a dot. ex:
          # <tag-name>.mm. Note that models produced by trainmodels.sh
          # follow this rule: the name is composed by:
          # <tag-name>.<codebook-name>.mm 
CODEBOOKNAME=$2 # the file from which the codebook is read. (see makecodebook.sh)
                # CAL500.r34.s100.cbk (random seed = 34, size = 100).
DATASET=$3 # a file that contains a list of filenames, one per
           # line. Each filename contains the sequence of feature
           # vectors for an audio piece.  

# make vqmm
# if [ "$?" -eq 0 ] 
# then
    vqmm -output-dir ./Results/ -classify -models $MODELS -codebook $CODEBOOKNAME -list-of-files $DATASET 
# fi

# Example:
# ./testclassmodels.sh /tmp/t/ISMIR2004.r50.s25.fold1.cbkISMIR2004.ALL.YMFCC2.r50.s25.models.csv cbk/ISMIR2004.ALL.YMFCC2.r50.s25.cbk /tmp/t/ISMIR2004.r50.fold2.cbkISMIR2004.ALL.YMFCC2.r50.s25.csv
