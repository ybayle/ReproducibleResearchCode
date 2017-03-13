# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   01/12/2016
# Updated   01/12/2016
# Version   1.0.0
#

"""
Description of bayle.py
======================

0 Input the local extracted features from YAAFE
    13 MFCC per frame
    186 musical pieces as train set
1 Computes delta and double delta (39 features per frame)
2 Gather global mean (39 features per musical pieces)
3 train on mfcc & deltas (39 feat/frame) to output global predictions
4 Use global preds to compute song and instru n-grams and histogramm
    which add 70 feat/track
    lead to a total of 109 feat/track
5 Fit on 109x186
6 predict (or predict_proba) on 41491 track 

:Example:

source activate py27
ipython
run bayle.py -d /media/sf_github/yann/train/

..todo::

"""

import multiprocessing
import webbrowser
import utils
import numpy as np
from sklearn.svm import SVC
from sklearn import linear_model
import sys
from functools import partial
import time
from sklearn.metrics import precision_recall_curve, precision_score, recall_score, classification_report, f1_score
import time
import numpy as np
import matplotlib.pyplot as plt
import math
import re
import os
import sys
import csv
import time
import utils
import argparse
from datetime import date
from collections import Counter
from matplotlib.cm import ScalarMappable
from matplotlib.colors import Normalize
from matplotlib.colorbar import ColorbarBase
import matplotlib.pyplot as plt
import numpy as np
import joblib
from sklearn.ensemble import RandomForestClassifier
import librosa

import os
import sys
import json
import math
import utils
import random
import joblib
from pprint import pprint
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import precision_score, recall_score, f1_score
from sklearn.model_selection import train_test_split, StratifiedKFold
from sklearn.neural_network import MLPClassifier
from sklearn.gaussian_process import GaussianProcessClassifier
from sklearn.gaussian_process.kernels import RBF
from sklearn import datasets
from sklearn import svm
from sklearn.ensemble import RandomForestClassifier
from sklearn.cross_validation import KFold, cross_val_score
from statistics import mean, stdev
from sklearn.neighbors import KNeighborsClassifier
from sklearn.neighbors import KNeighborsClassifier
from sklearn.svm import SVC
from sklearn.tree import DecisionTreeClassifier
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier, ExtraTreesClassifier, GradientBoostingClassifier
from sklearn.naive_bayes import GaussianNB
from sklearn.discriminant_analysis import QuadraticDiscriminantAnalysis, LinearDiscriminantAnalysis
from sklearn.linear_model import LogisticRegression
from sklearn.metrics import precision_recall_curve, precision_score, recall_score, classification_report, f1_score, accuracy_score
from sklearn import linear_model
from sklearn.tree import DecisionTreeClassifier

import classify
# import reproduce

def arr2str(data, separator=","):
    return separator.join(str(x) for x in data)

def str2arr(data):
    return np.array(data).astype(np.float)

def read_gts(filename, separator="\t"):
    track_gts = {}
    with open(filename, "r") as filep:
        for line in filep:
            line = line.split(separator)
            track_gts[line[0]] = line[1][:-1]
    return track_gts

def match_feat_with_song_gt(dir_feat, dir_gts):
    """Description of match_feat_gt

    Use groundtruth created by 
    http://www.mathieuramona.com/wp/data/jamendo/ 

    associate to local features
    csv 7041 lines yaafe
    lab 326.973 sec ramona
    Definition of YAAFE from 
    http://yaafe.sourceforge.net/features.html
    """
    utils.print_success("Matching local feat to song/instru groundtruths")
    dir_feat = utils.abs_path_dir(dir_feat)
    dir_gts = utils.abs_path_dir(dir_gts)
    block_size = 1024.
    step_size = 512.
    fech = 22050.
    frame_size_ms = block_size / fech
    filenames = [fn for fn in os.listdir(dir_gts)]
    for index, filename in enumerate(filenames):
        utils.print_progress_start(str(index) + "/" + str(len(filenames)) + " " + filename)
        # gather groundtruths
        groundtruths = []
        with open(dir_gts + filename, "r") as filep:
            for row in filep:
                line = row.split(" ")
                end = float(line[1])
                if "no" in line[2]:
                    tag = ",i\n"
                else:
                    tag = ",s\n"
                groundtruths.append([end, tag])
        gt_len = len(groundtruths)
        overflow = False
        gt_index = 0
        cpt = 0
        # Write features & groundtruths to file
        str_to_write = ""
        feat_fn = filename.split(".")[0]
        feat_fn += ".wav.mfcc.csv"
        with open(dir_feat + feat_fn, "r") as filep:
            for index, line in enumerate(filep):
                # todo cleanup
                if gt_index < gt_len:
                    if frame_size_ms * index > groundtruths[gt_index][0]:
                        gt_index += 1
                    if gt_index < gt_len:
                        str_to_write += line[:-1] + groundtruths[gt_index][1]
        with open(dir_feat + feat_fn, "w") as filep:
            filep.write(str_to_write)
    utils.print_progress_end()

