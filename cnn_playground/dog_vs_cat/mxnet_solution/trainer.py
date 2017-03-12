import argparse
import mxnet as mx
import numpy as np
import json

ap = argparse.ArgumentParser()
ap.add_argument("-f", "--file", required=True, help="Location of setup file")

args = vars(ap.parse_args())

parsed_json = None
with open(args["file"]) as json_data:
    parsed_json = json.load(json_data)

def _get_iterators(batch_size, augment, data_shape=(parsed_json["im_depth"], parsed_json["im_dim"], parsed_json["im_dim"])):
	if augment:
		train = mx.io.ImageRecordIter(
            path_imgrec         = parsed_json["train_rec"],
            data_name           = 'data',
            label_name          = 'softmax_label',
            batch_size          = batch_size,
            data_shape          = data_shape,
            shuffle             = True,
            max_rotate_angle    = parsed_json["max_rotate_angle"],
            max_shear_ratio     = parsed_json["max_shear_ratio"],
            rand_crop           = parsed_json["rand_crop"],
            min_crop_size       = parsed_json["min_crop_size"],
            max_crop_size       = parsed_json["max_crop_size"],
            random_h            = parsed_json["random_h"],
            random_s            = parsed_json["random_s"],
            random_l            = parsed_json["random_l"],
            rand_mirror         = parsed_json["rand_mirror"])
		val = mx.io.ImageRecordIter(
            path_imgrec         = parsed_json["val_rec"],
            data_name           = 'data',
            label_name          = 'softmax_label',
            batch_size          = batch_size,
            data_shape          = data_shape,
            shuffle             = True,
            max_rotate_angle    = parsed_json["max_rotate_angle"],
            max_shear_ratio     = parsed_json["max_shear_ratio"],
            rand_crop           = parsed_json["rand_crop"],
            min_crop_size       = parsed_json["min_crop_size"],
            max_crop_size       = parsed_json["max_crop_size"],
            random_h            = parsed_json["random_h"],
            random_s            = parsed_json["random_s"],
            random_l            = parsed_json["random_l"],
            rand_mirror         = parsed_json["rand_mirror"])
	else:
		train = mx.io.ImageRecordIter(
            path_imgrec         = parsed_json["train_rec"],
            data_name           = 'data',
            label_name          = 'softmax_label',
            batch_size          = batch_size,
            data_shape          = data_shape,
            rand_crop           = False,
            rand_mirror         = False)
		val = mx.io.ImageRecordIter(
            path_imgrec         = parsed_json["val_rec"],
            data_name           = 'data',
            label_name          = 'softmax_label',
            batch_size          = batch_size,
            data_shape          = data_shape,
            rand_crop           = False,
            rand_mirror         = False)
	
	return (train, val)

def _get_fine_tune_model(symbol, arg_params, num_classes, layer_name='flatten0'):
	"""
	symbol: the pre-trained network symbol
	arg_params: the argument parameters of the pre-trained model
	num_classes: the number of classes for the fine-tune datasets
	layer_name: the layer name before the last fully-connected layer
	"""
	all_layers = symbol.get_internals()
	net = all_layers[layer_name+'_output']
	net = mx.symbol.FullyConnected(data=net, num_hidden=num_classes, name='fc1')
	net = mx.symbol.SoftmaxOutput(data=net, name='softmax')
	new_args = dict({k:arg_params[k] for k in arg_params if 'fc1' not in k})
	return (net, new_args)

import logging
head = '%(asctime)-15s %(message)s'
logging.basicConfig(level=logging.DEBUG, format=head)

def _fit(symbol, arg_params, aux_params, train, val, batch_size):
	model_prefix = parsed_json["trained_model"]
	checkpoint = mx.callback.do_checkpoint(model_prefix)
	metric = mx.metric.create('acc')
	mod = mx.mod.Module(symbol=symbol, context=mx.gpu(0))
	mod.bind(data_shapes=train.provide_data, label_shapes=train.provide_label)
	mod.init_params(initializer=mx.init.Xavier(rnd_type='gaussian', factor_type="in", magnitude=2))
	mod.set_params(arg_params, aux_params, allow_missing=True)
	optimizer = mx.optimizer.SGD(learning_rate=parsed_json["learning_rate"])
	mod.fit(train, val, 
        num_epoch=parsed_json["train_epoch"],
        batch_end_callback = mx.callback.Speedometer(batch_size, 10),        
        kvstore='device',
        optimizer='adam',
        optimizer_params={'learning_rate':parsed_json["learning_rate"]},
		#optimizer=optimizer,
        eval_metric=metric,
        epoch_end_callback = checkpoint)
	return (mod.score(val, 'acc'), mod, metric)

num_classes = parsed_json["num_classes"]

if parsed_json["training"]:
	(train, val) = _get_iterators(parsed_json["batch_size"], parsed_json["augment_data"])
	sym, arg_params, aux_params = mx.model.load_checkpoint(parsed_json["pretrain_model"], 0)
	(new_sym, new_args) = _get_fine_tune_model(sym, arg_params, num_classes)

	mod_score, mod, metric = _fit(new_sym, new_args, aux_params, train, val, parsed_json["batch_size"])
	print mod_score

	for name, val in metric.get_name_value():
		print('%s=%f' % (name, val))
else:
	_print_category_acc()






