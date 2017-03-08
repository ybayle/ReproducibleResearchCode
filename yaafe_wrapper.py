# -*- coding: utf-8 -*-

"""Description of yaafe_feat_extraction
yaafe.py -r 22050 -f "mfcc: MFCC blockSize=2048 stepSize=1024" audio_fn.txt
"""
import os
import sys
sys.path.insert(0, './src/')
import utils

dir_tracks="tracks/"
utils.print_success("YAAFE features extraction (approx. 8 minutes)")

# Assert Python version
if sys.version_info.major != 2:
    utils.print_error("Yaafe needs Python 2 environment")

# Assert folder exists
dir_tracks = utils.abs_path_dir(dir_tracks)    

filelist = os.listdir(dir_tracks)
dir_feat = utils.create_dir(utils.create_dir("features") + "database1")
# dir_tmp = utils.create_dir("tmp")
# dir_yaafe = utils.create_dir(dir_tmp + "yaafe")
# fn_filelist = dir_yaafe + "filelist.txt"
dir_current = os.getcwd()
os.chdir(dir_tracks)
yaafe_cmd = 'yaafe -r 22050 -f "mfcc: MFCC blockSize=2048 stepSize=1024" '
yaafe_cmd += "--resample -b " + dir_feat + " "
for index, filen in enumerate(filelist):
    utils.print_progress_start(str(index+1) + "/" + str(len(filelist)) + " " + filen)
    os.system(yaafe_cmd + filen + "> /dev/null 2>&1")
utils.print_progress_end()
os.chdir(dir_current)
