import math
import matplotlib.pyplot as plt
import numpy as np
import PIL as pil
import torch
import torch.autograd as autograd
import torch.nn as nn

import sys
sys.path.append('loader/')

import loader.camvid_loader as camvid_loader
import loader.utility as utility
import loss.loss as loss_2d
import lr_scheduling
import models.link_net as segment_models
import transformer.transform_policy as transform_policy
import visualize

from torch.autograd import Variable
from torchvision import transforms, utils

Void = (0, 0, 0)
Sky = (128, 128, 128)
Building = (128, 0, 0)
Pole = (192, 192, 128)
Road = (128, 64, 128)    
Tree = (128, 128, 0)
SignSymbol = (192, 128, 128)
Fence = (64, 64, 128)
Car = (64, 0, 128)
Pedestrian = (64, 64, 0)
Bicyclist = (0, 128, 192)
label_colors = [Void, Sky, Building, Pole, Road, Tree, SignSymbol, Fence, Car, Pedestrian, Bicyclist]

#transform_func = transforms.Compose([transform_policy.random_crop((320, 480)), transform_policy.flip_horizontal()])
transform_func = transforms.Compose([transform_policy.random_crop((320, 480)), transform_policy.flip_horizontal(), transform_policy.to_tensor()])
normalizer = transform_policy.normalize(copy = False)
loader = camvid_loader.create_camvid_loader("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_raws/", 
                                            "/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/train_int_labels/", 
                                            transform_func, normalizer, cache = False)

sample = loader[231]
raw_img, label_img = sample['raw'], sample['label']

model = segment_models.link_net(len(label_colors))
data_loader = torch.utils.data.DataLoader(loader, batch_size = 16, shuffle = True, num_workers = 4)

color_info = utility.read_color_count_sorted('/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/color_count_train_sorted.txt')
print(color_info)

color_info = { (data[0], data[1], data[2]): data[4] for data in color_info}
weights = []
for color in label_colors:
    w = color_info[color]
    w = 1 / math.log(1.02 + w, 2)
    weights.append(w)

print("len of label_colors:", len(label_colors))
weights = np.array(weights)
weights = torch.from_numpy(weights.astype('float32'))

print(weights)

def train(model, loader, weights, epoch, lr_rate, save_model_as):    
    model.cuda()
    model.train(True)
    best_loss = 9999999
    loss_func = loss_2d.cross_entropy_loss_2d(weights.cuda())
    optimizer = torch.optim.Adam(model.parameters(), lr = lr_rate)
    plot = visualize.plot_2d(min_x = 0, max_x = epoch)    
    for e in range(epoch):        
        aggr_loss = 0
        for i, samples in enumerate(loader):            
            raws, labels = Variable(samples['raw']), Variable(samples['label'])
            raws, labels = raws.cuda(), labels.cuda()
        
            optimizer.zero_grad()
        
            output = model(raws)        
            loss = loss_func(output, labels)
            aggr_loss += loss.data[0]
            
            print("epoch:", e, "batch:", i+1, "aggregate loss:", aggr_loss / (i + 1), "loss:", loss.data[0])
            
            loss.backward()
            optimizer.step()
            
            iter_num = len(loader)*e + i
            lr_scheduling.poly_lr_scheduler(optimizer, lr_rate, iter_num)
                
        plot.update(e + 1, aggr_loss / len(loader))
        if (e != 0) and (e % 200 == 0):
            if aggr_loss < best_loss:
                best_loss = aggr_loss
                torch.save(model.state_dict(), save_model_as + "_{}_epoch_best_model.tch".format(e))
                
            torch.save(model.state_dict(), save_model_as + "_{}_epoch.tch".format(e))
            plot.savefig(save_model_as + "_{}_epoch.png".format(e))
           
    if aggr_loss < best_loss:
        torch.save(model.state_dict(), save_model_as + "_{}_epoch_best_model.tch".format(epoch))
    plot.savefig(save_model_as + "_{}_epoch.png".format(epoch))    
    torch.save(model.state_dict(), save_model_as + "_{}_epoch.tch".format(epoch))
    
    return model


model = train(model, data_loader, weights, 800, 0.0005, "link_net_adam")

def test_data_loader(data_loader):    
    for i_batch, sample_batched in enumerate(data_loader):
        print(i_batch, sample_batched['raw'].size(),
            sample_batched['label'].size())
    
        raw_batched = sample_batched['raw'].numpy()
        label_batched = sample_batched['label'].numpy()
        print(raw_batched.shape, raw_batched[0].shape)
        raw_batched = raw_batched.transpose((0,2,3,1))        
        #pil.Image.fromarray(raw_batched[0]).show()
        #pil.Image.fromarray(label_batched[0]).show()
    
        #if i_batch == 0:
        #    break
