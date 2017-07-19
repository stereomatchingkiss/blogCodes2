from PIL import Image

import os

import keras.backend as K
import vgg16_avg

from keras.models import Model
from keras.layers import *
from keras import metrics

from scipy.optimize import fmin_l_bfgs_b
from scipy.misc import imsave

#Implement algorithm mentioned in the paper "A Neural Algorithm of Artistic Style".
#Following codes are come from fast.ai courses

rn_mean = np.array([123.68, 116.779, 103.939], dtype=np.float32)

#create image close to zero mean and convert rgb channel to bgr channel since the vgg model need bgr channel
#the value of rn_mean is come from image net data set
preproc = lambda x: (x - rn_mean)[:,:,:,::-1]
deproc = lambda x: x[:,:,:,::-1] + rn_mean

dpath= os.getcwd() + "/"

#I make the size of content image, style image, generated img
#have the same shape, but this is not mandatory
#since we do not use any full connection layer
def read_img(im_name, shp):
    style_img = Image.open(im_name)
    if len(shp) > 0:
        style_img = style_img.resize((shp[2], shp[1]))
    style_arr = np.array(style_img)    
    #The image read by PIL is three dimensions, but the model
    #need a four dimensions tensor(first dim is batch size)
    style_arr = np.expand_dims(style_arr, 0)
    
    return preproc(style_arr)

content_img_name = "dog"
content_img_arr = read_img(dpath + "img/{}.png".format(content_img_name), [])
content_shp = content_img_arr.shape
style_img_arr = read_img(dpath + "img/starry.png", content_shp)

content_base = K.variable(content_img_arr)
style_base = K.variable(style_img_arr)
gen_img = K.placeholder(content_shp)
batch = K.concatenate([content_base, style_base, gen_img], 0)

#Feed the batch into the vgg model, every time we call the model/layer to
#generate output, it will generate output of content_base, style_base,
#gen_img.
model = vgg16_avg.VGG16_Avg(input_tensor = batch, include_top=False)

outputs = {l.name:l.output for l in model.layers}
style_layers = [outputs['block{}_conv1'.format(i)] for i in range(1,4)]
content_layer = outputs['block4_conv2']

#gram matrix is a matrix collect the correlation of all of the vectors
#in a set. Check wiki(https://en.wikipedia.org/wiki/Gramian_matrix) for more details
def gram_matrix(x):
    #change height,width,depth to depth, height, width, it could be 2,1,0 too
    #maybe 2,0,1 is more efficient due to underlying memory layout
    features = K.permute_dimensions(x, (2,0,1))
    #batch flatten make features become 2D array
    features = K.batch_flatten(features)
    return K.dot(features, K.transpose(features)) / x.get_shape().num_elements()    

def style_loss(x, targ):
    return metrics.mse(gram_matrix(x), gram_matrix(targ))
    
content_loss = lambda base, gen: metrics.mse(gen, base)    

#l[1] is the output(activation) of style_base, l[2] is the output of gen_img
loss = sum([style_loss(l[1], l[2]) for l in style_layers]) #loss of style image and gen_img

#content_layer[0] is the output of content_base, content_layer[2] is the output of gen_img
loss += content_loss(content_layer[0], content_layer[2]) / 10. #loss of content image and gen_img

#The loss need two variables but we only pass in one,
#because we only got one placeholder in the graph,
#the other variable already determine by K.variable
grad = K.gradients(loss, gen_img)
fn = K.function([gen_img], [loss] + grad)

#fn will return loss and grad, but fmin_l_bfgs need to seperate them
#that is why we need a class to separate loss and gradient and store them
class Evaluator:
    def __init__(self, fn_, shp_):
        self.fn = fn_
        self.shp = shp_
        
    def loss(self, x):
        loss_, grads_ = self.fn([x.reshape(self.shp)])
        self.grads = grads_.flatten().astype(np.float64)
        
        return loss_.astype(np.float64)
    
    def grad(self, x):
        return np.copy(self.grads)
    
evaluator = Evaluator(fn, content_shp)

def solve_img(evalu, niter, x):
    for i in range(0, niter):
        x, min_val, info = fmin_l_bfgs_b(evalu.loss, x.flatten(), fprime=evalu.grad, maxfun = 20)
        #value of PIL lie within -127 and 127
        x = np.clip(x, -127, 127)
        print(i, ',Current loss value:', min_val)
        x = x.reshape(content_shp)
        simg = deproc(x.copy())
        img_name = '{}_{}_neural_style_img_{}.png'.format(dpath + "gen_img/", content_img_name, i)
        imsave(img_name, simg[0])
    return x

#This is the real value of the placeholder--gen_img
rand_img = lambda shape: np.random.uniform(-2.5, 2.5, shape)/100

solve_img(evaluator, 10, rand_img(content_shp)/10.)
