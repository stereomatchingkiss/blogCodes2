from gluoncv import model_zoo
from gluoncv.utils import TrainingHistory, LRScheduler

from mxnet import autograd, gluon, image, init, nd
from mxnet.gluon import nn
from mxnet.gluon import data as gdata, loss as gloss, utils as gutils
from mxnet import ndarray as nd
from mxnet.gluon.data.vision import transforms

from matplotlib import pyplot as plt

import argparse
import logging
import mobile_nets as mnet
import mxnet as mx
import MultiOutputImageDataset as moi
import numpy as np
import time

def parse_args():
    parser = argparse.ArgumentParser(description='An example of training multi output classifier by gluoncv')
    parser.add_argument('--train_root', type=str, help='Folder of train')
    parser.add_argument('--val_root', type=str, help='Folder of validate')
    parser.add_argument('--batch_size', type=int, default=32, help='Batch size')
    parser.add_argument('--epoch', type=int, default=10, help='Epoch')
    parser.add_argument('--input_size', type=int, default=128, help='Input width and height of image will resize to this value')    
    parser.add_argument('--lr', type=float, default=0.001, help="Learning rate")
    parser.add_argument('--lr-decay', type=float, default=0.1,
                        help='Decay rate of learning rate. default is 0.1.')
    parser.add_argument('--lr-decay-epoch', type=str, default='5,10',
                        help='Epochs at which learning rate decays.')
    parser.add_argument('--num_workers', type=int, default=0, help="Number of workers")
    parser.add_argument('--save_as', type=str, default='fashion_net.params', help="Save model params as")
    parser.add_argument('--start_epoch', type=int, default=0, help='Start training from epoch')
    parser.add_argument('--resume', type=str, default='',
                        help='Resume from previously saved parameters if not None. '
                        'For example, you can resume from ./fashion_net.params')
						
    return parser.parse_args()
	
color_dict = {"black" : 0, "blue" : 1, "red" : 2}
clothes_dict = {"jeans" : 0, "dress" : 1, "shirt" : 2}

context = mx.gpu()

args = parse_args()

net = mnet.fashion_net_2_branches(len(clothes_dict), len(color_dict), context)
if args.resume.strip():
    net.load_parameters(args.resume.strip())

net.hybridize()

train_metric_clothes = mx.metric.Accuracy()
train_metric_colors = mx.metric.Accuracy()
train_history = TrainingHistory(['train-clothes-error', 'train-color-error', 'train-error-avg', 'test-clothes-error', 'test-color-error', 'test-error-avg'])

def validate(ctx, val_loader):
    clothes_metric = mx.metric.Accuracy()
    color_metric = mx.metric.Accuracy()
    for i, batch in enumerate(val_loader):        
        label_clothes = batch[2].as_in_context(ctx)
        label_color = batch[1].as_in_context(ctx)
        outputs = net(batch[0].as_in_context(ctx))
        clothes_metric.update(label_clothes, outputs[0])
        color_metric.update(label_color, outputs[1])
    _, clothes_acc = clothes_metric.get()
    _, color_acc = color_metric.get()
	
    return clothes_acc, color_acc

def train(train_loader, val_loader, batch_size, save_as, lr_scheduler):    
    optimizer = 'adam'
    # Set parameters
    optimizer_params = {'lr_scheduler': lr_scheduler}

    # Define our trainer for net
    trainer = gluon.Trainer(net.collect_params(), optimizer, optimizer_params)
    criterion_clothes = gloss.SoftmaxCrossEntropyLoss()
    criterion_color = gloss.SoftmaxCrossEntropyLoss()
    
    logging.basicConfig()
    logger = logging.getLogger()
    logger.setLevel(logging.INFO)
    fh = logging.FileHandler(args.save_as + "_train.log")
    logger.addHandler(fh)
    logger.info(args)
    logger.info('Start training from [Epoch {}]'.format(args.start_epoch))
    best_acc = -100
    
    for epoch in range(args.start_epoch, args.epoch):
        print('epoch:', epoch, ', learning rate:', trainer.learning_rate)
        tic = time.time()
        train_metric_clothes.reset()
        train_metric_colors.reset()
        train_loss_clothes = 0
        train_loss_color = 0
            
        # Loop through each batch of training data
        for i, batch in enumerate(train_loader):                        
            clothes_labels = batch[2].as_in_context(context)
            color_labels = batch[1].as_in_context(context)
            with autograd.record():
                outputs = net(batch[0].as_in_context(context))
                loss_clothes = criterion_clothes(outputs[0], clothes_labels)
                loss_color = criterion_color(outputs[1], color_labels)                
            
            lr_scheduler.update(i, epoch)
            # Backpropagation
            autograd.backward([loss_clothes, loss_color])            

            # Optimize
            trainer.step(batch_size)

            # Update metrics
            train_loss_clothes += loss_clothes.sum().asscalar()
            train_loss_color += loss_color.sum().asscalar()            
            train_metric_clothes.update(clothes_labels, outputs[0])
            train_metric_colors.update(color_labels, outputs[1])            

        name, train_clothes_acc = train_metric_clothes.get()
        name, train_color_acc = train_metric_colors.get()
        # Evaluate on Validation data
        validate_clothes_acc, validate_color_acc = validate(context, val_loader)
        if (validate_clothes_acc + validate_color_acc) > best_acc:
            best_acc = validate_clothes_acc + validate_color_acc
            net.save_parameters(save_as + "_best")

        # Update history and print metrics
        train_history.update([1-train_clothes_acc, 1-train_color_acc, (1-train_clothes_acc + 1-train_color_acc) / 2, 
		                      1-validate_clothes_acc, 1-validate_color_acc, (1-validate_clothes_acc + 1-validate_color_acc) / 2])
        logger.info('[Epoch {}] lr={:.2E} train_clothes_acc={:.3f} train_color_acc={:.3f} train_acc_avg={:.3f} train_clothes_loss={:.3f}, train_color_loss={:.3f}, '
		      'validate_clothes_acc={:.3f}, validate_color_acc=={:.3f}, validate_acc_avg={:.3f} time: {}'.format
             (epoch, trainer.learning_rate, train_clothes_acc, train_color_acc, (train_clothes_acc + train_color_acc)/2, 
			 train_loss_clothes, train_loss_color, 
			 validate_clothes_acc, validate_color_acc, (validate_clothes_acc+validate_color_acc)/2, time.time()-tic))

    # We can plot the metric scores with:
    train_history.plot()
    net.save_parameters(save_as + "_" + str(args.epoch) + "epoch")
	
if __name__ == '__main__':    
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
            
    lr_decay_epoch = [int(i) for i in args.lr_decay_epoch.split(',')]
    lr_scheduler = LRScheduler(mode='step',
                               baselr=args.lr,
                               niters=len(train_data) // args.batch_size,
                               nepochs=args.epoch,
                               step=lr_decay_epoch,
                               step_factor=args.lr_decay, 
							   power=2,
                               warmup_epochs=0)		
    
    train(train_loader, val_loader, batch_size, args.save_as, lr_scheduler)