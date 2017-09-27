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

import paper_model
import squeeze_net_model
import visualize

def augment_images(imgs):
    """
    image augmentation function, have tried with horizontal flip and blur, 
    both of them will degrade the performance
    
    Args:
        imgs : input images
    """
    new_images = []
    
    for img in imgs:        
        if random.random() > 0.5:
            new_img = np.empty(img.shape)
            #new_img[0] = ndimage.uniform_filter(img[0], sigma=11)
            #new_img[1] = ndimage.uniform_filter(img[1], sigma=11)
            new_img = img[:,::-1,:]
        else:
            new_img = img
            
        new_images.append(new_img)
    
    return new_images

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
        img = np.empty(shape=([shape[1],shape[2],shape[3]]), dtype='float32')
        if shape[3] == 2:
            img[:,:,0] = misc.imread(folder + '/' + imgs_name[i][0])
            img[:,:,1] = misc.imread(folder + '/' + imgs_name[i][1])            
        else:
            img[:,:,0:3] = misc.imread(folder + '/' + imgs_name[i][0])
            img[:,:,3:6] = misc.imread(folder + '/' + imgs_name[i][1])            
        imgs.append(img/255.0)    
        
    return imgs

folder = "/home/ramsus/Qt/blogCodes2/deep_homography/data/imagenet_train_gray_500032/"
#folder = "/home/ramsus/Qt/blogCodes2/deep_homography/data/ms_coco_train_gray_10000/"

input_shape = [None,128,128,2]
imgs_name, points = read_imgs_info(folder + 'info.txt')

features = tf.placeholder(tf.float32, input_shape, name = 'input')
model = squeeze_net_model.create_model(features)
print(model)

global_step = tf.Variable(0, trainable=False)
boundaries = [30000, 60000]
values = [0.005, 0.0005, 0.00005]
learning_rate = tf.train.piecewise_constant(global_step, boundaries, values)

x = tf.placeholder(tf.float32, shape=[None,8])
#apply sqrt to avoid the loss value bloated
loss = tf.sqrt((tf.nn.l2_loss(model - x)))

with tf.Session() as sess:
    sess.run(tf.global_variables_initializer())
    saver = tf.train.Saver()
    saver.save(sess, 'reshape_final.ckpt')
    tf.train.write_graph(sess.graph.as_graph_def(), "", 'graph_final.pb')

def train(batch_size, loss_record_step, total_steps):
    out_file = open("loss.txt", "w")
    optimizer = tf.train.GradientDescentOptimizer(learning_rate)
    trainer = optimizer.minimize(loss, global_step = global_step)
    with tf.Session() as sess:
        max_x = total_steps / loss_record_step
        plot = visualize.plot_2d(min_x = 0, max_x = max_x)
        sess.run(tf.global_variables_initializer())
        img_size = len(imgs_name)        
        total_loss = 0
    
        if img_size % batch_size != 0:
            raise Exception("iterate % batch_size != 0")    

        for i in range(total_steps):
            index = i*batch_size%img_size
            rng = slice(index, index+batch_size)
            train_imgs = read_imgs(imgs_name[rng], folder, input_shape)
            real_points = points[rng]
        
            train_accuracy = loss.eval(feed_dict={features : train_imgs, x : real_points})
            print(i, "batch acc", train_accuracy, learning_rate.eval())
            total_loss += train_accuracy
            if i % loss_record_step == 0 and i != 0:
                saver = tf.train.Saver()
                saver.save(sess, 'reshape_{}.ckpt'.format(i))
                tf.train.write_graph(sess.graph.as_graph_def(), "", 'graph_{}.pb'.format(i))
                
                print("total loss at {}/{} step : {}\n".format(i, total_steps, total_loss/loss_record_step))
                out_file.write("total loss per {} step:{}, learning rate = {}\n".format(loss_record_step, total_loss/loss_record_step, learning_rate.eval()))
                plot.update(i / loss_record_step, total_loss / loss_record_step)
                total_loss = 0
            
            trainer.run(feed_dict={features : train_imgs, x : real_points})
                    
        print("total loss at {}/{} step : {}\n".format(i, total_steps, total_loss/loss_record_step))
        out_file.write("total loss at {}/{} step : {}\n".format(i, total_steps, total_loss/loss_record_step))
        plot.update(max_x, total_loss / loss_record_step)
        gray_or_color = "gray"
        if input_shape[3] == 6:
            gray_or_color = "color"
    
        plot.savefig("{}_steps_{}_{}_images.png".format(total_steps, img_size, gray_or_color))
        saver = tf.train.Saver()
        saver.save(sess, 'reshape_final.ckpt')
        tf.train.write_graph(sess.graph.as_graph_def(), "", 'graph_final.pb')

start = timer()
#train(64, 10000, 90000)
#train(100, 100, 500)
end = timer()
print("elapsed time:", end - start)

"""
python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/freeze_graph.py --input_graph=graph.pb --input_checkpoint=reshape.ckpt --output_graph=frozen_graph.pb --output_node_names=avgpool10
python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/optimize_for_inference.py --input frozen_graph.pb --output opt_graph.pb --frozen_graph True --input_names input --output_names avgpool10
~/Qt/3rdLibs/tensorflow/bazel-bin/tensorflow/tools/graph_transforms/transform_graph --in_graph=opt_graph.pb --out_graph=fused_graph.pb --inputs=input --outputs=avgpool10 --transforms="fold_constants sort_by_execution_order"

#python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/freeze_graph.py --input_graph=sgd_gray_500032/graph.pb --input_checkpoint=sgd_gray_500032/reshape.ckpt --output_graph=sgd_gray_500032/frozen_graph.pb --output_node_names=regression_output/BiasAdd
#python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/optimize_for_inference.py --input sgd_gray_500032/frozen_graph.pb --output sgd_gray_500032/opt_graph.pb --frozen_graph True --input_names input --output_names regression_output/BiasAdd
#~/Qt/3rdLibs/tensorflow/bazel-bin/tensorflow/tools/graph_transforms/transform_graph --in_graph=sgd_gray_500032/opt_graph.pb --out_graph=sgd_gray_500032/fused_graph.pb --inputs=input --outputs=regression_output/BiasAdd --transforms="fold_constants sort_by_execution_order"
"""
