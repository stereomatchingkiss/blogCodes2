import glob
import numpy as np
import os
import PIL as pil

from multiprocessing import Pool
from random import shuffle

def count_img_pix(img, color_count):
    """Count pixel number of the image. img is a numpy array with shape == height,width,channels.
    color_count is a dictionary with key == (r,g,b), value = occur number of pixel
    
    Example:
    
    img = np.array(pil.Image.open("lena.png"))
    color_count = {}
    
    count_img_px(img, color_count)
    """    
    for row in range(img.shape[0]):
        for col in range(img.shape[1]):
             (r, g, b) = int(img[row,col][0]), int(img[row,col][1]), int(img[row,col][2])
             if (r, g, b) in color_count:
                 color_count[(r, g, b)] += 1
             else:
                 color_count[(r, g, b)] = 1
    
    return color_count, img.shape[0] * img.shape[1]

#count pixel number of the images
def count_imgs_pix(img_folder):
    """Count pixel number of the images in the folder.
    
    Example:
    
    #color_count is a dictionary with key == (r,g,b), value = occur number of pixel
    #total is the total number of the pixels
    color_count, total = count_imgs_pix("camvid/LabeledApproved_full")
    """
    color_count = {}
    imgs_name = list(glob.glob(img_folder + "*.png" ))
    #imgs_name = imgs_name[0:10]
    total = 0
    for i, name in enumerate(imgs_name):
        print(i, ":", name)
        img = pil.Image.open(name)
        img = np.array(img)
        color_count, pix_num = count_img_pix(img, color_count)
        total = total + pix_num
        
    return color_count, total

def write_color_count_sorted(file_location, color_count, color_table, total):    
    """write color count with sorted data, format of output file is 
    "r g b occur_number occur_probability label_of_the_color". The resutls will
    be sorted by occur number of the pixel value.
    
    color_count is a dictionary with key == (r,g,b), value = occur number of pixel
    total is the total number of the pixels
    
    Example:
    
    #read label color read the label color info from camvid dataset,it is a key with format
    #key : (r,g,b), value : occur number of pixel
    color_table = camvid_loader.read_label_color("camvid/label_colors.txt")
    color_count, total = count_imgs_pix("camvid/LabeledApproved_full")
    write_color_count_sorted("count_color.txt", color_count, color_table, total)
    """
    with open(file_location, "w") as f:
        cc = [[k[0], k[1], k[2], v] for k,v in color_count.items()]        
        cc = sorted(cc, key=lambda color: color[3], reverse = True)
        for k in cc:
            if (k[0], k[1], k[2]) in color_table:                
                f.write("{:3d} {:3d} {:3d} {:10d} {:5f} {}\n".format(k[0], k[1], k[2], k[3], k[3]/total, color_table[(k[0], k[1], k[2])]))

def relabel_func(colors, target_folder, img_location):
    """implementation details of relabel_color
    """
    print(img_location)
    img = np.array(pil.Image.open(img_location))
    for row in range(img.shape[0]):
        for col in range(img.shape[1]):
            pix = [img[row, col, 0], img[row, col, 1], img[row, col, 2]]                
            if pix not in colors:
                img[row, col, :] = 0
                    
    img = pil.Image.fromarray(img)
    print("save as:", target_folder + img_location.split("/")[-1])
    img.save(target_folder + img_location.split("/")[-1])

def relabel_color(source_folder, target_folder, colors):
    """If the color do not exist in colors, make it as void(0,0,0)
    """
    if not os.path.exists(target_folder):
        os.makedirs(target_folder)
        
    imgs_location = glob.glob(source_folder + "*.png")    
    print("img location size:", len(imgs_location))        
    
    pool = Pool(4)
    func = partial(relabel_func, colors, target_folder)
    pool.map(func, imgs_location)
    pool.close()
    pool.join()        
    
def split_train_test(raw_pics_location, label_location, train_pics_location, train_label_location, train_size):
    raw_pics = list(glob.glob(raw_pics_location + "*.png"))
    label_pics = list(glob.glob(label_location + "*.png"))
    
    combine_list = []
    for i in range(len(raw_pics)):
        combine_list.append([raw_pics[i], label_pics[i]])
    
    combine_list = shuffle(combine_list)
    for i in range(train_size):
        os.rename(raw_pics[i], train_pics_location + raw_pics[i].split("/")[-1])
        os.rename(label_pics[i], train_label_location + label_pics[i].split("/")[-1])

 
