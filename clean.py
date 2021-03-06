# -*- coding: utf-8 -*-
#!/usr/bin/python
#
# Author    Yann Bayle
# E-mail    bayle.yann@live.fr
# License   MIT
# Created   19/01/2017
# Updated   08/03/2017
# Version   1.0.0
#

"""
Description of clean.py
=================================

Clean all files generated by reproduciblity.py

:Example:

python clean.py

"""

import os
import shutil

folders_list = ["results", "figures", "src/tmp", "src/__pycache__"]

for folder in folders_list:
    if os.path.exists(folder) and os.path.isdir(folder):
        shutil.rmtree(folder)

print("Cleaning successful")
