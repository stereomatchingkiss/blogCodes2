import glob
import numpy as np
import PIL as pil
import torch

import models.link_net as segment_models
import transformer.transform_policy as transform_policy

from torch.autograd import Variable
from PIL.Image import Image

def decode_segmap(gray_img, label_colors):    
    label_colours = np.array(label_colors)
    r = gray_img.copy()
    g = gray_img.copy()
    b = gray_img.copy()
    for l in range(len(label_colors)):
        mask = gray_img == l
        r[mask] = label_colours[l, 0]
        g[mask] = label_colours[l, 1]
        b[mask] = label_colours[l, 2]

    rgb = np.zeros((gray_img.shape[0], gray_img.shape[1], 3))
    rgb[:, :, 0] = r
    rgb[:, :, 1] = g
    rgb[:, :, 2] = b
    
    return rgb

def test_accuracy(model, raw_img_name, save = False, show = False):
    size = (480,360)
    img = pil.Image.open('/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_raws/' + raw_img_name + ".png")
    img = img.resize(size)
    real_map = pil.Image.open('/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_relabels/' + raw_img_name + '_L.png').resize(size)
    img = np.array(img).astype('float32')
    normalizer = transform_policy.normalize()
    img = normalizer(img)

    img = img.transpose(2,0,1)
    img = np.expand_dims(img, 0)
    img = torch.from_numpy(img)    

    img = Variable(img.cuda())
    output = model(img)

    print("output size:", output.size())
    #max function return values of max at axis 1 and their index
    #we need to get the index with maximum value for our purpose
    output = output.data.max(1)[1]
    print(output.size())
    output = output.cpu().numpy()
    print(output.shape)
    output = np.squeeze(output)
    print(output.shape)

    rgb = decode_segmap(output, label_colors)
    rgb = pil.Image.fromarray(rgb.astype('uint8'))
    if show:
        rgb.show()
        real_map.show()
    if save:
        real_map.save('real_map_of_' + raw_img_name + ".jpg")    
        rgb.save('predict_map_of_' + raw_img_name + '.jpg')
    
    accuracy = (np.array(rgb).astype('uint8') == np.array(real_map).astype('uint8')).mean()
    print("accuracy:", accuracy)
    
    return accuracy
    
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

model = segment_models.link_net(len(label_colors))
model.load_state_dict(torch.load("link_net_rmsprob_800_epoch.tch"))
model.cuda()
test_accuracy(model, "Seq05VD_f02280", True, True)

"""
total_acc = 0
test_imgs = list(glob.glob("/home/ramsus/Qt/computer_vision_dataset/segmentation/camvid/test_raws/*.png"))
for i in range(len(test_imgs)):    
    acc = test_accuracy(model, test_imgs[i].split("/")[-1][0:-4])
    print(i, 'acc of img:', acc)
    total_acc += acc
    
print("average acc:", total_acc / len(test_imgs))
"""
