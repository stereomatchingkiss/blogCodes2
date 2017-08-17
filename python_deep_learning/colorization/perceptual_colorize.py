import bcolz
import numpy as np

import sys
sys.path.insert(0, '../utils') #I put BcolzArrayIterator in utils

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
from tqdm import tqdm

#lumi array contain y channel of YCrCb, converted from the formula of opencv, 72*72*1
lumi_arr = bcolz.open('image_net_full_opencv_y_72.bc')
#contain rgb channels, 72*72*3
origin_arr = bcolz.open('/home/ramsus/Qt/computer_vision_dataset/super_res/image_net_full_72.bc')

mse_loss = torch.nn.MSELoss()
#This network in charge of perceptual loss
#5, 10, 17 are the convolution layer after max_pool layer
#before relu activation
vgg_loss = vgg_net([5, 10, 17])
#This is the transform net we want to train
transform_net = colorization_net()

vgg_loss.cuda()
transform_net.cuda()

def preprocess_input(input_data):
    input_data = input_data.transpose((0,3,1,2))
    input_data = torch.from_numpy(input_data)
    return Variable(input_data.cuda())    

#preprocess the input of perceptual net(pretrained vgg16)
def preprocess_perceptual_input(input_data):
    input_data = input_data / 255.0
    #mean = np.array([0.485, 0.456, 0.406])
    #std = np.array([0.229, 0.224, 0.225])
    input_data[:, 0, :, :] = (input_data[:, 0, :, :] - 0.485)/0.229
    input_data[:, 1, :, :] = (input_data[:, 1, :, :] - 0.456)/0.224
    input_data[:, 2, :, :] = (input_data[:, 2, :, :] - 0.406)/0.225
    return input_data

def train(epoch, niter, weights = [0.2, 0.8, 0.2], lr_rate = [0.01, 0.001]):
    #set network at training mode, has effect on dropout and batchnorm 
    transform_net.train()
    agg_loss = 0 #aggregate loss
    loss_len = len(weights)
    
    for e in range(epoch):
        optimizer = optim.Adam(transform_net.parameters(), lr_rate[e])
        batch_size = 16
        bc = BcolzArrayIterator(lumi_arr, origin_arr, batch_size = batch_size)
        for i in range(niter):
            #make gradient as zero
            optimizer.zero_grad()
            #lumi_bc contain 16 image with y channel, origin_bc contain 16 images with rgb channels
            lumi_bc, origin_bc = next(bc)
            #shape of lumi_bc and origin_bc are (16,72,72,3) but PyTorch expect (16,3,72,72)
            #therefore we need to do some preprocessing
            lumi_bc = preprocess_input(lumi_bc)
            origin_bc = preprocess_input(origin_bc)
        
            #output predicted rgb in [0, 255]
            predict_rgb = transform_net(lumi_bc)
            
            predict_features = vgg_loss(preprocess_perceptual_input(predict_rgb))
            origin_features = vgg_loss(preprocess_perceptual_input(origin_bc))
            
            #this is the preceptual loss
            loss = 0
            for j in range(loss_len):
                loss += mse_loss(predict_features[j], origin_features[j]) * weights[j]
        
            #print some info
            agg_loss += loss.data[0]
            print("epoch {}, {} iteration, agg loss={}, loss={}".format(e, i, agg_loss / (i+1), loss.data[0]))

            #backward, compute gradient and optimize the weights        
            loss.backward()
            optimizer.step()
        
        torch.save(transform_net.state_dict(), 'color_transform_net_epoch_{}_iter_{}'.format(e+1, niter))

#I try with learning rate 0.01, 0.001
train(1, 2000, [0.2, 0.8, 0.2], [0.001])
