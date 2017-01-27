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
  vqmm -quiet y -output-dir $OUTPUTDIR -list-of-files $DATASET -epsilon 0.00001 -codebook $CODEBOOKNAME -make-tag-models    
# fi

# Example:
# ./traintagmodels.sh cbk/ALLCAL500.YMFCC2.r1.s25.cbk /tmp/VOCALS-NONVOCALS.YMFCC2.fold1.csv ./Models/

# Train CAL500:
# ./traintagmodels.sh cbk/ALLCAL500.YMFCC2.r50.s25.cbk /tmp/CALL500.TOP97.r50.fold2345.csv ./Models/
