# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   19/01/2017
# Updated   19/01/2017
# Version   1.0.0
#

"""
Description of reproducibility.py
=================================

Launch source code file in order to reproduce results of the article

:Example:

python reproducibility.py

Dependencies:
marsyas software
http://marsyas.info/doc/manual/marsyas-user/Step_002dby_002dstep-building-instructions.html#Step_002dby_002dstep-building-instructions


"""

import os
import isrc
import utils
import svmbff
import urllib

def clean_filenames(tracks_dir = "tracks/"):
    for old_fn in os.listdir(tracks_dir):
        new_fn = old_fn
        new_fn = new_fn.replace(" ", "_")
        new_fn = new_fn.replace(",", "_")
        new_fn = new_fn.replace("(", "_")
        new_fn = new_fn.replace(")", "_")
        os.rename(tracks_dir + old_fn, tracks_dir + new_fn)

def main():
    """Description of main
    """

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

    svmbff.main()

if __name__ == "__main__":
    main()
