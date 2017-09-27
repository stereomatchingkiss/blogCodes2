from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf

def _conv_block(inputs, filters, kernel_size = 3, is_training = True):
    output = tf.layers.conv2d(
        inputs = inputs,
        filters = filters,
        kernel_size = [kernel_size, kernel_size],
        kernel_initializer = tf.contrib.layers.xavier_initializer(),
        padding='same')
        
    output = tf.contrib.layers.batch_norm(output, is_training = is_training)    
    return tf.nn.relu(output)    

def _fire_module(inputs, fire_id, s1, e1, e3, is_training = True):
    """
    Basic module that makes up the SqueezeNet architecture. It has two layers.
     1. Squeeze layer (1x1 convolutions)
     2. Expand layer (1x1 and 3x3 convolutions)
    :param input: Tensorflow tensor
    :param fire_id: Variable scope name
    :param channel: Depth of the previous output
    :param s1: Number of filters for squeeze 1x1 layer
    :param e1: Number of filters for expand 1x1 layer
    :param e3: Number of filters for expand 3x3 layer
    :return: Tensorflow tensor
    """
    
    with tf.name_scope(fire_id):        
        squeeze = _conv_block(inputs, s1, 1, is_training)
        expand1 = _conv_block(squeeze, e1, 1, is_training)
        expand3 = _conv_block(squeeze, e3, 3, is_training)
        
        return tf.concat([expand1, expand3], 3)        

 
def create_model(features, class_num = 8, is_training = True):
    """
    Create regression cnn mentioned by the paper "Deep Image Homography Estimation"
    
    Args:
        features : input features, should be 2 stack of gray image or color image with size 128*128
        is_training : True will set batch_norm on training mode and vice versa
    """
    
    net = tf.layers.conv2d(inputs = features, filters = 64, kernel_size = [3, 3], strides = (2, 2), padding = 'same')
    net = tf.contrib.layers.batch_norm(net, is_training = is_training)
    net = tf.nn.relu(net)
    net = tf.layers.max_pooling2d(net, pool_size = [3, 3], strides = 2)
    
    net = _fire_module(net, "fire2", 16, 64, 64, is_training)
    net = _fire_module(net, "fire3", 16, 64, 64, is_training)
    net = tf.layers.max_pooling2d(net, pool_size = [3, 3], strides = 2)
    
    net = _fire_module(net, "fire4", 32, 128, 128, is_training)
    net = _fire_module(net, "fire4", 32, 128, 128, is_training)
    net = tf.layers.max_pooling2d(net, pool_size = [3, 3], strides = 2)
    
    net = _fire_module(net, "fire4", 48, 192, 192, is_training)
    net = _fire_module(net, "fire4", 48, 192, 192, is_training)
    net = _fire_module(net, "fire4", 64, 256, 256, is_training)
    net = _fire_module(net, "fire4", 64, 256, 256, is_training)
            
    #net = tf.layers.dropout(net, training = is_training)
    #net = tf.layers.conv2d(inputs = net, filters = class_num, kernel_size = [1, 1])
    #net = tf.nn.relu(net)
    #net = tf.layers.average_pooling2d(net, pool_size = 7, strides = 7, padding = 'same')    
        
    net = tf.reshape(net, [-1, 512 * 7 * 7])
    net = tf.layers.dense(inputs = net, units = class_num, kernel_initializer = tf.contrib.layers.xavier_initializer(), name = 'regression_output')
        
    return net
