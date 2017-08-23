import re
import torch

from functools import partial
from torch.utils.data import Dataset, DataLoader
from torchvision import transforms, utils

#read label info from label_colors.txt
def read_label_color(location):    
    color_table = {}
    with open(location, "r") as f:
        for line in f:            
            info = re.split(" |\t", line)
            (r,g,b) = int(info[0]), int(info[1]), int(info[2])
            if (r,g,b) not in color_table:
                color_table[(r,g,b)] = info[3].replace("\n", "")
    
    return color_table            

#read color_count.txt, format of input file is
#r g b occur_number
def read_color_count(file_location):
    color_count = []
    with open(file_location, "r") as f:
        for line in f:
            line = line.replace("\n", "")
            info = line.split(" ")        
            color_count.append([int(i) for i in info])
        
    return sorted(color_count, key=lambda color: color[3], reverse = True)



