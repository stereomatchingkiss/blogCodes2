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

class data_loader(Dataset):
    """This class load data of image segmentation and return raw image and label in a dictionary. 
    If cache is true, this class will save all of the images(including raws and labels) into
    the ram, else it would try to load the image whenever __get_item__ is called
    
    Example:
    loader = data_loader("/home/computer_vision_dataset/segmentation/camvid/train_raws/", "/home/computer_vision_dataset/segmentation/camvid/train_labels/", None, False)
    sample = loader[0]
    raw_img = sample['raw']
    label_img = sample['label']
    """
    
    def __init__(self, raw_folder, label_folder, transform = None, cache = True):
        super(data_loader, self).__init__()
                        
        self._mcache = cache
        self._mtransform = transform
        
        self._mraw_names, self._mraw_imgs, self._mlabel_names, self._mlabel_imgs = self._load_data(raw_folder, label_folder, cache)        
        print(len(self._mraw_names), len(self._mraw_imgs))
        print(len(self._mlabel_names), len(self._mlabel_imgs))                
        
    def __getitem__(self, idx):
        """This function return two data, raw image and label image        
        """

        result = {}
        if(self._mcache):
            result = {'raw' : self._mraw_imgs[idx], 'label' : self._mlabel_imgs[idx]}
        else:            
            print(self._mraw_names[idx], self._mlabel_names[idx])
            result = {'raw' : np.array(pil.Image.open(self._mraw_names[idx])), 'label' : np.array(pil.Image.open(self._mlabel_names[idx]))}
            
        if self._mtransform:
            result = self._mtransform(result)
            
        return result
        
    def __len__(self):
        return len(self._mraw_names)
            
    def _load_data(self, raw_folder, label_folder, cache):
        
        raw_names = []
        raw_imgs = []
        label_names = []
        label_imgs = []
        valid_types = ['jpg', 'jpeg', 'png', 'bmp']        
        for root, dirs, files in os.walk(raw_folder):
            for i, f in enumerate(files):
                label_name = f[0:-4] + "_L.png"                
                raw_type = imghdr.what(raw_folder + f)                
                if raw_type in valid_types:
                    print(i, "is valid type:", "open raw file:", raw_folder + f, "open label file:", label_folder + label_name)
                    raw_names.append(raw_folder + f)
                    label_names.append(label_folder + label_name)
                    if cache:
                        img = pil.Image.open(raw_folder + f)
                        raw_imgs.append(np.array(img))
                        img = pil.Image.open(label_folder + label_name)
                        label_imgs.append(np.array(img))
                                        
        return raw_names, raw_imgs, label_names, label_imgs



