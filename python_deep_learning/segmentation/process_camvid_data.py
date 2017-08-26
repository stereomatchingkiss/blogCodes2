import camvid_loader
import glob
import os
import utility

from random import shuffle

def relabel():
    Sky = [128, 128, 128]
    Building = [128, 0, 0]
    Pole = [192, 192, 128]
    Road = [128, 64, 128]    
    Tree = [128, 128, 0]
    SignSymbol = [192, 128, 128]
    Fence = [64, 64, 128]
    Car = [64, 0, 128]
    Pedestrian = [64, 64, 0]
    Bicyclist = [0, 128, 192]
    Void = [0, 0, 0]
    colors = [Sky, Building, Pole, Road, Tree, SignSymbol, Fence, Car, Pedestrian, Bicyclist, Void]
    camvid_loader.relabel_color("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/LabeledApproved_full/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/link_net_labels/", colors)

def split_train_test(raw_pics_location, label_location, train_pics_location, train_label_location, train_size):
    raw_pics = list(glob.glob(raw_pics_location + "*.png"))
    label_pics = [label_location + name.split("/")[-1][0:-4] + "_L.png" for name in raw_pics]
    
    combine_list = []
    for i in range(len(raw_pics)):
        combine_list.append([raw_pics[i], label_pics[i]])
    
    shuffle(combine_list)    
    for i in range(train_size):
        name_list = combine_list[i]        
        os.rename(name_list[0], train_pics_location + name_list[0].split("/")[-1])
        os.rename(name_list[1], train_label_location + name_list[1].split("/")[-1]) 
    
#color_table = camvid_loader.read_label_color("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/label_colors.txt")
#color_count = camvid_loader.read_color_count("color_count.txt")
#color_count, total = camvid_loader.count_imgs_pix("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/link_net_labels/")
#camvid_loader.write_color_count_sorted("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/color_count_relabel_sorted.txt", color_count, color_table, total)

split_train_test("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid//701_StillsRaw_full/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/link_net_labels/",
                 "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid//train_raws/", "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_labels/", 367)


#color_count, total = utility.count_imgs_pix("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_labels/")
#utility.write_color_count_sorted("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/color_count_relabel_sorted.txt", color_count, color_table, total)
