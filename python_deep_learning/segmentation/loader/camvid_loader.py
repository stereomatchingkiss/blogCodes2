import imghdr
import numpy as np
import os
import PIL as pil
import re
import torch

import sys
sys.path.append('../')
import transformer.transform_policy as transform_policy

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

class data_loader(Dataset):
    """This class load data of image segmentation and return raw image and label in a dictionary. 
    Args:
        raw_folder(string) : Location of the raw images
        label_folder(string) : Location of the labels        
        transform(class) : Composition of different transformation
    
    Example:
    label_colors = {[0, 0, 0] : 0, [128, 128, 128] : 1, [128, 256, 32] : 2}    
    transform_func = transforms.Compose([transform_policy.random_crop((320, 480)), transform_policy.flip_horizontal(), transform_policy.to_tensor()])    
    loader = data_loader("/home/computer_vision_dataset/segmentation/camvid/train_raws/", "/home/computer_vision_dataset/segmentation/camvid/train_labels/", 
                         transform_func, True)
    sample = loader[0]
    raw_img = sample['raw']
    label_img = sample['label']
    """
    
    def __init__(self, raw_folder, label_folder, transform = None, normalize = True, cache = True):
        super(data_loader, self).__init__()

        self._mcache = cache        
        self._mtransform = transform        
        
        self._mraw_names, self._mraw_imgs, self._mlabel_names, self._mlabel_imgs = self._load_data(raw_folder, label_folder)        
        print(len(self._mraw_names), len(self._mraw_imgs))
        print(len(self._mlabel_names), len(self._mlabel_imgs))                
        
    def __getitem__(self, idx):
        """This function return two data, raw image and label image        
        """

        result = {}
        idx = idx % len(self._mraw_names) 
        if self._mcache:
            result = {'raw' : self._mraw_imgs[idx], 'label' : self._mlabel_imgs[idx]}
        else:
            raw = np.array(pil.Image.open(self._mraw_names[idx])).astype('float32')
            label = np.array(pil.Image.open(self._mlabel_names[idx])).astype('int64')
            result = {'raw' : raw, 'label' : label}
            
        if self._mtransform:            
            result = self._mtransform(result)
            
        return result
        
    def __len__(self):
        return len(self._mraw_names)
            
    def _load_data(self, raw_folder, label_folder):
        
        raw_names = []
        raw_imgs = []
        label_names = []
        label_imgs = []
        valid_types = ['jpg', 'jpeg', 'png', 'bmp']        
        normalize = transform_policy.normalize(copy = False)
        for root, dirs, files in os.walk(raw_folder):
            for i, f in enumerate(files):
                label_name = f[0:-4] + "_L.png"                
                raw_type = imghdr.what(raw_folder + f)                
                if raw_type in valid_types:
                    print(i, "is valid type:", "open raw file:", raw_folder + f, "open label file:", label_folder + label_name)
                    raw_names.append(raw_folder + f)
                    label_names.append(label_folder + label_name)
                    
                    if(self._mcache):
                        img = pil.Image.open(raw_folder + f)
                        if(self._mnormalize):
                            img = normalize(img)                        
                        raw_imgs.append(np.array(img).astype('float32'))
                        img = pil.Image.open(label_folder + label_name)
                        label_img = np.array(img).astype('int64')                            
                        label_imgs.append(label_img)
                                        
        return raw_names, raw_imgs, label_names, label_imgs        

