from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import matplotlib.pyplot as plt
import numpy as np
import random
import scipy.ndimage as ndimage
import tensorflow as tf

from scipy import misc
from timeit import default_timer as timer

import squeeze_net_model
import visualize

def read_imgs_info(file_name):
    """
    Read image locations and corresponding labels.Every training sample contain
    a pair of images and 8 int value, they are coordinate of top left, top right, 
    bottom left, bottom right (x1,y1)~(x4,y4). 
    
    Args:
        file_name : name of the input file
    """
    imgs_name = []
    points = []
    with open(file_name) as in_file:
        for line in in_file:
            strs = line.split('\t')
            imgs_name.append([strs[0], strs[1]])
            points.append([float(strs[i]) for i in range(2,10)])
            
    return imgs_name, points

def read_imgs(imgs_name, folder, shape, debug = False):
    imgs = []    
    for i in range(0, len(imgs_name)):
        if debug:
            print(i, "read img:", imgs_name[i])
        img = np.empty(shape=([shape[1],shape[2],shape[3]]))
        if shape[3] == 2:
            img[:,:,0] = misc.imread(folder + '/' + imgs_name[i][0])
            img[:,:,1] = misc.imread(folder + '/' + imgs_name[i][1])
        else:
            img[:,:,0:3] = misc.imread(folder + '/' + imgs_name[i][0])
            img[:,:,3:6] = misc.imread(folder + '/' + imgs_name[i][1])
        imgs.append(img)    
        
    return imgs

#imgs_folder = "/home/ramsus/Qt/blogCodes2/deep_homography/data/imagenet_train_gray_500032/" #total loss: 57.39
imgs_folder = "/home/ramsus/Qt/blogCodes2/deep_homography/data/ms_coco_train_gray_10000/" #total loss: 84.48
imgs_names, points = read_imgs_info(imgs_folder + '/info.txt')
print(imgs_names[0])
#for i in inference_graph.get_operations():
#    print(i.name)

with tf.Session() as sess:        
    saver = tf.train.import_meta_graph('/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_70000.ckpt.meta')
    inference_graph = tf.get_default_graph()
    #load as reshape_final.ckpt.data-00000-of-00001 will throw exception, I really do not know why 
    #tensorflow always like to give their users surprise
    saver.restore(sess, '/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_70000.ckpt')
    #saver.restore(sess, '/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/reshape_final.ckpt')
    batch_size = 100
    img_size = len(imgs_names)
    
    if img_size%batch_size != 0:
        raise Exception('img_size%batch_size != 0')
    
    iteration = int(img_size/batch_size)
    _accuracy = inference_graph.get_tensor_by_name('regression_output/BiasAdd:0')
    _features  = inference_graph.get_tensor_by_name('input:0')
    total_loss = 0
    
    for i in range(iteration):
        rng = slice(i*batch_size, i*batch_size+batch_size)
        real_poinst = points[rng]
        name_list = imgs_names[rng]
        results = sess.run(_accuracy, feed_dict = {_features: read_imgs(name_list, imgs_folder, [None, 128, 128, 2])})
        l2_loss = (np.sum((results-real_poinst) ** 2)/2)/batch_size
        total_loss += loss
        print(i, loss)
    print('total loss:', total_loss/iteration)
    #name_list = [imgs_names[1]]
    #results = sess.run(_accuracy, feed_dict = {_features: read_imgs(name_list, folder, [None, 128, 128, 2])})
    #print(results)
    #print(points[1])
    #a = np.asarray(results[0])
    #b = np.asarray(points[1])
    #print(np.sum(np.abs(a-b))/8)
    #for i in range(100):
    #    index = i*batch_size%img_size
    #    rng = slice(index, index+batch_size)
    #    train_imgs = read_imgs(imgs_name[rng], folder, input_shape)
    #    real_points = points[rng]
        
    

start = timer()
end = timer()
print("elapsed time:", end - start)
