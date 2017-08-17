
# coding: utf-8

# In[1]:

import bcolz
import numpy as np

import sys
sys.path.insert(0, '../utils')

import torch
import torch.autograd as autograd
import torch.nn as nn
import torch.optim as optim
import torchvision

from bcolz_array_iterator import BcolzArrayIterator
from color_converter import ycrcb_to_rgb, ycrcb_to_rgb_torch
from colorize_net_0 import colorization_net
from loss_net import vgg_net
from torch.autograd import Variable
from torchvision import datasets, models, transforms

lumi_arr = bcolz.open('image_net_full_opencv_y_72.bc')
origin_arr = bcolz.open('/home/ramsus/Qt/computer_vision_dataset/super_res/image_net_full_72.bc')

transform_net = colorization_net()
transform_net.load_state_dict(torch.load('color_transform_net_epoch_1_iter_2000'))

vgg16 = torchvision.models.vgg16()
print(vgg16)

img = lumi_arr[99]
print("origin shape", img.shape)
img = np.transpose(img, (2,0,1))
img = np.expand_dims(img, 0)
img = transform_net(Variable(torch.from_numpy(img)))
img = img.data.numpy()

print("predict shape", img.shape)

img = np.transpose(img, (0,2,3,1))
print("predict transpose shape", img.shape)
img = img.reshape(img.shape[1],img.shape[2],img.shape[3])
img = img.astype('uint8')

import PIL as pil

img = pil.Image.fromarray(img)
img.show()

img.save('predict_color.jpg')
