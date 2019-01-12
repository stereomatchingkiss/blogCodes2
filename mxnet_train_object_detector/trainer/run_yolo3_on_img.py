"""Train YOLOv3 with random shapes."""
import argparse
import os
import numpy as np
import mxnet as mx
from mxnet import gluon
import gluoncv as gcv
from gluoncv.model_zoo import get_model
from gluoncv.data.transforms.presets.yolo import YOLO3DefaultValTransform
from gluoncv.utils.metrics.voc_detection_2 import VOC07MApMetric

from gluoncv.utils import viz

from matplotlib import pyplot as plt
    
def parse_args():
    parser = argparse.ArgumentParser(description='Train YOLO networks with random input shape.')
    parser.add_argument('--net-params', type=str, default='',
                        help="Parameters of saved model")
    parser.add_argument('--data-shape', type=int, default=320,
                        help="Input data shape for evaluation, use 320, 416, 608... " +
                             "Training is with random shapes from (320 to 608).")    
    parser.add_argument('--num-workers', '-j', dest='num_workers', type=int,
                        default=0, help='Number of data workers, you can use larger '
                        'number to accelerate data loading, if you CPU and GPUs are powerful.')
    parser.add_argument('--gpus', type=str, default='0',
                        help='Training with GPUs, you can specify 1,3 for example.')
    parser.add_argument('--classes_list', type=str, 
                        help='Location of the classes list, every line present one class')
    parser.add_argument('--inference_folder', type=str, 
                        help='Location of folder to inference, suffix must be .jpg, .jpeg, .bmp or .png')
            
    args = parser.parse_args()
    return args
	
def read_classes(args):
    with open(args.classes_list) as f:
        return f.readlines()	

if __name__ == '__main__':
    args = parse_args()       

    # training contexts    
    ctx = mx.cpu()

    # network
    net_name = args.net_params

    net = get_model("yolo3_darknet53_custom", classes = read_classes(args), pretrained_base=True)     
    net.load_parameters(net_name)	
    net.collect_params().reset_ctx(ctx)    
	
    directory = os.fsencode(args.inference_folder)   
    for file in os.listdir(directory):
        filename = os.fsdecode(file)
        print("filename abs:", os.path.join(args.inference_folder, filename))
        if filename.endswith(".jpg") or filename.endswith(".jpeg") or filename.endswith(".png") or filename.endswith(".bmp"): 
            x, image = gcv.data.transforms.presets.yolo.load_test(os.path.join(args.inference_folder, filename), args.data_shape)
            cid, score, bbox = net(x)
            ax = viz.plot_bbox(image, bbox[0], score[0], cid[0], class_names=net.classes)
            plt.show()