def match_feat_with_instru_gt(indir, outdir):
    """Description of match_feat_gt

    Apply instru groundtruth to CCmixter and MedleyDB
    """
    utils.print_success("Matching local features to instrumental groundtruths")
    indir = utils.abs_path_dir(indir) + "/"
    outdir = utils.abs_path_dir(outdir) + "/"
    filenames = [fn for fn in os.listdir(indir)]
    for filename in filenames:
        outfile = open(outdir + filename, "w")
        with open(indir + filename, "r") as filep:
            for line in filep:
                outfile.write(line[:-1] + " i\n")
        outfile.close()

def process_local_feat(indir, file_gts_track, outdir_local, out_feat_global, train):
    """Description of process_local_feat
    Add delta and double delta to MFCCs
    """
    
    utils.print_success("Processing local features")
    
    # Preprocess arg
    indir = utils.abs_path_dir(indir)
    file_gts_track = utils.abs_path_file(file_gts_track)
    filelist = os.listdir(indir)
    outdir_local = utils.abs_path_dir(outdir_local)

    track_gts = {}
    with open(file_gts_track, "r") as filep:
        for line in filep:
            line = line.split(",")
            if train:
                index = line[0]
            else:
                index = line[0] + ".wav.mfcc.csv"
            track_gts[index] = line[1][:-1]

    for index, filename in enumerate(filelist):
        utils.print_progress_start(str(index) + "/" + str(len(filelist)) + " " + filename)
        if filename in track_gts:
            mfccs = []
            groundtruths = []
            with open(indir + filename, "r") as filep:
                next(filep)
                next(filep)
                next(filep)
                next(filep)
                next(filep)
                for line in filep:
                    line = line.split(",")
                    mfccs.append(str2arr(line[:-1]))
                    if train:
                        groundtruths.append(line[-1][:-1])
            mfccs = np.array(mfccs)
            delta_mfcc = librosa.feature.delta(mfccs)
            delta2_mfcc = librosa.feature.delta(mfccs, order=2)
            # Write local features in outdir_local
            with open(outdir_local + filename, "w") as filep:
                gt_to_write = ""
                if "i" in track_gts[filename]:
                    gt_to_write = ",i"
                elif "s" in track_gts[filename]:
                    # postpone frame groundtruth annotationa to another function later in the code
                    gt_to_write = ""
                else:
                    utils.print_warning("bayle.py line 231 local frame groundtruth undefined")
                if train:
                    for a, b, c, d in zip(mfccs, delta_mfcc, delta2_mfcc, groundtruths):
                        filep.write(arr2str(a) + "," + arr2str(b) + "," + arr2str(c) + "," + d + "\n")
                else:
                    for a, b, c in zip(mfccs, delta_mfcc, delta2_mfcc):
                        filep.write(arr2str(a) + "," + arr2str(b) + "," + arr2str(c) + gt_to_write + "\n")
            # # Write global features in out_feat_global
            # with open(out_feat_global, "a") as filep:
            #     filep.write(filename + "," +
            #         arr2str(np.mean(mfccs, axis=0)) + "," + 
            #         arr2str(np.mean(delta_mfcc, axis=0)) + "," + 
            #         arr2str(np.mean(delta2_mfcc, axis=0)) + "," + 
            #         track_gts[filename] + "\n")
    utils.print_progress_end()
    utils.print_success("Adding local groundtruths to Songs in Jamendo thanks to Ramona annotations")
    match_feat_with_song_gt(dir_feat=outdir_local, dir_gts="groundtruths/frame_annot_jamendo_ramona/")
    utils.print_success("Done")

