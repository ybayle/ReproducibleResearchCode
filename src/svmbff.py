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
Description of svmbff.py
======================

bextract -mfcc -zcrs -ctd -rlf -flx -ws 1024 -as 898 -sv -fe filename.mf -w out.arff

:Example:

python svmbff.py
"""

import os
import csv
import sys
import time
import utils
import shutil
import argparse
import multiprocessing
from statistics import stdev
from scipy.io import arff
from sklearn.metrics import precision_recall_curve, precision_score, recall_score, classification_report, f1_score, accuracy_score

begin = int(round(time.time() * 1000))

def validate_arff(filename):
    """Description of validate_arff

    Check if filename exists on path and is a file
    If file corresponds to valid arff file return absolute path
    Otherwise move file to invalid directory and return False
    """
    # Check if file exists
    if os.path.isfile(filename) and os.path.exists(filename):
        filename = os.path.abspath(filename)
    else:
        return False
    # If does not satisfy min size, move to "empty" folder
    if os.stat(filename).st_size < 8100:
        tmp_path = filename.split("/")
        empty_dirname = "/".join(tmp_path[:-1]) + "/empty/"
        if not os.path.exists(empty_dirname):
            os.makedirs(empty_dirname)
        shutil.move(filename, empty_dirname + tmp_path[-1])
        return False
    # # If filename does not match with feature name, move to "invalid" folder
    # name_file = filename.split("/")[-1][:12]
    # with open(filename) as filep:
    #     for i, line in enumerate(filep):
    #         if i == 70:
    #             # 71th line
    #             name_feat = line.split(" ")[2][1:13]
    #             break
    # if name_file != name_feat:
    #     tmp_path = filename.split("/")
    #     invalid_dirname = "/".join(tmp_path[:-1]) + "/invalid/"
    #     if not os.path.exists(invalid_dirname):
    #         os.makedirs(invalid_dirname)
    #     shutil.move(filename, invalid_dirname + tmp_path[-1])
    #     return False
    # If everything went well, return filename absolute path
    return filename

def merge_arff(indir, outfilename):
    """Description of merge_arff

    bextract program from Marsyas generate one output file per audio file
    This function merge them all in one unique file
    Check if analysed file are valid i.e. not empty
    """
    utils.print_success("Preprocessing ARFFs")
    indir = utils.abs_path_dir(indir)
    filenames = os.listdir(indir)
    outfn = open(outfilename, 'w')
    cpt_invalid_fn = 0
    # Write first lines of ARFF template file
    for filename in filenames:
        new_fn = validate_arff(indir + filename)
        if new_fn:
            with open(new_fn, 'r') as template:
                nb_line = 77
                for line in template:
                    if not nb_line:
                        break
                    nb_line -= 1
                    outfn.write(line)
                break
        else:
            cpt_invalid_fn += 1
    # Append all arff file to the output file
    cur_file_num = 1
    for filename in filenames:
        new_fn = validate_arff(indir + filename)
        if new_fn:
            cur_file_num = cur_file_num + 1
            utils.print_progress_start("Analysing file\t" + str(cur_file_num))
            fname = open(new_fn, 'r')
            outfn.write("".join(fname.readlines()[74:77]))
            fname.close()
        else:
            cpt_invalid_fn += 1
    utils.print_progress_end()
    outfn.close()
    # os.system("rm " + indir + "*.arff")
    if cpt_invalid_fn:
        utils.print_warning(str(cpt_invalid_fn) + " ARFF with errors found")
    return outfilename

def add_groundtruth(feature_fn, groundtruth_fn, output_fn):
    """Description of add_groundtruth

    Write in output filename the groundtruth merged with corresponding features

    ..todo:: Error with old_tag not corresponding to filename...


    """
    utils.print_success("Adding groundtruth")
    feature_fn = utils.abs_path_file(feature_fn)
    groundtruth_fn = utils.abs_path_file(groundtruth_fn)
    if os.path.isfile(output_fn) and os.path.exists(output_fn):
        utils.print_warning("Overwritting existing output file: " + 
            utils.abs_path_file(output_fn))
    # TODO Read groundtruth file in memory
    tmp_gt = csv.reader(open(groundtruth_fn, "r"))
    groundtruths = {}
    for row in tmp_gt:
        groundtruths[row[0]] = row[1]
    tags = []
    output = open(output_fn, "w")
    
    # switch if test set preprocessing
    # separator = "_"
    separator = "."

    with open(feature_fn, "r") as feat:
        line_num = 0
        tmp_line = ""
        for line in feat:
            line_num += 1
            if line_num > 74:
                if line[0] != "%":
                    # Alter feature line with correct tag
                    cur_line = line.split(",")
                    old_tag = cur_line[-1].split(separator)[0]
                    if old_tag in groundtruths:
                        new_tag = groundtruths[old_tag]
                        output.write(tmp_line + ",".join(cur_line[:-1]) + "," + new_tag +"\n")
                        tmp_line = ""
                        tags.append(new_tag)
                    else:
                        # TODO
                        # File not in groundtruth
                        tmp_line = ""
                        # utils.print_warning("Error with " + old_tag)
                else:
                    tmp_line += line
            elif line_num == 2:
                output.write("@relation train_test.arff\n")
                # output.write("@relation MARSYAS_KEA\n")
            elif line_num == 71:
                # Alter line 71 containing all tag gathered along the way
                # TODO enhance
                output.write("@attribute output {i,s}\n")
            else:
                # Write header
                output.write(line)
    output.close()

def split_number(number, nb_folds):
    """Description of split_number

    Return an int array of size nb_folds where the sum of cells = number
    All the integers in cells are the same +-1 
    """
    if not isinstance(number, int) and not isinstance(nb_folds, int):
        utils.print_error("Variable must be integer")
    if number < nb_folds:
        utils.print_error("Number of folds > Number of data available")
    min_num = int(number/nb_folds)
    folds = [min_num] * nb_folds
    for num in range(0, number-(min_num*nb_folds)):
        folds[num] = folds[num] + 1
    return folds

def create_folds(filelist, nb_folds, folds_dir, invert_train_test=False):
    """Description of create_folds

    """
    utils.print_success("Creating folds")
    if nb_folds < 1:
        utils.print_error("Wrong number of folds provided")

    # folds_dir = "/".join(filelist.split("/")[:-1])
    if nb_folds == 1:
        # Train and test set are the same
        folds_dir = folds_dir + "01_fold/"
        utils.create_dir(folds_dir)
        os.system("cp " + filelist + " " + folds_dir + "/train_test.arff")
    else:
        # Create train and test set
        folds_dir = folds_dir + str(nb_folds).zfill(2) + "_folds/"
        utils.create_dir(folds_dir)
        # TODO
        # Read filelist
        # Extract name and tag
        # Separate different tag
        # create folds
        data, meta = arff.loadarff(filelist)
        tags = {}
        for row in data:
            tag = row[-1].decode("ascii")
            if tag in tags:
                tags[tag] += 1
            else:
                tags[tag] = 1
        tags_folds = {}
        tags_folds_index = {}
        for tag in tags:
            tags_folds[tag] = split_number(tags[tag], nb_folds)
            tags_folds_index[tag] = 0
        # Create empty folds
        folds = {}
        # Init empty folds
        for index in range(0, nb_folds):
            folds[index] = ""
        # Fill folds with data
        with open(filelist, "r") as filelist_pointer:
            arff_header = ""
            tmp = ""
            for i, line in enumerate(filelist_pointer):
                utils.print_progress_start("\t" + str(i))
                # Until the 75th line
                if i > 74:
                    # Process ARFF data
                    if "% " in line:
                        # Memorize line
                        tmp += line
                    else:
                        # Get line 3 and add it to corresponding fold
                        tag = line.split(",")[-1][:-1]
                        num_fold = tags_folds_index[tag]
                        if tags_folds[tag][num_fold] == 0:
                            tags_folds_index[tag] += 1
                        tags_folds[tag][tags_folds_index[tag]] -= 1
                        folds[tags_folds_index[tag]] += tmp + line
                        tmp = ""
                else:
                    # Save ARFF header lines
                    arff_header += line
            utils.print_progress_end
        # At this point data has been split up in different part
        # Use this part to create train/test split
        if invert_train_test:
            # Test is bigger than train
            fn_with_min_data = "/train_"
            fn_with_max_data = "/test_"
        else:
            # Train is bigger than test
            fn_with_min_data = "/test_"
            fn_with_max_data = "/train_"
        for index_test in range(0, nb_folds):
            filep = open(folds_dir + fn_with_min_data + str(index_test+1).zfill(2) + ".arff", "a")
            filep.write(arff_header + folds[index_test])
            filep.close()
            filep = open(folds_dir + fn_with_max_data + str(index_test+1).zfill(2) + ".arff", "a")
            filep.write(arff_header)
            for index_train in range(0, nb_folds):
                if index_train != index_test:
                    filep.write(folds[index_train])
            filep.close()
    return folds_dir

def process_results(in_fn, out_fn):
    in_fn = utils.abs_path_file(in_fn)
    out_fp = open(out_fn, "w")
    with open(in_fn, "r") as filep:
        for index, line in enumerate(filep):
            if index % 2:
                row = line[:-1].split("\t")
                out_fp.write(row[0].split("_")[0] + "," + row[2] + "\n")
    out_fp.close()

def experiment_2_3():
    process_results("tmp/svmbff/SVMBFF.csv", "predictions/SVMBFF.csv")

def run_kea(train_file, test_file, out_file, verbose=False):
    """Description of run_kea

    Launch kea classification on specified file
    """
    kea_cmd = 'kea -m tags -w ' + train_file + ' -tw ' + test_file + ' -pr ' + out_file
    if not verbose:
        kea_cmd += "> /dev/null 2>&1"
    os.system(kea_cmd)
    train_dir = train_file.split(os.sep)
    train_dir = os.sep.join(train_dir[:-1])
    os.system("rm " + train_dir + "/*affinities*")
    test_dir = test_file.split(os.sep)
    test_dir = os.sep.join(test_dir[:-1])
    os.system("rm " + test_dir + "/*affinities*")

def run_kea_on_folds(folds_dir):
    """Description of run_kea_on_folds

    Wrapper for kea on folds
    """
    folds_dir = utils.abs_path_dir(folds_dir)
    out_file = folds_dir + "/results.txt"
    if os.path.exists(folds_dir + "/train_test.arff"):
        train_file = folds_dir + "/train_test.arff"
        test_file = train_file
        run_kea(train_file, test_file, out_file)
    else:
        nb_folds = len([name for name in os.listdir(folds_dir) if os.path.isfile(os.path.join(folds_dir, name))])
        # Run on multiple train/test
        for index in range(1, int(nb_folds/2)+1):
            utils.print_progress_start("Train/Test on fold " + str(index))
            train_file = folds_dir + "/train_" + str(index).zfill(2) + ".arff"
            test_file = folds_dir + "/test_" + str(index).zfill(2) + ".arff"
            out_file = folds_dir + "/results_" + str(index).zfill(2) + ".arff"
            run_kea(train_file, test_file, out_file)
        utils.print_progress_end()
        utils.print_warning("TODO multiprocessing")
        # # Parallel computing on each TrainTestFolds
        # printTitle("Parallel train & test of folds")
        # partialRunTrainTestOnFold = partial(runTrainTestOnFold, args=args)
        # pool = multiprocessing.Pool()
        # pool.map(partialRunTrainTestOnFold, range(nb_folds)) #make our results with a map call
        # pool.close() #we are not adding any more processes
        # pool.join() #tell it to wait until all threads are done before going on

def extract_feat_train():
    dirs = ["/media/sf_SharedFolder/DataSets/Jamendo/Yann/song/",
            "/media/sf_SharedFolder/DataSets/ccmixter_corpus/instru/",
            "/media/sf_SharedFolder/DataSets/MedleyDB/MedleyDB/instru/vrai/"]
    outdir= "res/"
    for indir in dirs:
        extensions = ["wav", "mp3"]
        filenames = [fn for fn in os.listdir(indir)
                if any(fn.endswith(ext) for ext in extensions)]
        for index, filename in enumerate(filenames):
            dirName = indir.split("/")[-2] + ".mf"
            with open(dirName, "w") as filep:
                filep.write(indir + filename + "\n") 
            outfilename = outdir + filename[:-3].replace(" ", "_") + "arff"
            bextract_cmd = "bextract -mfcc -zcrs -ctd -rlf -flx -ws 1024 -as 898 -sv -fe " + dirName + " -w " + outfilename
            os.system(bextract_cmd)

def read_gts(filename):
    filename = utils.abs_path_file(filename)
    groundtruths = {}
    i = 0
    with open(filename, "r") as filep:
        for index, line in enumerate(filep):
            if index > 73:
                if i == 0:
                    i += 1
                    name = line.split("/")[-1][:-1]
                elif i == 1:
                    i += 1
                elif i == 2:
                    i = 0
                    groundtruths[name] = line.split(",")[-1][:-1]
    return groundtruths

def read_preds(filename):
    pres_filen = utils.abs_path_file(filename)
    predictions = {}
    i = 0
    with open(filename, "r") as filep:
        for index, line in enumerate(filep):
            if index % 2:
                line = line.split("\t")
                name = line[0].split("/")[-1]
                pred = float(line[-1])
                if pred > 0.5:
                    predictions[name] = "s"
                else:
                    predictions[name] = "i"
    return predictions

def figure2():
    # folds_dir = create_folds("results/dataset.arff", 5)
    # run_kea_on_folds(folds_dir)
    # read results arff file and print accuracy and f-measure
    gts_filen = "results/dataset.arff"
    gts = read_gts(gts_filen)
    folds_dir = "results/05_folds/"
    res_files = [name for name in os.listdir(folds_dir) if os.path.isfile(os.path.join(folds_dir, name)) and "results" in name]
    acc = []
    f1 = []
    for res in res_files:
        predictions = []
        groundtruths = []
        preds = read_preds(folds_dir + res)
        for name in preds:
            if name in gts:
                groundtruths.append(gts[name])
                predictions.append(preds[name])
        acc.append(accuracy_score(groundtruths, predictions))
        predictions = [1 if i=="s" else 0 for i in predictions]
        groundtruths = [1 if i=="s" else 0 for i in groundtruths]
        f1.append(f1_score(groundtruths, predictions, average='weighted'))
    # Print average ± standard deviation
    print("Accuracy " + str(sum(acc)/float(len(acc))) + " ± " + str(stdev(acc)))
    print("F-Measure " + str(sum(f1)/float(len(f1))) + " ± " + str(stdev(f1)))
    # with open("../stats/table1_accuracy.csv", "a") as filep:
    #     filep.write("SVMBFF")
    #     for val in acc:
    #         filep.write("," + str(val))
    #     filep.write("\n")
    # with open("../stats/table1_f1.csv", "a") as filep:
    #     filep.write("SVMBFF")
    #     for val in f1:
    #         filep.write("," + str(val))
    #     filep.write("\n")
    with open("../stats/table1_accuracy.csv", "a") as filep:
        for val in acc:
            filep.write("SVMBFF," + str(val) + "\n")
    with open("../stats/table1_f1.csv", "a") as filep:
        for val in f1:
            filep.write("SVMBFF," + str(val) + "\n")

def extract_features(tracks_dir="tracks/", feat_dir="features/"):
    utils.print_success("Extracting features")
    tracks_fn = os.listdir(tracks_dir)
    utils.create_dir(feat_dir)
    tmp_folder = "tmp/"
    utils.create_dir(tmp_folder)
    bextract = "bextract -mfcc -zcrs -ctd -rlf -flx -ws 1024 -as 898 -sv -fe "
    for index, filename in enumerate(tracks_fn):
        utils.print_progress_start(str(index) + "/" + str(len(tracks_fn)) + " " + filename)
        track_path = filename + ".mf"
        with open(track_path, "w") as filep:
            filep.write(tracks_dir + filename + "\n")
        new_fn = filename.split(".")[0] + ".arff"
        try:
            os.system(bextract + track_path + " -w " + new_fn + "> /dev/null 2>&1")
        except:
            utils.print_info("You have to make marsyas available systemwide, tips:")
            utils.print_info("http://marsyas.info/doc/manual/marsyas-user/Step_002dby_002dstep-building-instructions.html#Step_002dby_002dstep-building-instructions")
            utils.print_info("http://stackoverflow.com/a/21173918")
            utils.print_error("Program exit")
        os.rename("MARSYAS_EMPTY" + new_fn, feat_dir + new_fn)
        os.system("rm " + track_path)
    utils.print_progress_end()
    os.system("rm bextract_single.mf")

def table1_exp1(folds_dir):
    utils.print_success("Experiment 1 in Table 1")
    fn_gts = "groundtruths/database1.csv"
    gts = utils.read_groundtruths(fn_gts)
    res_files = [name for name in os.listdir(folds_dir) if os.path.isfile(os.path.join(folds_dir, name)) and "results" in name]
    acc = []
    f1 = []
    for res in res_files:
        predictions = []
        groundtruths = []
        preds = read_preds(folds_dir + res)
        for name in preds:
            name_gts = name.split(".")[0]
            if name_gts in gts:
                groundtruths.append(gts[name_gts])
                predictions.append(preds[name])
        acc.append(accuracy_score(groundtruths, predictions))
        predictions = [1 if i=="s" else 0 for i in predictions]
        groundtruths = [1 if i=="s" else 0 for i in groundtruths]
        f1.append(f1_score(groundtruths, predictions, average='binary'))
    # Print average ± standard deviation
    utils.print_info("Accuracy " + str(sum(acc)/float(len(acc))) + " ± " + str(stdev(acc)))
    utils.print_info("F-Measure " + str(sum(f1)/float(len(f1))) + " ± " + str(stdev(f1)))
    dir_res = utils.create_dir("results/")
    with open(dir_res + "table1_accuracy.csv", "a") as filep:
        for val in acc:
            filep.write("SVMBFF," + str(val) + "\n")
    with open(dir_res + "table1_f1.csv", "a") as filep:
        for val in f1:
            filep.write("SVMBFF," + str(val) + "\n")

def experiment_1():
    utils.print_success("SVMBFF Experiment 1 (approx. 2 minutes)")
    
    # Variables
    dir_tmp = "tmp/"
    utils.create_dir(dir_tmp)
    dir_svmbff = dir_tmp + "svmbff/"
    utils.create_dir(dir_svmbff)
    dir_tracks = "tracks/"
    dir_feat = "features/"
    fn_feats_db1 = "svmbff_database1.arff"
    feats_gts_db1 = dir_feat + "svmbff_database1.arff"
    groundtruths = "groundtruths/database1.csv"
    
    extract_features(dir_tracks)
    merge_arff(dir_feat, fn_feats_db1)
    add_groundtruth(fn_feats_db1, groundtruths, feats_gts_db1)
    os.remove(fn_feats_db1)
    dir_folds = create_folds(feats_gts_db1, 5, dir_svmbff)
    run_kea_on_folds(dir_folds)
    table1_exp1(dir_folds)

def main():
    utils.print_success("SVMBFF (approx. 2 minutes)")
    experiment_1()

if __name__ == "__main__":
    PARSER = argparse.ArgumentParser(description="Validate list of ISRCs")
    PARSER.add_argument(
        "-i",
        "--input_dir",
        help="input directory containing all ARFF file from Marsyas bextract",
        type=str,
        default="data",
        metavar="input_dir")
    PARSER.add_argument(
        "-o",
        "--output_file",
        help="output file",
        type=str,
        default="feat_with_groundtruth.txt",
        metavar="output_file")
    PARSER.add_argument(
        "-g",
        "--groundtruth_file",
        help="groundtruth file",
        type=str,
        default="groundtruth.txt",
        metavar="groundtruth_file")
    PARSER.add_argument(
        "-n",
        "--nb_folds",
        default=1,
        type=int,
        metavar="nb_folds",
        help="classification folds number, must be >= 1, default = 1")

    main()
    # figure2()
    # indir1 = "res/"
    # indir2 = "/media/sf_DATA/Datasets/Simbals/new/201611/arff/"
    # merge_arff(indir2, "test2.arff")

    # utils.print_success("Kea classification")
    # # Variable declaration
    # input_dir = PARSER.parse_args().input_dir
    # res_dir = "analysis"
    # utils.create_dir(res_dir)
    # if input_dir[-1] == "/":
    #     input_dir = input_dir[:-1]
    # proj_dir = res_dir + "/" + input_dir.split("/")[-1]
    # utils.create_dir(proj_dir)
    # feat_without_groundtruth = proj_dir + "/feat_without_groundtruth.arff"
    # feat_with_groundtruth = proj_dir + "/" + PARSER.parse_args().output_file
    # # Functions call
    # merge_arff(input_dir, feat_without_groundtruth)
    # add_groundtruth(feat_without_groundtruth,
    #     PARSER.parse_args().groundtruth_file,
    #     feat_with_groundtruth)
    # os.remove(feat_without_groundtruth)
    # folds_dir = create_folds(feat_with_groundtruth, PARSER.parse_args().nb_folds, invert_train_test=True)
    # folds_dir = create_folds("results/train_kea.arff", 5)
    # run_kea_on_folds(folds_dir)

# # 2 merge all arff files dans train/test file (generate train/test folds/set,
# #   reuse vqmm) à partir des fichiers sources d'un autre dossier, tout copier
# #   dans dossier de svmbff. no-overlap train/Test
# # 3 lancer kea sur toutes les train/test
# # 4 Afficher les résultats

#     utils.print_success("Finished in " + str(int(round(time.time() * 1000)) - begin) + "ms")

# """
# kea -m tags -w ' + train_file + ' -tw ' + test_file + ' -pr ' + out_file
# """
