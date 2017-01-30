#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   24/10/2016
# Updated   09/11/2016
# Version   1.0.0
#

import os
import re
import csv
import sys
import json
import time
import utils
import shutil
import fnmatch
import argparse
import subprocess
import webbrowser
import multiprocessing
from datetime import datetime
from functools import partial
from sklearn.model_selection import StratifiedKFold
from sklearn.metrics import precision_recall_curve, precision_score, recall_score, classification_report, f1_score, accuracy_score
from statistics import stdev
import numpy as np
import matplotlib.pyplot as plt

def valid_scientific_notation(val):
    pattern = re.compile("-?[0-9]\.[0-9]+[Ee][+-][0-9]{2}")
    if val:
        if pattern.match(val):
            return True
        else:
            return False
    else:
        return False

def convert_feats_files(filename):
    dir_out = filename.split(os.sep)[:-2]
    dir_out = utils.create_dir(os.sep.join(dir_out) + "/database1_vqmm")
    dir_out = dir_out[:-1]
    # Remove empty file or with too few lines to be analyzed
    if os.stat(filename).st_size > 4000:
        # Above line is slightly faster (20microsec) than
        # os.stat(filename).st_size
        outFileName = dir_out + filename.split(os.sep)[-1]
        outFile = open(outFileName, 'w')
        fileInvalid = False
        try:
            with open(filename, 'r') as data:
                reader = csv.reader(data)
                dismissedLines = 5
                requiredNumCol = 13
                for row in reader:
                    if 0 == dismissedLines:
                        if not len(row) < requiredNumCol:
                            str2write = ""
                            for col in row:
                                if valid_scientific_notation(col):
                                    str2write = str2write + "{0:.14f}".format(float(col)) + " "
                                else:
                                    utils.print_warning("Invalid scientific notation in " + filename)
                                    break
                            outFile.write(str2write[:-1] + "\n")
                        else:
                            utils.print_warning("Wrong number of col in " + filename)
                            break
                    else:
                        dismissedLines = dismissedLines - 1
        finally:
            outFile.close()
    else:
        utils.print_warning("File too small: only " + str(os.stat(filename).st_size) + " for " + filename)

def preprocess_features(folder):
    utils.print_success("Preprocessing train set")
    folder = utils.abs_path_dir(folder)
    filelist = os.listdir(folder)
    nb_file = str(len(filelist))
    for index, filename in enumerate(filelist):
        utils.print_progress_start(str(index) + "/" + nb_file + " " + filename)
        convert_feats_files(folder + filename)
    utils.print_progress_end()

def preprocess():
    outdir = "/media/sf_github/vqmm2/data/"
    errdir = "/media/sf_github/vqmm2/"
    indir = "/media/sf_github/yaafe/"
    included_extenstions = ["csv"]
    for database in ["ccmixter/", "Jamendo/", "MedleyDB/"]:
        folder = indir+database
        filenames = [fn for fn in os.listdir(folder)
            if any(fn.endswith(ext) for ext in included_extenstions)]
        for filename in filenames:
            validAndConvertFile(folder, outdir, errdir, filename)

def preprocess_testset():
    utils.print_success("Preprocessing test set")
    utils.print_warning("Preprocessing whole test set since the beginning")
    groundtruths = utils.get_test_gts()
    new_file = open("vqmm/test.txt", "w")
    with open("vqmm/old.txt", "r") as filep:
        for line in filep:
            if line[56:68] in groundtruths:
                new_file.write(line)
    new_file.close()

def train_on_test():
    """Description of train_on_test
    Train and test on same dataset, (to check if everything ok)
    """
    create_cbk()
    train()
    test("res186/", "models_file186.txt", "train186.txt")

def process_results():
    tmp_gts = utils.get_test_gts()
    predictions = []
    groundtruths = []
    res_dir = "res/"
    utils.create_dir(res_dir)
    res_file = open(res_dir + "VQMM.csv", "w")
    with open("vqmm/results_uneven/test.cbkcodebookFile.perItemCL.txt", "r") as filep:
        for line in filep:
            line = line.split("\t")
            isrc = line[0][4:16]
            if isrc in tmp_gts:
                res_file.write(isrc + "," + line[1] + "\n")
    res_file.close()

