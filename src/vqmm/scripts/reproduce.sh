#!/bin/bash

DIR=/tmp/reproduce
# The following URLs point to the data and scripts used to reproduce
# the results. If for some reason the script fails to download the
# file, you may experiment an alternative my modifying the line
# indicated below.
DATAMFCC1="https://dl.dropboxusercontent.com/u/886471/reproduce-cal500-ymfcc2-scripts.tgz"
DATAMFCC2="http://ubuntuone.com/55DUskcXvj25ANAyQuzdxu"
UONE_STRANGENAME="55DUskcXvj25ANAyQuzdxu"
DATAMFCC3="http://inescporto.pt/~fgouyon/data/ConfoundsPaper/reproduce-cal500-ymfcc2-scripts.tgz"
DATAMFCC=$DATAMFCC1 # <- this line may be modified if download fails.
TARFILE="reproduce-cal500-ymfcc2-scripts.tgz"

mkdir $DIR
cd $DIR
echo "Fetch last version of VQMM:"
wget https://bitbucket.org/ThibaultLanglois/vqmm/get/tip.zip
unzip tip.zip
mv ThibaultLanglois*/*.* .
mv ThibaultLanglois*/Makefile .
mv ThibaultLanglois*/README .
rm -Rf ThibaultLanglois*/
echo "Make VQMM executable."
make
wget $DATAMFCC
if ["$DATAMFCC" == "$DATAMFCC2" ] # Ubuntu One does not keep the
                                # original filename when downloading
                                # with wget
then
   mv $UONE_STRANGENAME $TARFILE
fi
tar -xzvf $TARFILE
echo "Let's make a codebook..."
mkdir cbk
./makecodebook.sh 50 75 ./ALLCAL500.YMFCC2.csv ./cbk/CAL500.ALL.YMFCC2.r50.s75.cbk
echo "codebook done."
echo "The script train-models-CAL500.TOP97.r50.s75.cbkCAL500.ALL.YMFCC2.r50.s75.sh trains"
echo "models for the five permutations of folds."
echo "Start training models !!! EXPECT ~4 HOURS OF COMPUTATION ..."
./train-models-CAL500.TOP97.r50.s75.cbkCAL500.ALL.YMFCC2.r50.s75.sh
echo "Finished training models."
echo "Start testing models..."
./test-models-CAL500.TOP97.r50.s75.cbkCAL500.ALL.YMFCC2.r50.s75.sh
echo "Finished testing models..."
echo "The evaluation of the results appear in the files:"
echo "./Results/CAL500.TOP97.r50.fold1.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv"
echo "./Results/CAL500.TOP97.r50.fold2.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv"
echo "./Results/CAL500.TOP97.r50.fold3.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv"
echo "./Results/CAL500.TOP97.r50.fold4.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv"
echo "./Results/CAL500.TOP97.r50.fold5.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv"
echo "For example:"
cat ./Results/CAL500.TOP97.r50.fold1.cbkCAL500.ALL.YMFCC2.r50.s75.summary1.csv
echo "The line above should be similar to the following:"
echo "CAL500.ALL.YMFCC2.r50.s75.cbk,CAL500.TOP97.r50.fold1.cbkCAL500.ALL.YMFCC2.r50.s75.csv,0.343264,0.446330,0.390784"
echo "Where the first string correspond to the data set used for testing and:"
echo "# Average per-tag performance:"
echo "# Precision: 0.343264"
echo "# Recall 0.446330"
echo "# F-Score: 0.390784"
echo ""
echo "Any question ? -> tl@di.fc.ul.pt"
