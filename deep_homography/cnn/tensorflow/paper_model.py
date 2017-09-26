from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

from tensorflow.python.framework import function

import numpy as np
import tensorflow as tf

def _conv_block(inputs, filters, kernel_size = 3, is_training = True):
    output = tf.layers.conv2d(
        inputs = inputs,
        filters = filters,
        kernel_size = [kernel_size, kernel_size],
        padding='same')
    output = tf.contrib.layers.batch_norm(output, is_training = is_training)
    return tf.nn.relu(output)

def _conv_block_pool(inputs, filters, kernel_size = 3, is_training = True):
    output = _conv_block(inputs, filters, kernel_size, is_training)
    output = _conv_block(output, filters, kernel_size, is_training)
    return tf.layers.max_pooling2d(output, pool_size = [2, 2], strides = 2)

 
def create_model(features, is_training = True):
    """
    Create regression cnn mentioned by the paper "Deep Image Homography Estimation"
    
    Args:
        features : input features, should be 2 stack of gray image or color image with size 128*128
        is_training : True will set batch_norm on training mode and vice versa
    """
        
    net = _conv_block_pool(features, 64, 3, is_training)    
    net = _conv_block_pool(net, 64, 3, is_training)
    net = _conv_block_pool(net, 128, 3, is_training)
    net = _conv_block_pool(net, 128, 3, is_training)
    net = tf.layers.dropout(net, training = is_training)
    
    net = tf.reshape(net, [-1, 128 * 8 * 8])
    net = tf.layers.dense(inputs = net, units = 1024, activation = tf.nn.relu)
    net = tf.layers.dropout(net, training = is_training)    
    net = tf.layers.dense(inputs = net, units = 8, name = 'regression_output')
    
    return net
