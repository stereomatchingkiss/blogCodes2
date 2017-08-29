import glob
import multiprocessing
import numpy as np
import os
import PIL as pil
import re

import sys
sys.path.append('./../transformer')

import transformer.transform_policy as transform_policy

from functools import partial
from multiprocessing import Pool
from random import shuffle

def _convert_label_to_integer(target_folder, label_colors, label_to_int, src_name):
    print("label_to_int:", src_name)
    img = np.array(pil.Image.open(src_name))
    img = label_to_int(img)
    pil.Image.fromarray(img.astype('uint8')).save(target_folder + src_name.split("/")[-1])

def convert_label_to_integer(source_folder, target_folder, label_colors):
    """convert label to integer
    Args:
        source_folder(string): folder of labels want to convert to integer
        target_folder(string): save the labels after converted
        label_colors(list):  list which store the color, color will transforme to integer 
        by the order
    """
    label_to_int = transform_policy.label_to_integer(label_colors, 0)
    if not os.path.exists(target_folder):
        os.makedirs(target_folder)
        
    pool = Pool(multiprocessing.cpu_count())
    func = partial(_convert_label_to_integer, target_folder, label_colors, label_to_int)
    pool.map(func, list(glob.glob(source_folder + "*.png")))
    pool.close()
    pool.join()       
        
def convert_integer_to_label(img, label_colors):
    """Convert integer to label
        Args:
            img(ndarray, H x W): ndarray with image contents, H x W
            label_colors : list which store the color, integer will transforme to color by the order.
    """
        
    integer_to_label = { i:color for i, color in enumerate(label_colors) }
    label = np.zeros((img.shape[0], img.shape[1], 3)).astype('uint8')
    for row in range(img.shape[0]):
        for col in range(img.shape[1]):
            key = int(img[row, col])
            label[row, col] = integer_to_label[key]
    
    return label

def count_img_pix(img, color_count, label_colors):
    """Count pixel number in label_colors of the image
    
    Args:
        img(numpy ndarray): is a numpy array with shape == height,width,channels.
        color_count(dictionary): is a dictionary with key == (r,g,b), value = occur number of pixel
        label_colors(list) : A list with (r,g,b) values want to count
    
    Example:
    
    
    img = np.array(pil.Image.open("lena.png"))
    color_count = {}
    label_colors = utility.read_color_integer(camvid_folder + 'label_integer.txt')
    count_img_px(img, color_count, label_colors)
    """    
    
    for pix in label_colors:
        mask = img == pix     
        if pix in color_count:
            color_count[pix] += int(mask.sum())
        else:
            color_count[pix] = int(mask.sum())
        
    return color_count, img.shape[0] * img.shape[1]

#count pixel number of the images
def count_imgs_pix(img_folder, label_colors):
    """Count pixel number in label_colors of the images
    
    Args:
        img_folder(string) : self explained
        label_colors(list) : A list with (r,g,b) values want to count 
    
    Example:
    
    label_colors = utility.read_color_integer(camvid_folder + 'label_integer.txt')
    #color_count is a dictionary with key == (r,g,b), value = occur number of pixel
    #total is the total number of the pixels
    color_count, total = count_imgs_pix("camvid/LabeledApproved_full", label_colors)
    """
    color_count = {}
    imgs_name = list(glob.glob(img_folder + "*.png" ))
    #imgs_name = imgs_name[0:10]
    total = 0
    for i, name in enumerate(imgs_name):
        print(i, ":", name)
        img = pil.Image.open(name)
        img = np.array(img)
        color_count, pix_num = count_img_pix(img, color_count, label_colors)
        total = total + pix_num
        
    return color_count, total

def read_color_count_sorted(file_location):
    """Read the file save by the function "write_color_count_sorted"
    Args:
        file_location(string): self explained
        
    Return:
        A list, [r value, g value, b value, total pixels, percentage, name of object]
    """
    results = []
    with open(file_location, 'r') as f:
        for line in f:
            info = re.split(' +', line.strip())            
            info[0] = int(info[0])
            info[1] = int(info[1])
            info[2] = int(info[2])
            info[3] = int(info[3])
            info[4] = float(info[4])
            info[5] = info[5]
            results.append(info)
    
    return results

def read_color_integer(file_location):
    """Read the color value. The file should have following format
    
    r g b
    
    I use this file to map the r,g,b value to integer, fisrt r,g,b 
    value will treated as 0, second is 1 and so on
    
    Args:
        file_location(string): self explained
    
    Return:
        A list of (r,g,b)        
    """
    results = []
    with open(file_location) as f:
        for line in f:
            info = re.split(' +', line.strip())
            pix = (int(info[0]), int(info[1]), int(info[2]))
            results.append(pix)
            
    return results

def _relabel_func(colors, target_folder, img_location):
    """implementation details of relabel_color.
    """
    print(img_location)
    img = np.array(pil.Image.open(img_location))
    for row in range(img.shape[0]):
        for col in range(img.shape[1]):
            pix = (img[row, col, 0], img[row, col, 1], img[row, col, 2])
            if pix not in colors:
                img[row, col, :] = 0
                    
    img = pil.Image.fromarray(img)
    print("save as:", target_folder + img_location.split("/")[-1])
    img.save(target_folder + img_location.split("/")[-1])

def relabel_color(source_folder, target_folder, colors):
    """This function change the color do not exist in colors as void(0,0,0)
    Args:
        source_folder(string): self explained
        target_folder(string): relabel results will save at here
        colors(list): list store tuple with rgb value. Like [(12,12,12), (0,0,0)]
    """
    if not os.path.exists(target_folder):
        os.makedirs(target_folder)
        
    imgs_location = glob.glob(source_folder + "*.png")    
    print("img location size:", len(imgs_location))        
    
    pool = Pool(multiprocessing.cpu_count())
    func = partial(_relabel_func, colors, target_folder)
    pool.map(func, imgs_location)
    pool.close()
    pool.join()

def write_color_count_sorted(save_as, color_count, color_table, total):    
    """write color count will sort the color count and save it into a file with the format 
    "r g b occur_number occur_probability label_of_the_color". The resutls will
    be sorted by occur number of the pixel value.
    
    Args:
        save_as(string): self explained
        color_count(dictionary): A dictionary with key == (r,g,b), value = occur number of pixel
        color_table(dicitonary): A dictionary with key == (r,g,b), value = label(name of object like car, bus etc) of color        
        total(int,float): Total number of the pixels of the dataset
    
    Example:
    
    #read label color read the label color info from camvid dataset,it is a key with format
    #key : (r,g,b), value : occur number of pixel
    color_table = camvid_loader.read_label_color("camvid/label_colors.txt")
    color_count, total = count_imgs_pix("camvid/LabeledApproved_full")
    write_color_count_sorted("count_color.txt", color_count, color_table, total)
    """
    with open(save_as, "w") as f:
        cc = [[k[0], k[1], k[2], v] for k,v in color_count.items()]        
        cc = sorted(cc, key=lambda color: color[3], reverse = True)
        for k in cc:
            if (k[0], k[1], k[2]) in color_table:            
                #r, g, b, occur number, percentage, name
                f.write("{:3d} {:3d} {:3d} {:10d} {:5f} {}\n".format(k[0], k[1], k[2], k[3], k[3]/total, color_table[(k[0], k[1], k[2])]))

