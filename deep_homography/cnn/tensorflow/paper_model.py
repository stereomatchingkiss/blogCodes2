from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import numpy as np
import tensorflow as tf

def _conv_block(inputs, filters, kernel_size = 3):
    output = tf.layers.conv2d(
        inputs = inputs,
        filters = filters,
        kernel_size = [kernel_size, kernel_size],
        padding='same')
    output = tf.contrib.layers.batch_norm(output, is_training = True)
    return tf.nn.relu(output)

def _conv_block_pool(inputs, filters, kernel_size = 3):
    output = _conv_block(inputs, filters, kernel_size)
    output = _conv_block(output, filters, kernel_size)
    return tf.layers.max_pooling2d(output, pool_size = [2, 2], strides = 2)

 
def create_model(features):
    """
    Create regression cnn mentioned by the paper "Deep Image Homography Estimation"
    
    Args:
        features : input features, should be 2 stack of gray image or color image with size 128*128
        mode : color expect two stack of color images, gray expect 2 stack of gray image        
    """
    
    net = _conv_block_pool(features, 64)    
    net = _conv_block_pool(net, 64)
    net = _conv_block_pool(net, 128)
    net = _conv_block_pool(net, 128)
    net = tf.layers.dropout(net)
    
    net = tf.reshape(net, [-1, 128 * 8 * 8])
    net = tf.layers.dense(inputs = net, units = 1024, activation = tf.nn.relu)
    net = tf.layers.dropout(net)
    net = tf.layers.dense(inputs = net, units = 8, name = 'regression_output')
    
    return net
