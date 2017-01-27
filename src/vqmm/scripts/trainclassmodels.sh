#!/bin/bash

# TODO: the script should give an error if less than two args is given.

CODEBOOKNAME=$1 # the file from which the codebook is read. See makecodebook.sh.
DATASET=$2 # a file that contains a list of filenames, one per
           # line. Each filename contains the sequence of feature
           # vectors for an audio piece.  
OUTPUTDIR=$3 # the directory where the models must be saved.

# make vqmm
# if [ "$?" -eq 0 ] 
# then
  vqmm -quiet y -output-dir $OUTPUTDIR -list-of-files $DATASET -epsilon 0.00001 -codebook $CODEBOOKNAME -make-class-models    
# fi

# Example:
# ./trainclassmodels.sh cbk/ISMIR2004.ALL.YMFCC2.r50.s25.cbk  /tmp/ISMIR2004.r50.fold1.csv ./Models/ISMIR2004-r50.s25/