def column(matrix, i):
    return [row[i] for row in matrix]

def ngram_proba(local_pred, threshold=0.5, above_threshold=True):
    """
    n-gram creation
    """
    cpt_ngram = 0
    nb_ngram = 30
    ngrams = [0,] * nb_ngram
    for pred in local_pred:
        if above_threshold:
            condition = pred > threshold
        else:
            condition = pred <= threshold
        if condition:
            cpt_ngram += 1
        else:
            if cpt_ngram < nb_ngram:
                ngrams[cpt_ngram] += 1
            else:
                ngrams[nb_ngram-1] += 1
            cpt_ngram = 0
    nb_tag_sing = float(sum(ngrams))
    if nb_tag_sing > 0.:
        ngrams = [float(x) / nb_tag_sing for x in ngrams]
    # utils.print_error(ngrams)
    return ','.join(str(x) for x in ngrams)

def ngram(preds, tag):
    """Description of ngram
    """
    cpt_ngram = 0
    nb_ngram = 30
    ngrams = [0,] * nb_ngram
    for pred in preds:
        if tag in pred:
            cpt_ngram += 1
        else:
            if cpt_ngram < nb_ngram:
                ngrams[cpt_ngram] += 1
            else:
                ngrams[nb_ngram-1] += 1
            cpt_ngram = 0
    nb_tag = float(sum(ngrams))
    if nb_tag > 0.:
        ngrams = [float(x) / nb_tag for x in ngrams]
    return ','.join(str(x) for x in ngrams)

def create_track_feat_testset(folder, infile, outfile, model_file, train=False):
    """Description of create_track_feat_testset
    Need to read each test file
    compute deltas on mfcc in the ram
    predict and predict_proba 
    generate song and instru ngrams and histograms
    Add the mean of mfcc+deltas
    append 109 features vector in feat_track/feat_test.csv
    """

    utils.print_success("Create track feat testset")
    folder = utils.abs_path_dir(folder)
    infile = utils.abs_path_file(infile)
    clf = joblib.load(model_file)
    track_gts = read_gts(infile, separator=",")
    for index, filename in enumerate(track_gts):
        utils.print_progress_start(str(index+1) + "/" + str(len(track_gts)) + " " + filename)
        mfccs = []
        mfccs_1 = []
        extension = ""
        if train:
            extension = ""
        else:
            extension += "_audio_full_mono_22k"
        extension += ".wav.mfcc.csv"
        with open(folder + filename + extension, "r") as filep:
            if train:
                next(filep)
                next(filep)
                next(filep)
                next(filep)
                next(filep)
            for line in filep:
                if train:
                    line = line.split(",")
                else:
                    line = line.split(" ")
                mfccs_1.append(str2arr(line[:-1]))
                # if train:
                #     mfccs.append(str2arr(line[:-1]))
                # else:
                #     mfccs.append(str2arr(line[0:]))
        mfccs = np.array(mfccs_1)
        delta_mfcc = librosa.feature.delta(mfccs)
        delta2_mfcc = librosa.feature.delta(mfccs, order=2)
        tmp = np.append(mfccs, delta_mfcc, axis=1)
        features = np.append(tmp, delta2_mfcc, axis=1)
        preds_proba = clf.predict_proba(features)

        # Histogramm
        nb_hist_class = 10
        numbers = column(preds_proba, 0)
        hist_pred = np.histogram(numbers, nb_hist_class)
        hist_pred_norm = hist_pred[0] / float(sum(hist_pred[0]))

        ngram_threshold = 0.5
        song_ngram_proba = ngram_proba(local_pred=numbers, threshold=ngram_threshold, above_threshold=True)
        instru_ngram_proba = ngram_proba(local_pred=numbers, threshold=ngram_threshold, above_threshold=False)
        
        preds = clf.predict(features)
        song_ngram = ngram(preds, "s")
        instru_ngram = ngram(preds, "i")

        with open(outfile, "a") as filep:
            filep.write(filename[:12] + "," +
                arr2str(np.mean(mfccs, axis=0)) + "," + 
                arr2str(np.mean(delta_mfcc, axis=0)) + "," + 
                arr2str(np.mean(delta2_mfcc, axis=0)) + "," + 
                arr2str(hist_pred_norm) + "," +
                song_ngram_proba + "," + 
                instru_ngram_proba + "," +
                song_ngram + "," + 
                instru_ngram + "," +
                track_gts[filename] + "\n")
    utils.print_progress_end()