def read_item_tag(filename):
    filename = utils.abs_path_file(filename)
    data = {}
    with open(filename, "r") as filep:
        for line in filep:
            line = line.split("\t")
            data[line[0][2:]] = line[1]
    return data

def convert_features(inDIR, outDIR, errDIR, filename):
    """
    convertit les features a la volé plutôt que de tout stocker
    """
    inFileName = inDIR + filename
    errFileName = errDIR + filename
    # Remove empty file or with too few lines to be analyzed
    if os.stat(inFileName).st_size > 4000:
        # Above line is slightly faster (20microsec) than
        # os.stat(inFileName).st_size
        outFileName = outDIR + filename
        outFile = open(outFileName, 'w')
        fileInvalid = False
        try:
            with open(inFileName, 'r') as data:
                reader = csv.reader(data)
                dismissedLines = 5
                requiredNumCol = 13
                for row in reader:
                    if 0 == dismissedLines:
                        if not len(row) < requiredNumCol:
                            str2write = ""
                            for col in row:
                                if valid_scientific_notation(col):
                                    str2write = str2write + "{0:.14f}".format(float(col)) + " "
                                else:
                                    fileInvalid = True
                                    break
                            outFile.write(str2write[:-1] + "\n")
                        else:
                            fileInvalid = True
                            break
                    else:
                        dismissedLines = dismissedLines - 1
        finally:
            outFile.close()
            if fileInvalid:
                # os.remove(outFileName) # TODO uncomment
                if not os.path.exists(errDIR):
                    os.makedirs(errDIR)
                shutil.move(inFileName, errFileName)
    else:
        if not os.path.exists(errDIR):
            os.makedirs(errDIR)
        shutil.move(inFileName, errFileName)

def create_cbk(vqmm_cmd, files_list, file_cbk):
    randomSeed = "1"
    codebookSize = "100"
    subprocess.call([vqmm_cmd, '-quiet', 'n', '-list-of-files', files_list, 
        '-random', randomSeed, '-codebook-size', codebookSize, '-codebook', file_cbk])

def train(vqmm_cmd, codebook_file, models_dir, list_of_files):
    epsilon = "0.00001"
    subprocess.call([vqmm_cmd, '-quiet', 'n', 
        '-output-dir', models_dir,
        '-list-of-files', list_of_files,
        '-epsilon', epsilon, 
        '-codebook', codebook_file, '-make-tag-models'])

def test(vqmm_cmd, codebook_file, outputdir="res/", models_file="models_file.txt", testfile="test.txt"):
    subprocess.call([vqmm_cmd, '-tagify', '-output-dir', outputdir, '-models', models_file, 
        '-codebook', codebook_file, '-list-of-files', testfile])

