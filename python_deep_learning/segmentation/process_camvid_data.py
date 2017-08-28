import glob
import numpy as np
import argparse
import os
import PIL as pil
import shutil

import sys
sys.path.append('loader/')

import loader.camvid_loader as camvid_loader
import loader.utility as utility
import transformer.transform_policy as transform_policy

from random import shuffle

"""This script preprocess the 
"""

parser = argparse.ArgumentParser(description = 'Hyper params')
parser.add_argument('--camvid_folder', type = str, default = "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/",
                    help = 'This folder should contains the files and folders clone from https://github.com/mostafaizz/camvid')

args = vars(parser.parse_args())

parser.add_argument('--raw_pics_folder', type = str, default = args['camvid_folder'] + '701_StillsRaw_full/',
                    help = 'Path to the raw images')
parser.add_argument('--label_pics_folder', type = str, default = args['camvid_folder'] + 'LabeledApproved_full/',
                    help = 'Path to the label images')
parser.add_argument('--train_size', type = int, default = 368, 
                    help = 'Size of training data after split')
parser.add_argument('--copy', type = bool, default = True,
                    help = 'True : Copy the files when split up training set adn test set. False : Move the files when split up training set and test set')

args = vars(parser.parse_args())

def split_train_test(raw_pics_location, label_location, 
                     train_pics_location, train_label_location,
                     test_pics_location, test_label_location,
                     train_size, copy = True):
    raw_pics = list(glob.glob(raw_pics_location + "*.png"))
    label_pics = [label_location + name.split("/")[-1][0:-4] + "_L.png" for name in raw_pics]
    
    combine_list = []
    for i in range(len(raw_pics)):
        combine_list.append([raw_pics[i], label_pics[i]])
    
    shuffle(combine_list)
    
    if not os.path.exists(train_pics_location):
        os.makedirs(train_pics_location)
    if not os.path.exists(train_label_location):
        os.makedirs(train_label_location)
    if not os.path.exists(test_pics_location):
        os.makedirs(test_pics_location)
    if not os.path.exists(test_label_location):
        os.makedirs(test_label_location)
    
    for i in range(len(raw_pics)):
        name_list = combine_list[i]
        if copy:
            print(i, "copy", name_list)
            if i < train_size:
                shutil.copy2(name_list[0], train_pics_location + name_list[0].split("/")[-1])
                shutil.copy2(name_list[1], train_label_location + name_list[1].split("/")[-1])
            else:
                shutil.copy2(name_list[0], test_pics_location + name_list[0].split("/")[-1])
                shutil.copy2(name_list[1], test_label_location + name_list[1].split("/")[-1])
        else:
            print(i, "move", name_list)
            if i < train_size:
                os.rename(name_list[0], train_pics_location + name_list[0].split("/")[-1])
                os.rename(name_list[1], train_label_location + name_list[1].split("/")[-1])
            else:
                os.rename(name_list[0], test_pics_location + name_list[0].split("/")[-1])
                os.rename(name_list[1], test_label_location + name_list[1].split("/")[-1])
        

camvid_folder = args['camvid_folder']    
split_train_test(camvid_folder + '701_StillsRaw_full/', camvid_folder + 'LabeledApproved_full/',
                 camvid_folder + 'train_raws/', camvid_folder + 'train_labels/',
                 camvid_folder + 'test_raws/', camvid_folder + 'test_labels/', 
                 args['train_size'], args['copy'])

#read_color_integer read the mapping of color->integer
label_colors = utility.read_color_integer(camvid_folder + 'label_integer.txt')
#relabel_color mark the pixel value do not exist in label_colors as (0,0,0)
utility.relabel_color(camvid_folder + 'train_labels/', camvid_folder + 'train_relabels/', label_colors)
utility.relabel_color(camvid_folder + 'test_labels/', camvid_folder + 'test_relabels/', label_colors)
#count_imgs_pix return a dictionary with the rgb value and number of pixel, an integer with the total pixels of the images
color_count, total = utility.count_imgs_pix(camvid_folder + 'train_relabels/', label_colors)
#read_label_color read the info store in the label_colors.txt, which come with camvid dataset
color_table = camvid_loader.read_label_color(camvid_folder + '/label_colors.txt')
#write_color_count_sorted will sort the color count and save it into a file
utility.write_color_count_sorted(camvid_folder + 'color_count_train_sorted.txt', color_count, color_table, total)
#convert_label_to_integer convert rgb value to integer value. example : (0,0,0)->0, (128,128,128)->1 and so on
utility.convert_label_to_integer(camvid_folder + "train_labels/", camvid_folder + "train_int_labels/", label_colors)

#img = np.array(pil.Image.open(camvid_folder + "train_int_labels/0001TP_006690_L.png"))
#label = utility.convert_integer_to_label(img, label_colors)
#pil.Image.fromarray(label).show()
