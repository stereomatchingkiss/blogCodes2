from gluoncv import model_zoo
from gluoncv.utils import TrainingHistory

from mxnet import autograd, gluon, image, init, nd
from mxnet.gluon import nn
from mxnet.gluon import data as gdata, loss as gloss, utils as gutils
from mxnet import ndarray as nd
from mxnet.gluon.data.vision import transforms

from matplotlib import pyplot as plt

import argparse
import mobile_nets as mnet
import mxnet as mx
import MultiOutputImageDataset as moi
import numpy as np
import time

def parse_args():
    parser = argparse.ArgumentParser(description='An example of training multi output classifier by gluoncv')
    parser.add_argument('--train_root', type=str, help='Folder of train')
    parser.add_argument('--val_root', type=str, help='Folder of validate')
    parser.add_argument('--batch_size', type=int, default=16, help='Batch size')
    parser.add_argument('--epoch', type=int, default=3, help='Epoch')
    parser.add_argument('--input_size', type=int, default=96, help='Input width and height of image will resize to this value')    
    parser.add_argument('--lr', type=float, default=0.001, help="Learning rate")
    parser.add_argument('--num_workers', type=int, default=0, help="Number of workers")
    parser.add_argument('--save_as', type=str, default='fashion_net', help="Save model params as")
						
    return parser.parse_args()
	
color_dict = {"black" : 0, "blue" : 1, "red" : 2}
clothes_dict = {"jeans" : 0, "dress" : 1, "shirt" : 2}

context = mx.gpu()

net = mnet.fashion_net_2_branches(len(clothes_dict), len(color_dict), context)
net.hybridize()

train_metric_clothes = mx.metric.Accuracy()
train_metric_colors = mx.metric.Accuracy()
train_history = TrainingHistory(['train-clothes-error', 'train-color-error', 'test-clothes-error', 'test-color-error'])

def validate(ctx, val_loader):
    clothes_metric = mx.metric.Accuracy()
    color_metric = mx.metric.Accuracy()
    for i, batch in enumerate(val_loader):        
        label_clothes = batch[1].as_in_context(ctx)
        label_color = batch[2].as_in_context(ctx)
        outputs = net(batch[0].as_in_context(ctx))
        clothes_metric.update(label_clothes, outputs[0])
        color_metric.update(label_color, outputs[1])
    _, clothes_acc = clothes_metric.get()
    _, color_acc = color_metric.get()
	
    return clothes_acc, color_acc

def train(train_loader, val_loader, batch_size, lr, save_as):    
    optimizer = 'adam'
    # Set parameters
    optimizer_params = {'learning_rate': lr}

    # Define our trainer for net
    trainer = gluon.Trainer(net.collect_params(), optimizer, optimizer_params)
    criterion_clothes = gloss.SoftmaxCrossEntropyLoss()
    criterion_color = gloss.SoftmaxCrossEntropyLoss()
    for epoch in range(args.epoch):
        tic = time.time()
        train_metric_clothes.reset()
        train_metric_colors.reset()
        train_loss_clothes = 0		
        train_loss_color = 0

        # Loop through each batch of training data
        for i, batch in enumerate(train_loader):
            # AutoGrad
            #print("batch[0] shape:", batch[0].shape, "batch[1] shape:", batch[1].shape, "batch[2].shape", batch[2].shape)
            clothes_labels = batch[1].as_in_context(context)
            color_labels = batch[2].as_in_context(context)
            with autograd.record():
                outputs = net(batch[0].as_in_context(context))
                #print("output[0] shape:", outputs[0].shape, ", clothes labels shape:", clothes_labels.shape, ", color_labels:", color_labels.shape)
                #print("output[0] type:", type(outputs[0]), ", clothes type:", type(clothes_labels), ", color_labels type:", type(color_labels))
                loss_clothes = criterion_clothes(outputs[0], clothes_labels)
                loss_color = criterion_color(outputs[1], color_labels)                

            # Backpropagation
            autograd.backward([loss_clothes, loss_color])            

            # Optimize
            trainer.step(batch_size)

            # Update metrics
            train_loss_clothes += loss_clothes.sum().asscalar()
            train_loss_color += loss_color.sum().asscalar()
            #print("lost of clothes:", train_loss_clothes, ", lost of color:", train_loss_color)            
            train_metric_clothes.update(clothes_labels, outputs[0])
            train_metric_colors.update(color_labels, outputs[1])

        name, train_clothes_acc = train_metric_clothes.get()
        name, train_color_acc = train_metric_colors.get()
        # Evaluate on Validation data
        validate_clothes_acc, validate_color_acc = validate(context, val_loader)

        # Update history and print metrics
        train_history.update([1-train_clothes_acc, 1-train_color_acc, 1-validate_clothes_acc, 1-validate_color_acc])
        print('[Epoch %d] train_clothes_acc=%f train_color_acc=%f train_clothes_loss=%f, train_color_loss=%f, validate_clothes_acc=%f, validate_color_acc==%f, time: %f' %
             (epoch, train_clothes_acc, train_color_acc, train_loss_clothes, train_loss_color, validate_clothes_acc, validate_color_acc, time.time()-tic))

    # We can plot the metric scores with:
    train_history.plot()
    net.save_parameters(save_as)
	
if __name__ == '__main__':

    args = parse_args()

    batch_size = args.batch_size
	
    train_augs = gdata.vision.transforms.Compose([
            transforms.RandomResizedCrop((args.input_size, args.input_size)),
            transforms.RandomFlipLeftRight(), 
			transforms.RandomBrightness(0.1),
			transforms.RandomLighting(0.1),       
            transforms.ToTensor(),
            transforms.Normalize([0.4914, 0.4822, 0.4465], [0.2023, 0.1994, 0.2010])
    ])

    test_augs = gdata.vision.transforms.Compose([
            transforms.Resize(args.input_size),
            transforms.ToTensor(),
            transforms.Normalize([0.4914, 0.4822, 0.4465], [0.2023, 0.1994, 0.2010])
    ])
	
    train_data = moi.MultiOutputImageDataset(args.train_root, [color_dict, clothes_dict], train_augs)           
    train_loader = gdata.DataLoader(train_data, batch_size=batch_size, shuffle=True, num_workers=args.num_workers, last_batch='discard')
	
    val_data = moi.MultiOutputImageDataset(args.val_root, [color_dict, clothes_dict], test_augs)           
    val_loader = gdata.DataLoader(val_data, batch_size=batch_size, shuffle=True, num_workers=args.num_workers, last_batch='discard')
        
    train(train_loader, val_loader, batch_size, args.lr, args.save_as)