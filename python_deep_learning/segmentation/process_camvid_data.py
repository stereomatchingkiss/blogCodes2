import glob
import numpy as np
import os
import PIL as pil
import shutil

import sys
sys.path.append('loader/')

import loader.camvid_loader as camvid_loader
import loader.utility as utility
import transformer.transform_policy as transform_policy

from random import shuffle

Void = (0, 0, 0)
Sky = (128, 128, 128)
Building = (128, 0, 0)
Pole = (192, 192, 128)
Road = (128, 64, 128)    
Tree = (128, 128, 0)
SignSymbol = (192, 128, 128)
Fence = (64, 64, 128)
Car = (64, 0, 128)
Pedestrian = (64, 64, 0)
Bicyclist = (0, 128, 192)
label_colors = [Void, Sky, Building, Pole, Road, Tree, SignSymbol, Fence, Car, Pedestrian, Bicyclist]

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
    for i in range(train_size):
        name_list = combine_list[i]
        if copy:
            print(i, "copy", name_list)
            raw = pil.Image.open(name_list[0])
            label = pil.Image.open(name_list[1])
            raw.save(train_pics_location + name_list[0].split("/")[-1])
            label.save(train_label_location + name_list[1].split("/")[-1])
        else:
            print(i, "move", name_list)
            os.rename(name_list[0], train_pics_location + name_list[0].split("/")[-1])
            os.rename(name_list[1], train_label_location + name_list[1].split("/")[-1])                
    
#split_train_test("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/701_StillsRaw_full/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/LabeledApproved_full/",
#                 "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_raws/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_labels/", 368, True)


#for name in list(glob.glob("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/701_StillsRaw_full/*.png")):    
#    if not os.path.exists("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_raws/" + name.split("/")[-1]):
#        print(name.split("/")[-1][0:-4] + "_L.png")
#        shutil.copy2(name, "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_raws/")
#        shutil.copy2("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/LabeledApproved_full/" + name.split("/")[-1][0:-4] + "_L.png",
#                     "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_labels/")

#utility.relabel_color("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_labels/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_relabels/", label_colors)
#utility.relabel_color("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_labels/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_relabels/", label_colors)
#color_count, total = utility.count_imgs_pix("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_relabels/")
#color_table = camvid_loader.read_label_color("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/label_colors.txt")
#utility.write_color_count_sorted("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/color_count_train_sorted.txt", color_count, color_table, total)

#utility.convert_label_to_integer("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_labels/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_int_labels/", label_colors)

#img = np.array(pil.Image.open("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_int_labels/0001TP_007440_L.png"))
#label = utility.convert_integer_to_label(img, label_colors)
#pil.Image.fromarray(label).show()


def test_loop():
    for i in range(10):
        print(i, "in loop")
        
    print(i, "out of loop")
    
    for j in range(5):
        print(i, "new loop")
        
test_loop()        
