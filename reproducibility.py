# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   19/01/2017
# Updated   30/01/2017
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
from statistics import mean, stdev
from sklearn.metrics import precision_recall_curve, precision_score, recall_score, classification_report, f1_score, accuracy_score
sys.path.insert(0, './src/')
import isrc
import utils
import ghosal
import svmbff

utils.print_warning("You need a python 2 environment.")
utils.print_warning("You need at least 150Go of free space and to store tracks in a dir named: tracks/")

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

def read_item_tag(filename):
    """Description of read_file

    example line:
    filename,tag
    """

    filename = utils.abs_path_file(filename)
    groundtruths = {}
    with open(filename, "r") as filep:
        for row in filep:
            line = row.split(",")
            groundtruths[line[0]] = line[1][:-1]
    return groundtruths


def results_experiment_2(algo_name, predictions, groundtruths):
    instru_gts = []
    instru_pred = []
    song_gts = []
    song_pred = []
    song_tmp_gts = []
    song_tmp_pred = []
    cpt = 0
    nb_instru = groundtruths.count("i")
    for index, tag in enumerate(groundtruths):
        if "i" in groundtruths[index]:
            instru_gts.append("i")
            instru_pred.append(predictions[index])
        else:
            if cpt == nb_instru:
                song_gts.append(song_tmp_gts)
                song_pred.append(song_tmp_pred)
                song_tmp_gts = []
                song_tmp_pred = []
                cpt = 0
            else:
                song_tmp_gts.append("s")
                song_tmp_pred.append(predictions[index])
                cpt += 1

    acc = []
    f1 = []
    for index, row in enumerate(song_gts):
        groundtruths = instru_gts + song_gts[index]
        predictions = instru_pred + song_pred[index]
        acc.append(accuracy_score(groundtruths, predictions))
        f1.append(f1_score(groundtruths, predictions, average='weighted'))

    print("Accuracy " + str(sum(acc)/float(len(acc))) + " ± " + str(stdev(acc)))
    print("F-Measure " + str(sum(f1)/float(len(f1))) + " ± " + str(stdev(f1)))

def experiment_2():
    utils.print_success("Experiment 2")
    groundtruths_file = "groundtruths/database2.csv"
    dir_pred = "predictions/"
    predictions_files = os.listdir(dir_pred)
    gts = read_item_tag(groundtruths_file)
    for pred_file in predictions_files:
        algo_name = pred_file.split("/")[-1][:-4]
        utils.print_info(algo_name)
        test_groundtruths = []
        predictions = []
        with open(dir_pred + pred_file, "r") as filep:
            for line in filep:
                row = line[:-1].split(",")
                isrc = row[0]
                if isrc in gts:
                    test_groundtruths.append(gts[isrc]) 
                    predictions.append("s" if float(row[1])>0.5 else "i")
        results_experiment_2(algo_name, predictions, test_groundtruths)

    algo_name = "Random"
    utils.print_info(algo_name)
    test_groundtruths = ["s", ] * test_groundtruths.count("s") + ["i", ] * test_groundtruths.count("i")
    predictions = ["s", "i", ] * int(len(test_groundtruths)/2)
    if len(test_groundtruths) % 2:
        predictions += ["s"]
    results_experiment_2(algo_name, predictions, test_groundtruths)

def experiment_3():
    utils.print_success("Experiment 3")
    groundtruths_file = "groundtruths/database2.csv"
    dir_pred = "predictions/"
    predictions_files = os.listdir(dir_pred)
    gts = read_item_tag(groundtruths_file)
    for pred_file in predictions_files:
        algo_name = pred_file.split("/")[-1][:-4]
        utils.print_info(algo_name)

        test_groundtruths = []
        predictions = []
        with open(dir_pred + pred_file, "r") as filep:
            for line in filep:
                row = line[:-1].split(",")
                isrc = row[0]
                if isrc in gts:
                    test_groundtruths.append(gts[isrc]) 
                    predictions.append("s" if float(row[1])>0.5 else "i")
        
        print("Accuracy : " + str(accuracy_score(test_groundtruths, predictions)))
        print("F-score  : " + str(f1_score(test_groundtruths, predictions, average='weighted')))
        print("Precision: " + str(precision_score(test_groundtruths, predictions, average=None)))
        print("Recall   : " + str(recall_score(test_groundtruths, predictions, average=None)))

    utils.print_info("Random")
    test_groundtruths = ["s", ] * test_groundtruths.count("s") + ["i", ] * test_groundtruths.count("i")
    predictions = ["s", "i", ] * int(len(test_groundtruths)/2)
    if len(test_groundtruths) % 2:
        predictions += ["s"]
    print("Accuracy : " + str(accuracy_score(test_groundtruths, predictions)))
    print("F-score  : " + str(f1_score(test_groundtruths, predictions, average='weighted')))
    print("Precision: " + str(precision_score(test_groundtruths, predictions, average=None)))
    print("Recall   : " + str(recall_score(test_groundtruths, predictions, average=None)))

def main():
    """Description of main
    """

    utils.print_success("Reproducible research (approx. 30 minutes)")

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
    # vqmm.main()

    # svmbff_train = "features/svmbff_database1.arff"
    # svmbff_test = "features/svmbff_database2.arff"
    # dir_tmp = utils.create_dir(utils.create_dir("tmp") + "svmbff")
    # svmbff_out = dir_tmp + "SVMBFF.csv"
    # svmbff.run_kea(svmbff_train, svmbff_test, svmbff_out)
    # svmbff.experiment_2_3()
    # ghosal.experiments_2_3("tmp/ghosal/database1.csv")

    experiment_2()
    experiment_3()

if __name__ == "__main__":
    main()