def experiment_1(vqmm_cmd, codebook_file):
    """Description of experiment_1
    5-folds Cross-validation on 186 tracks evenly distributed

    Load train set in mem while convert
    create_cbk
    train 
    """
    utils.print_success("Experiment 1 (approx. 10min)")

    # # create 5 folds for train/test
    dir_tmp = utils.create_dir(utils.create_dir("tmp") + "vqmm")
    dir_folds = utils.create_dir(dir_tmp + "folds")
    names = []
    groundtruths = []
    with open("tmp/vqmm/filelist.txt", "r") as filep:
        for line in filep:
            line = line[:-1].split("\t")
            names.append(line[0])
            groundtruths.append(line[1])
    names = np.array(names)
    groundtruths = np.array(groundtruths)
    skf = StratifiedKFold(n_splits=5)
    index = 0
    for train_index, test_index in skf.split(names, groundtruths):
        index += 1

        # create fold
        with open(dir_folds + "train.txt", "w") as filep:
            for name, gt in zip(names[train_index], groundtruths[train_index]):
                filep.write(name + "\t" + gt + "\n")
        with open(dir_folds + "test.txt", "w") as filep:
            for name, gt in zip(names[test_index], groundtruths[test_index]):
                filep.write(name + "\t" + gt + "\n")

        # train and test on each folds.
        dir_res = utils.create_dir(dir_tmp + "fold_" + str(index))
        dir_models = utils.create_dir(dir_tmp + "models")
        train(vqmm_cmd, codebook_file, dir_models, dir_folds + "train.txt")
        # Need to explicitly create models_file here for VQMM
        models_list = os.listdir(dir_models)
        models_file = dir_tmp + "models_file.txt"
        with open(models_file, "w") as filep:
            for model_path in models_list:
                if not "NOT" in model_path:
                    filep.write(dir_models + model_path + "\n")
        test(vqmm_cmd, codebook_file, outputdir=dir_res, models_file=models_file, testfile=dir_folds + "test.txt")
    
    acc = []
    f1 = []
    for index in range(1, 6):
        predsfile = dir_tmp + "fold_" + str(index) + "/test.cbkcodebook.perItemCL.txt"
        gtsfile = dir_tmp + "fold_" + str(index) + "/test.cbkcodebook.perItemGT.txt"
        preds = read_item_tag(predsfile)
        gts = read_item_tag(gtsfile)
        groundtruths = []
        predictions = []
        for name in preds:
            if name in gts:
                groundtruths.append(gts[name])
                predictions.append(preds[name])
        acc.append(accuracy_score(groundtruths, predictions))
        predictions = [1 if i=="s" else 0 for i in predictions]
        groundtruths = [1 if i=="s" else 0 for i in groundtruths]
        f1.append(f1_score(groundtruths, predictions, average='weighted'))
    # Print average ± standard deviation
    print(acc)
    print(f1)
    print("Accuracy " + str(sum(acc)/float(len(acc))) + " ± " + str(stdev(acc)))
    print("F-Measure " + str(sum(f1)/float(len(f1))) + " ± " + str(stdev(f1)))
    # with open("../stats/table1_accuracy.csv", "a") as filep:
    #     filep.write("VQMM")
    #     for val in acc:
    #         filep.write("," + str(val))
    #     filep.write("\n")
    # with open("../stats/table1_f1.csv", "a") as filep:
    #     filep.write("VQMM")
    #     for val in f1:
    #         filep.write("," + str(val))
    #     filep.write("\n")
    with open(dir_tmp + "table1_accuracy.csv", "a") as filep:
        for val in acc:
            filep.write("VQMM," + str(val) + "\n")
    with open(dir_tmp + "table1_f1.csv", "a") as filep:
        for val in f1:
            filep.write("VQMM," + str(val) + "\n")

def main():
    """
    1 
    2 Make cbk on train set
    3 Train 200
    4 Test 50k
    """
    utils.print_success("VQMM (approx. 35min)")

    # 1 
    # preprocess features
    # YAAFE produce files which contain unusable float format
    # Need to transfroms those into a valid format
    # preprocess_features("features/database1/")

    # 2
    # Read filenames & groundtruths
    groundtruths = {}
    with open("groundtruths/database1.csv", "r") as filep:
        for line in filep:
            row = line[:-1].split(",")
            groundtruths[row[0]] = row[1]

    # 3
    # VQMM needs a special file containing path & filename along ground truth.
    dir_feats = utils.abs_path_dir("features/database1/")
    files_list = os.listdir(dir_feats)
    dir_tmp = utils.create_dir(utils.create_dir("tmp") + "vqmm")
    filenames_gts = dir_tmp + "filelist.txt"
    with open(filenames_gts, "w") as filep:
        for filename in files_list:
            fn = filename.split(".")[0]
            filep.write(dir_feats + filename + "\t" + groundtruths[fn] + "\n")

    # 4
    # Need to compile VQMM and check that everything is ok
    utils.print_success("Compiling VQMM")
    vqmm_cmd = "src/vqmm/vqmm"
    os.system("make -C src/vqmm/src")

    # 5
    # Create codebook needed for VQMM
    file_cbk = dir_tmp + "codebook.txt"
    create_cbk(vqmm_cmd, filenames_gts, file_cbk)

    # 5 
    # launch expe1
    experiment_1(vqmm_cmd, file_cbk)

    # randomSeed = "1"
    # codebookSize = "100"    
#     [0.92105263157894735, 0.81578947368421051, 0.78947368421052633, 0.80555555555555558, 0.77777777777777779]
# [0.92099792099792099, 0.81566181566181561, 0.78888888888888897, 0.80540540540540539, 0.77708978328173373]
# Accuracy 0.821929824561 ± 0.057301735896627654
# F-Measure 0.821608762847 ± 0.05750793298432727


    # 6 TODO
    # preprocess_testset()
    # create_cbk()
    # train()
    # test()
    # process_results()

if __name__ == "__main__":
    main()
