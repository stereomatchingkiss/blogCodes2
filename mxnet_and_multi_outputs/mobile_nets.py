from gluoncv import model_zoo
from mxnet.gluon import nn, HybridBlock
from mxnet import init

import mxnet as mx

class fashion_net_2_branches(HybridBlock):
    def __init__(self, num_clothes, num_colors, ctx):
        super(fashion_net_2_branches, self).__init__()
        self._features = model_zoo.get_model('mobilenetv2_1.0', pretrained=True, ctx = mx.gpu()).features
        for _, w in self._features.collect_params().items():
            w.grad_req = 'null'
			
        self._flatten = nn.Flatten()
        
        self._clothes_conv = nn.Conv2D(channels=100, kernel_size=(1,1), activation='relu')
        self._clothes_bn = nn.BatchNorm(center=False, scale=True)        
        self._clothes_out = nn.Conv2D(channels=num_clothes, kernel_size=(1,1), activation='relu')
        
        self._clothes_conv.initialize(init=init.Normal(0.001), ctx=ctx)
        self._clothes_bn.initialize(init=init.Zero(), ctx=ctx)
        self._clothes_out.initialize(init=init.Normal(0.001), ctx=ctx)
		
        self._color_conv = nn.Conv2D(channels=100, kernel_size=(1,1), activation='relu')
        self._color_bn = nn.BatchNorm(center=False, scale=True)
        self._color_out = nn.Conv2D(channels=num_colors, kernel_size=(1,1), activation='relu')
		
        self._color_conv.initialize(init=init.Normal(0.001), ctx=ctx)
        self._color_bn.initialize(init=init.Zero(), ctx=ctx)
        self._color_out.initialize(init=init.Normal(0.001), ctx=ctx)
		
    def hybrid_forward(self, F, x):
        x = self._features(x)
		
        clothes_result = self._clothes_conv(x)
        clothes_result = self._clothes_bn(clothes_result)
        clothes_result = self._clothes_out(clothes_result)
        clothes_result = self._flatten(clothes_result)
		
        color_result = self._color_conv(x)
        color_result = self._color_bn(color_result)
        color_result = self._color_out(color_result)
        color_result = self._flatten(color_result)
		
        return clothes_result, color_result