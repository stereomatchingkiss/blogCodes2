from __future__ import absolute_import
from __future__ import division
from __future__ import print_function

import matplotlib.pyplot as plt
import numpy as np
from scipy import misc
import tensorflow as tf

import paper_model
import visualize

def read_imgs_info(file_name):
    imgs_name = []
    points = []
    with open(file_name) as in_file:
        for line in in_file:
            strs = line.split('\t')
            imgs_name.append([strs[0], strs[1]])
            points.append([float(strs[i]) for i in range(2,10)])
            
    return imgs_name, points

def read_imgs(imgs_name, folder):
    imgs = []    
    for i in range(0, len(imgs_name)):
        print(i, "read img:", imgs_name[i])
        img = np.empty(shape=(128, 128, 2))
        img[:,:,0] = misc.imread(folder + '/' + imgs_name[i][0])
        img[:,:,1] = misc.imread(folder + '/' + imgs_name[i][1])        
        imgs.append(img)    
        
    return imgs

def show_imgs(img):
    fig = plt.figure()
    fig.add_subplot(1,2,1)
    plt.imshow(img[:,:,0])
    fig.add_subplot(1,2,2)
    plt.imshow(img[:,:,1])
    plt.show()

folder = "/home/ramsus/Qt/blogCodes2/deep_homography/data/ms_coco_train_gray/"

imgs_name, points = read_imgs_info(folder + 'info.txt')
imgs = read_imgs(imgs_name, folder)

features = tf.placeholder(tf.float32, [None,128,128,2], name = 'input')
model = paper_model.create_model(features)

x = tf.placeholder(tf.float32, shape=[None,8])
loss = tf.sqrt(tf.nn.l2_loss(model - x))
optimizer = tf.train.GradientDescentOptimizer(0.005)
trainer = optimizer.minimize(loss)

out_file = open("record.txt", "w")
with tf.Session() as sess:
    batch_size = 100
    plot = visualize.plot_2d(min_x = 0, max_x = epoch)    
    sess.run(tf.global_variables_initializer())

    for epoch in range(10):
        total_loss = 0
        for i in range(100):
            index = i*batch_size
            if index == 10000:
                index = 0
            rng = slice(index, index+batch_size)
            train_imgs = imgs[rng]
            real_points = points[rng]
        
            train_accuracy = loss.eval(feed_dict={features : train_imgs, x : real_points})
            print("epoch:" + str(epoch), i, "batch acc", train_accuracy)
            total_loss += train_accuracy
            
            trainer.run(feed_dict={features : train_imgs, x : real_points})

        print("total loss epoch:" + str(epoch), total_loss/100.0)
        out_file.write("total loss epoch:" + str(epoch) + ","  + str(total_loss/100.0) + "\n")
        plot.update(e + 1, total_loss / 100)
    
    plot.savefig("ten_epoch.png")
    #saver = tf.train.Saver()
    #saver.save(sess, 'reshape.ckpt')
    #tf.train.write_graph(sess.graph.as_graph_def(), "", 'graph.pb')
    
"""

#python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/freeze_graph.py --input_graph=graph.pb --input_checkpoint=reshape.ckpt --output_graph=frozen_graph.pb --output_node_names=regression_output/BiasAdd
#python3 ~/.keras2/lib/python3.5/site-packages/tensorflow/python/tools/optimize_for_inference.py --input frozen_graph.pb --output opt_graph.pb --frozen_graph True --input_names input --output_names regression_output/BiasAdd

#~/Qt/3rdLibs/tensorflow/bazel-bin/tensorflow/tools/graph_transforms/transform_graph --in_graph=opt_graph.pb --out_graph=fused_graph.pb --inputs=input --outputs=regression_output/BiasAdd --transforms="fold_constants sort_by_execution_order"
"""