def figures1bd(indir, file_gts_track):
    """Description of figures1bd

    infile is formated like:
    /media/sf_github/yann/train/01 - 01 Les Jardins Japonais.wav.mfcc.csv
    feat1 feat2 ... featn tag1
    feat1 feat2 ... featn tag2
    ...
    feat1 feat2 ... featn tag2

    0 Input the local extracted features from YAAFE
        13 MFCC per frame
        186 musical pieces as train set
    1 Computes delta and double delta (39 features per frame)
    2 Gather global mean (39 features per musical pieces)
    3 train on mfcc & deltas (39 feat/frame) to output global predictions
    4 Use global preds to compute song and instru n-grams and histogramm
        which add 70 feat/track
        lead to a total of 109 feat/track
    5 Fit on 109x186
    6 predict (or predict_proba) on 41491 track 
    """

    # Preprocess arg
    indir = utils.abs_path_dir(indir)
    file_gts_track = utils.abs_path_file(file_gts_track)
    feat_frame_train = "feat_frame_train/"
    utils.create_dir(feat_frame_train)
    feat_frame_test = "feat_frame_test/"
    utils.create_dir(feat_frame_test)
    outdir_global = "feat_track/"
    utils.create_dir(outdir_global)
    feat_train = outdir_global + "train.csv"
    feat_test = outdir_global + "test.csv"
    models_dir = "models/"
    utils.create_dir(models_dir)
    loc_feat_testset_dirpath = "/media/sf_DATA/Datasets/Simbals/yaafe/results/processed/"
    filelist_test = "filelist_test.tsv"
    filelist_train = "filelist_train.tsv"
    models_global = "models_track/"
    utils.create_dir(models_global)

    # process_local_feat(indir, file_gts_track, feat_frame_train, feat_train, train=True)    
    # classify.create_models(outdir=models_dir, train_dir=feat_frame_train, separator=",")
    # create_track_feat_testset(indir, filelist_train, feat_train, train=True)

    # 15h28m44s to 19h08m28s Done in 13184117ms
    # create_track_feat_testset(loc_feat_testset_dirpath, filelist_test, feat_test)  

    # classify.create_models(outdir=models_global, train_file=feat_train)
    # classify.test_models_parallel(
        # models_dir=models_global,
        # out_dir="results/",
        # test_file=feat_test)
    
    # Display results
    reproduce.plot_results("results/")

def figure1a(file_gts_track):
    """Description of figure1a
    """
    outdir_global = "feat_track/"
    utils.create_dir(outdir_global)
    feat_train = outdir_global + "train.csv"

    # process_local_feat(indir, file_gts_track, feat_frame_train, feat_train, train=True)    
    classify.cross_validation(feat_train, n_folds=5)
    
def figure2(indir, file_gts_track):
    """Description of figure2

    Method to maintain 100 percent of precision and to maximize recall.
    """
    pass


def read_file_bayle(filename):
    """Description of read_file

    train/test example line:
    filename,feat1,feat2,...,featn,tag
    """
    filename = utils.abs_path_file(filename)
    filenames = []
    groundtruths = []
    features = []
    with open(filename, "r") as filep:
        for row in filep:
            line = row.split(",")
            filenames.append(line[0])
            features.append([float(i) for i in line[1:-1]])
            gt = line[-1]
            while "\n" in gt or "\r" in gt:
                gt = gt [:-1]
            groundtruths.append(gt)
    return filenames, features, groundtruths

def column(matrix, i):
    return [row[i] for row in matrix]

