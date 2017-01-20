# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   13/10/2016
# Updated   20/01/2017
# Version   1.0.0
#

"""
Description of ghosal.py
======================

:Example:

python ghosal.py
"""

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
# from sklearn.cross_validation import KFold, cross_val_score
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

def preprocess_yaafe_features(dir_features="features/database1/"):
    utils.print_success("Preprocessing YAAFE's features  (approx. 2 minutes)")
    groundtruths = utils.read_groundtruths("groundtruths/database1.csv")
    dir_features = utils.abs_path_dir(dir_features)
    filenames = os.listdir(dir_features)
    dir_tmp = utils.create_dir(utils.create_dir("tmp") + "ghosal")
    res_file = open(dir_tmp + "database1.csv", "w")
    res_file.write("filename,MFCC_01,MFCC_02,MFCC_03,MFCC_04,MFCC_05,MFCC_06,MFCC_07,MFCC_08,MFCC_09,MFCC_10,MFCC_11,MFCC_12,MFCC_13,tag\n")
    nb_header_lines = 4
    for index, filename in enumerate(filenames):
        utils.print_progress_start(str(index+1) + "/" + str(len(filenames)) + " " + filename)
        with open(dir_features + filename, "r+") as filep:
            tmp_mfcc = np.zeros(shape=(13,1))
            for line_index, line in enumerate(filep):
                # Skip 5 first header lines generated by YAAFE
                if line_index > nb_header_lines:
                    index = 0
                    mfccs = line.split(",")
                    for mfcc in mfccs:
                        tmp_mfcc[index] += float(mfcc)
                        index += 1
            tmp_mfcc /= (line_index - nb_header_lines)
            mfcc_str = ["%.15f" % number for number in tmp_mfcc]
            filen = filename.split(".")[0]
            if filen in groundtruths:
                res_file.write(filen + "," +  ",".join(mfcc_str) + "," + groundtruths[filen] + "\n")
    res_file.close()
    return res_file

def read_file(filename):
    filename = utils.abs_path_file(filename)
    with open(filename, "r") as filep:
        next(filep)
        first_line = True
        groundtruths = []
        filenames = []
        for row in filep:
            splitted_row = row.split(",")
            filenames.append(splitted_row[0])
            if first_line:
                first_line = False
                if "s" in splitted_row[-1][:-1]:
                    groundtruths = np.array([True])
                elif "i" in splitted_row[-1][:-1]:
                    groundtruths = np.array([False])
                features = np.array([splitted_row[1:-1]]).astype(np.float)
            else:
                cur_feat = np.array([splitted_row[1:-1]]).astype(np.float)
                if "s" in splitted_row[-1][:-1]:
                    groundtruths = np.append(groundtruths, np.array([True]))
                elif "i" in splitted_row[-1][:-1]:
                    groundtruths = np.append(groundtruths, np.array([False]))
                features = np.append(features, cur_feat, axis=0)
    return filenames, features, groundtruths

def classify(file_features):
    utils.print_success("Classifying")
    clf = linear_model.RANSACRegressor(random_state=3)
    filenames, features, groundtruths = read_file(file_features)
    acc = []
    f1 = []
    skf = StratifiedKFold(n_splits=5, random_state=3)
    for train, test in skf.split(features, groundtruths):
        clf.fit(features[train], groundtruths[train])
        preds_float = clf.predict(features[test])
        predictions = [i >= 0.5 for i in preds_float]
        acc.append(accuracy_score(groundtruths[test], predictions))
        f1.append(f1_score(groundtruths[test], predictions, average="weighted"))
    # Print average +- standard deviation
    print("Accuracy " + str(sum(acc)/float(len(acc))) + " ± " + str(stdev(acc)))
    print("F-Measure " + str(sum(f1)/float(len(f1))) + " ± " + str(stdev(f1)))
    dir_res = utils.create_dir("results/")
    with open(dir_res + "table1_accuracy.csv", "a") as filep:
        for val in acc:
            filep.write("Ghosal'," + str(val) + "\n")
    with open(dir_res + "table1_f1.csv", "a") as filep:
        for val in f1:
            filep.write("Ghosal'," + str(val) + "\n")

def main():
    utils.print_success("Ghosal' (approx. 15 minutes)")
    dir_features = "features/database1/"
    file_feat = preprocess_yaafe_features(dir_features)
    file_feat = "tmp/ghosal/database1.csv" # todo to delete
    classify(file_feat)

if __name__ == "__main__":
    main()