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
    deltas = []
    with open(file_name) as in_file:
        for line in in_file:
            strs = line.split('\t')
            imgs_name.append([strs[0], strs[1]])
            deltas.append([float(strs[i]) for i in range(2,10)])
            
    return imgs_name, deltas

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
imgs_names, deltas = read_imgs_info(imgs_folder + '/info.txt')

def test():
    with tf.Session() as sess:        
        saver = tf.train.import_meta_graph('/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_final.ckpt.meta')
        inference_graph = tf.get_default_graph()
        #load as reshape_final.ckpt.data-00000-of-00001 will throw exception, I really do not know why 
        #tensorflow always like to give their users surprise, is this fun?Or I am too stupid?
        saver.restore(sess, '/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_final.ckpt')
        batch_size = 100
        img_size = len(imgs_names)
    
        if img_size%batch_size != 0:
            raise Exception('img_size%batch_size != 0')
    
        iteration = int(img_size/batch_size)
        #Name of the tensor bug me some times to get it right, why do they add :0?
        #Where are the documents of using the trained model?
        _accuracy = inference_graph.get_tensor_by_name('regression_output/BiasAdd:0')
        _features  = inference_graph.get_tensor_by_name('input:0')
        total_l2_loss = 0
        total_sqrt_l2_loss = 0
        total_delta_diff = 0
    
        for i in range(iteration):
            rng = slice(i*batch_size, i*batch_size+batch_size)
            real_delta = deltas[rng]
            name_list = imgs_names[rng]
            results = sess.run(_accuracy, feed_dict = {_features: read_imgs(name_list, imgs_folder, [None, 128, 128, 2])})
            
            delta_diff = np.sum(np.abs(results - real_delta))
            total_delta_diff += delta_diff
            
            l2_loss = np.sum((results-real_delta) ** 2)/2
            sqrt_l2_loss = np.sqrt(l2_loss)        
            total_l2_loss += l2_loss
            total_sqrt_l2_loss += sqrt_l2_loss
            print("{},sqrt_l2_loss:{}, l2_loss:{}".format(i, sqrt_l2_loss, l2_loss))
            print("{},delta diff:{}".format(i, delta_diff))
        
        print('\ntotal l2 loss:{}, total sqrt l2 loss:{}'.format(total_l2_loss/iteration, total_sqrt_l2_loss/iteration))
        print('avg total l2 loss:{}, avg total sqrt l2 loss:{}'.format(total_l2_loss/iteration/batch_size, total_sqrt_l2_loss/iteration/batch_size))
        print("avg delta diff:{}".format(total_delta_diff/iteration/8.0/batch_size))
    
def test_one_img(index):
    with tf.Session() as sess:        
        saver = tf.train.import_meta_graph('/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_final.ckpt.meta')
        inference_graph = tf.get_default_graph()
        #load as reshape_final.ckpt.data-00000-of-00001 will throw exception, I really do not know why 
        #tensorflow always like to give their users surprise, is this fun?Or I am too stupid?
        saver.restore(sess, '/home/ramsus/Qt/blogCodes2/deep_homography/cnn/tensorflow/sgd_gray_squeeze_net_500032/reshape_final.ckpt')
        
        _accuracy = inference_graph.get_tensor_by_name('regression_output/BiasAdd:0')
        _features  = inference_graph.get_tensor_by_name('input:0')
        name_list = [imgs_names[index]]
        results = sess.run(_accuracy, feed_dict = {_features: read_imgs(name_list, imgs_folder, [None, 128, 128, 2])})
        estimate_delta = np.asarray(results[0])
        true_delta = np.asarray(deltas[index])
        
        delta_diff = np.abs(estimate_delta - true_delta)
        print('delta diff:', delta_diff)
        print('avg delta diff:', np.sum(delta_diff)/8.0)
        
        print('results:', results[0])
        print('true delta:', deltas[index])
                

start = timer()
test()
#test_one_img(0)
end = timer()
print("elapsed time:", end - start)
