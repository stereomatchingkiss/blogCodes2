"""Train YOLOv3 with random shapes."""
import argparse
import os
import time
import warnings
import numpy as np
import mxnet as mx
from mxnet import nd
from mxnet import gluon
import gluoncv as gcv
from gluoncv.model_zoo import get_model
from gluoncv.data.batchify import Tuple, Stack, Pad
from gluoncv.data.transforms.presets.yolo import YOLO3DefaultValTransform
from gluoncv.utils.metrics.voc_detection_2 import VOC07MApMetric

from matplotlib import pyplot as plt

def restricted_float(x):
    x = float(x)
    if x < 0.0 or x > 1.0:
        raise argparse.ArgumentTypeError("%r not in range [0.0, 1.0]"%(x,))
    return x
    
def parse_args():
    parser = argparse.ArgumentParser(description='Train YOLO networks with random input shape.')
    parser.add_argument('--net-params', type=str, default='',
                        help="Parameters of saved model")
    parser.add_argument('--data-shape', type=int, default=320,
                        help="Input data shape for evaluation, use 320, 416, 608... " +
                             "Training is with random shapes from (320 to 608).")
    parser.add_argument('--batch-size', type=int, default=8, help='Training mini-batch size')
    parser.add_argument('--num-workers', '-j', dest='num_workers', type=int,
                        default=0, help='Number of data workers, you can use larger '
                        'number to accelerate data loading, if you CPU and GPUs are powerful.')
    parser.add_argument('--gpus', type=str, default='0',
                        help='Training with GPUs, you can specify 1,3 for example.')
    parser.add_argument('--classes_list', type=str, 
                        help='Location of the classes list, every line present one class')
    parser.add_argument('--validate_dataset', type=str, 
                        help='Location of validate dataset, must be rec format')
    parser.add_argument('--iou', type=restricted_float, default="0.5", help='IOU overlap threshold of validate metric')
            
    args = parser.parse_args()
    return args
	
def read_classes(args):
    with open(args.classes_list) as f:
        return f.readlines()
		
def get_dataset(args):     
    val_dataset = gcv.data.RecordFileDetection(args.validate_dataset)
    classes = read_classes(args)
    val_metric = VOC07MApMetric(iou_thresh=args.iou, class_names=classes)
    
    return val_dataset, val_metric

def get_dataloader(net, val_dataset, data_shape, batch_size, num_workers, args):
    """Get dataloader."""
    width, height = data_shape, data_shape
    val_batchify_fn = Tuple(Stack(), Pad(pad_val=-1))    
    val_loader = gluon.data.DataLoader(
        val_dataset.transform(YOLO3DefaultValTransform(width, height)),
        batch_size, True, batchify_fn=val_batchify_fn, last_batch='keep', num_workers=num_workers)
    return val_loader
	
def validate(net, val_data, ctx, eval_metric):
    """Test on validation dataset."""
    eval_metric.reset()
    # set nms threshold and topk constraint
    net.set_nms(nms_thresh=0.45, nms_topk=400)
    mx.nd.waitall()
    net.hybridize()
	    
    for batch in val_data:
        data = gluon.utils.split_and_load(batch[0], ctx_list=ctx, batch_axis=0, even_split=False)
        label = gluon.utils.split_and_load(batch[1], ctx_list=ctx, batch_axis=0, even_split=False)
        det_bboxes = []
        det_ids = []
        det_scores = []
        gt_bboxes = []
        gt_ids = []        
        for x, y in zip(data, label):
            # get prediction results
            ids, scores, bboxes = net(x)
            #print("type of net return:", type(ids), type(scores), type(bboxes))
            det_ids.append(ids)
            det_scores.append(scores)
            # clip to image size
            det_bboxes.append(bboxes.clip(0, batch[0].shape[2]))
            # split ground truths
            gt_ids.append(y.slice_axis(axis=-1, begin=4, end=5))
            gt_bboxes.append(y.slice_axis(axis=-1, begin=0, end=4))

        # update metric        
        eval_metric.update(det_bboxes, det_ids, det_scores, gt_bboxes, gt_ids)
    return eval_metric.get()

if __name__ == '__main__':
    args = parse_args()       

    # training contexts
    ctx = [mx.gpu(int(i)) for i in args.gpus.split(',') if i.strip()]
    ctx = ctx if ctx else [mx.cpu()]

    # network
    net_name = args.net_params

    net = get_model("yolo3_darknet53_custom", classes = read_classes(args), pretrained_base=True)     
    net.load_parameters(net_name)	
    net.collect_params().reset_ctx(ctx)
	
    val_dataset, val_metric = get_dataset(args)
    val_loader = get_dataloader(net, val_dataset, args.data_shape, args.batch_size, args.num_workers, args)
    map_name, mean_ap = validate(net, val_loader, ctx, val_metric)
    val_msg = '\n'.join(['{}={}'.format(k, v) for k, v in zip(map_name, mean_ap)])
    print('Validation: \n{}'.format(val_msg))	   