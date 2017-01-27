# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   19/01/2017
# Updated   20/01/2017
# Version   1.0.0
#

"""
Description of reproducibility.py
=================================

Launch source code file in order to reproduce results of the article

:Example:

python reproducibility.py


"""

import os
import sys
import isrc
import utils
import ghosal
import svmbff
# import urllib

def clean_filenames(tracks_dir = "tracks/"):
    """Description of clean_filenames
    """
    for old_fn in os.listdir(tracks_dir):
        new_fn = old_fn
        new_fn = new_fn.replace(" ", "_")
        new_fn = new_fn.replace(",", "_")
        new_fn = new_fn.replace("(", "_")
        new_fn = new_fn.replace(")", "_")
        os.rename(tracks_dir + old_fn, tracks_dir + new_fn)

def yaafe_feat_extraction(dir_tracks):
    """Description of yaafe_feat_extraction
    yaafe.py -r 22050 -f "mfcc: MFCC blockSize=2048 stepSize=1024" audio_fn.txt
    """
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

def main():
    """Description of main
    """

    utils.print_success("Reproducible research (approx. 30 minutes)")
    utils.print_warning("You need at least 150Go of free space")
    utils.print_warning("and to store tracks in a dir named: tracks/")

    # Variables
    # groundtruths_filename = "groundtruths.csv"
    # results_dir = "results/"
    # utils.create_dir(results_dir)
    
    # # Figure 1
    # isrc.plot_isrc_year_distribution(groundtruths_filename, results_dir)
    
    # # Figure 2
    # isrc.plot_isrc_country_repartition(groundtruths_filename, results_dir)

    # TODO
    # instead of using my own wav processed file, download and compute the one from scientists website 

    # ramona_url = "http://www.mathieuramona.com/uploads/Main/"
    # jamendo_db = ["jam_train_audio.tar.gz",
    #     "jam_valid_audio.tar.gz",
    #     "jam_test_audio.tar.gz"]
    # for dataset in jamendo_db:
    #     utils.print_warning("TODO")
    #     # urllib.urlretrieve("http://www.example.com/songs/mp3.mp3", "mp3.mp3")
    # # https://members.loria.fr/ALiutkus/kam/
    # # https://infinit.io/_/XnG7U95

    # utils.print_info("For MedleyDB, you must request access to:")
    # utils.print_info("http://medleydb.weebly.com/downloads.html")
    # utils.print_error("Stopping programm, cannot continue further.")

    # tracks_dir = "tracks/"
    # clean_filenames(tracks_dir)

    # svmbff.experiment_1()
    # dir_tracks = utils.create_dir("tracks")
    # yaafe_feat_extraction(dir_tracks)
    # ghosal.experiment_1()
    vqmm.main()

if __name__ == "__main__":
    main()
