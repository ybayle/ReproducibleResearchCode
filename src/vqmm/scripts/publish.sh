cd ./Results
cat *summary1* | sort > All-results.csv
cd ..
rm -f ~/Dropbox/Public/vqmm.zip
zip ~/Dropbox/Public/vqmm.zip *.c *.h Makefile makecodebook.sh traintagmodels.sh  testtagmodels.sh trainclassmodels.sh  testclassmodels.sh
rm -f ~/Dropbox/Public/VQMM-Results.zip
zip ~/Dropbox/Public/VQMM-Results.zip Results/*
