import sys
import os
import argparse
import json
import cv2
import mxnet as mx
import numpy as np
import collections
from collections import namedtuple
import random_crop as rc

ap = argparse.ArgumentParser()
ap.add_argument("-f", "--file", required=True, help="Location of setup file")

args = vars(ap.parse_args())

parsed_json = None
with open(args["file"]) as json_data:
    parsed_json = json.load(json_data)

def _convert_img_to_mxnet_format(input_img, im_dim):
	if input_img is None:
		return None
	img = cv2.cvtColor(input_img, cv2.COLOR_BGR2RGB)
	img = cv2.resize(img, (im_dim, im_dim))
	img = np.swapaxes(img, 0, 2)
	img = np.swapaxes(img, 1, 2) 
	img = img[np.newaxis, :]

	return img

def _load_model(model, epoch, im_dim, im_depth):
	#sym, arg_params, aux_params = mx.model.load_checkpoint(parsed_json["model"], parsed_json["model_epoch"])
	sym, arg_params, aux_params = mx.model.load_checkpoint(model, epoch)
	mod = mx.mod.Module(symbol=sym, context=mx.gpu(0))		
	mod.bind(for_training=False, data_shapes=[('data', (1,im_depth,im_dim,im_dim))])
	mod.set_params(arg_params, aux_params, allow_missing=True)

	return mod	

def _predict_prob(model, img, im_dim):
	img = _convert_img_to_mxnet_format(img, im_dim)
	model.forward(Batch([mx.nd.array(img)]))
	prob = model.get_outputs()[0].asnumpy()
	return np.squeeze(prob)

with open(parsed_json["output"], "w") as wfile:
	wfile.write("id,label\n")
	random_crop_size = int(parsed_json["random_crop"])
	im_dim = int(parsed_json["im_dim"])
	rand_crop = rc.random_cropper(random_crop_size, im_dim, im_dim)	
	im_dir = parsed_json["im_dir"]
	img_files = [os.path.join(im_dir, f) for f in os.listdir(im_dir) if os.path.isfile(os.path.join(im_dir, f))]
	model = _load_model(parsed_json["model"], parsed_json["model_epoch"], parsed_json["im_dim"], parsed_json["im_depth"])
	lower_bound = parsed_json["clip_lower_bound"]
	upper_bound = parsed_json["clip_upper_bound"]		
	Batch = namedtuple('Batch', ['data'])
	result = {}
	index = 0
	for filename in img_files:		
		print "process " + str(index) + " : " + filename
		index = index + 1
		im_name = filename.split("/")
		im_name = im_name[len(im_name)-1]
		im_name = (int)(im_name.replace(".jpg", ""))
		avg_prob = 0
		img = cv2.imread(filename)
		if img is None:
			raise Exception(filename + " is None")		
		if random_crop_size <= 1:
			prob = _predict_prob(model, img, im_dim)
			avg_prob = avg_prob + prob[1]
		else:
			for crop_img in rand_crop.crop(img):
				prob = _predict_prob(model, crop_img, im_dim)
				avg_prob = avg_prob + prob[1]
		
		avg_prob = avg_prob / random_crop_size
		if parsed_json["clip"] == True:		
			if avg_prob < lower_bound:
				avg_prob = lower_bound
			elif avg_prob > upper_bound:
				avg_prob = upper_bound
		result[im_name] = avg_prob

	for keys, values in sorted(result.items()):
		wfile.write("{},{:1.17f}\n".format(keys, values))