def process_results(train, test):
    train_fn, train_features, train_groundtruths = read_file_bayle(train)
    test_fn, test_features, test_groundtruths = read_file_bayle(test)
    step = 0.1
    # for weight in np.arange(0.0, 1.0, step):
    # inside_clf = RandomForestClassifier(random_state=2)
    inside_clf = DecisionTreeClassifier(random_state=2)
        # class_weight={"i":weight, "s":1-weight})
    clf = AdaBoostClassifier(
        random_state=2,#with 4 98%precision song class
        base_estimator=inside_clf)
    clf.fit(train_features, train_groundtruths)
    predictions = clf.predict(test_features)
    print("Accuracy " + str(accuracy_score(test_groundtruths, predictions)))
    print("F-Measure " + str(f1_score(test_groundtruths, predictions, average="weighted")))
    print("Precision " + str(precision_score(test_groundtruths, predictions, average=None)))
    print("Recall " + str(recall_score(test_groundtruths, predictions, average=None)))
    print("F-Measure " + str(f1_score(test_groundtruths, predictions, average=None)))

    # predictions = [1.0 if i=="s" else 0.0 for i in predictions]
    predictions = column(clf.predict_proba(test_features), 0)
    outdir = "predictions/"
    with open(outdir + "Bayle.csv", "w") as filep:
        for name, pred in zip(test_fn, predictions):
            filep.write(name + "," + str(1.0 - float(pred)) + "\n")

def new_algo_final(indir, file_gts_track):
    utils.print_success("Approx. time ~6 hours.")
    # Preprocess arg
    indir = utils.abs_path_dir(indir)
    file_gts_track = utils.abs_path_file(file_gts_track)
    dir_tmp = utils.create_dir(utils.create_dir("src/tmp") + "bayle")
    feat_frame_train = utils.create_dir(dir_tmp + "feat_frame_train")
    feat_frame_test = utils.create_dir(dir_tmp + "feat_frame_test")
    outdir_global = utils.create_dir(dir_tmp + "feat_track")
    feat_train = outdir_global + "train.csv"
    feat_test = outdir_global + "test.csv"
    models_dir = utils.create_dir(dir_tmp + "models")
    loc_feat_testset_dirpath = "features/database2/"
    filelist_train = "groundtruths/database1.csv"
    filelist_test = "groundtruths/database2.csv"
    models_global = utils.create_dir(dir_tmp + "models_track")

    process_local_feat(indir, file_gts_track, outdir_local=feat_frame_train, out_feat_global=feat_train, train=False)
    classify.create_models(outdir=models_dir, train_dir=feat_frame_train, separator=",", classifiers="RandomForest")

    """
    Create features at track scale for the train set
    Features: MFCC + Delta + Double Delta + ngrams + hist
    """
    model_file = "src/tmp/bayle/models/RandomForest/RandomForest.pkl"
    model_file = "/media/sf_DATA/ReproducibleResearchIEEE2017/src/tmp/bayle/models/RandomForest/RandomForest.pkl"
    create_track_feat_testset(indir, filelist_train, feat_train, model_file, train=True)

    # # 15h28m44s to 19h08m28s Done in 13184117ms
    create_track_feat_testset(loc_feat_testset_dirpath, filelist_test, feat_test, model_file)  

    classify.create_models(outdir=models_global, train_file=feat_train, classifiers="RandomForest")
    process_results(feat_train, feat_test)

def main():
    begin = int(round(time.time() * 1000))
    PARSER = argparse.ArgumentParser(description="Bayle et al. (2017) algorithm")
    PARSER.add_argument(
        "-d",
        "--indir",
        help="input dir containing all local features extracted by YAAFE",
        type=str,
        default="/media/sf_github/yann/train/",
        metavar="indir")
    PARSER.add_argument(
        "-i",
        "--gts",
        help="input file containing all track groundtruths",
        type=str,
        default="filelist_train.tsv")

    indir = "features/database1/"
    file_gts_track = "groundtruths/database1.csv"
    new_algo_final(indir, file_gts_track)
    # figure1a(PARSER.parse_args().gts)
    # figures1bd(PARSER.parse_args().indir, PARSER.parse_args().gts)
    # figure2(PARSER.parse_args().indir, PARSER.parse_args().gts)
    
    # Local feat processing

    # Global feat processing
    # bayle_fig3()

    utils.print_success("Done in " + str(int(round(time.time() * 1000)) - begin) + "ms")

if __name__ == "__main__":
    main()
