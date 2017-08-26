import imghdr
import numpy as np
import os
import re

from loader_base import data_loader

#read label info from label_colors.txt
def read_label_color(location):    
    color_table = {}
    with open(location, "r") as f:
        for line in f:            
            info = re.split(" |\t*", line.strip())
            (r,g,b) = int(info[0]), int(info[1]), int(info[2])
            if (r,g,b) not in color_table:
                color_table[(r,g,b)] = info[3]
    
    return color_table

def read_data_location(raw_folder, label_folder):
    raw_names = []    
    label_names = []    
    valid_types = ['jpg', 'jpeg', 'png', 'bmp']            
    for root, dirs, files in os.walk(raw_folder):
        for i, f in enumerate(files):
            label_name = f[0:-4] + "_L.png"                
            raw_type = imghdr.what(raw_folder + f)                
            if raw_type in valid_types:
                print(i, "is valid type:", "open raw file:", raw_folder + f, "open label file:", label_folder + label_name)
                raw_names.append(raw_folder + f)
                label_names.append(label_folder + label_name)
                
    return raw_names, label_names

def create_camvid_loader(raw_folder, label_folder, transform = None, normalizer = None, cache = True):
    return data_loader(raw_folder, label_folder, read_data_location, transform, normalizer, cache)
