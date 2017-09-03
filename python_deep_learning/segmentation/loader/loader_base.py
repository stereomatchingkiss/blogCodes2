import imghdr
import numpy as np
import os
import PIL as pil
import re
import torch

import sys
sys.path.append('../')
import transformer.transform_policy as transform_policy

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
    """This class is a base class to load data of image segmentation and return raw image and label in a dictionary. 
    Args:
        raw_folder(string) : Location of the raw images
        label_folder(string) : Location of the labels
        location_loader(callback) : This class determine how to load the images and labels location, accept two parameters
        which are the location of raw images and location of labels
        trasnformer(class) : Composition of different transformation
        normalizer(class) : If it is not None, this class will normalize the image. If the cache is True too, 
        loader_base will normalize the image one time only. This class is not meant to composed with regular
        trnasformer of PyTorch
    
    Example:    
    transform_func = transforms.Compose([transform_policy.random_crop((320, 480)), transform_policy.flip_horizontal(), transform_policy.to_tensor()])    
    loader = loader_base("/home/computer_vision_dataset/segmentation/camvid/train_raws/", 
                         "/home/computer_vision_dataset/segmentation/camvid/train_labels/", 
                         transform_func, camvid_loader.loader)
    sample = loader[0]
    raw_img = sample['raw']
    label_img = sample['label']
    """
    
    def __init__(self, raw_folder, label_folder, location_loader, trasnformer = None, normalizer = None, cache = True):
        super(data_loader, self).__init__()

        self._mcache = cache        
        self._mtransform = trasnformer
        self._mraw_names, self._mlabel_names = location_loader(raw_folder, label_folder)
        self._mraw_imgs, self._mlabel_imgs = [], []
        self._mnormalizer = normalizer
        if cache:
            for i in range(len(self._mraw_names)):
                print(i, "is valid type:", "open raw file:", raw_folder + self._mraw_names[i], "open label file:", label_folder + self._mlabel_names[i])
                raw_img = pil.Image.open(self._mraw_names[i])
                label_img = pil.Image.open(self._mlabel_names[i])
                raw_img = np.array(raw_img).astype('float32')
                label_img = np.array(label_img).astype('int64')
                if normalizer:
                    raw_img = normalizer(raw_img)
                
                self._mraw_imgs.append(raw_img)
                self._mlabel_imgs.append(label_img)                    
        
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
            if self._mnormalizer:
                raw = self._mnormalizer(raw)
                
            result = {'raw' : raw, 'label' : label}
            
        if self._mtransform:            
            result = self._mtransform(result)
            
        return result
        
    def __len__(self):
        return len(self._mraw_names)                
