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
        self._relu = nn.Activation(activation='relu')
        self._swish = nn.Swish()
        
        self._clothes_fc_1 = nn.Dense(100)
        self._clothes_bn = nn.BatchNorm(center=False, scale=True)        
        self._clothes_out = nn.Dense(num_clothes)
        
        self._clothes_fc_1.initialize(init=init.Xavier(), ctx=ctx)
        self._clothes_bn.initialize(init=init.Zero(), ctx=ctx)
        self._clothes_out.initialize(init=init.Xavier(), ctx=ctx)
		
        self._color_fc_1 = nn.Dense(100)
        self._color_bn_1 = nn.BatchNorm(center=False, scale=True)
        self._color_fc_2 = nn.Dense(50)
        self._color_bn_2 = nn.BatchNorm(center=False, scale=True)
        self._color_out = nn.Dense(num_colors)
		
        self._color_fc_1.initialize(init=init.Xavier(), ctx=ctx)
        self._color_bn_1.initialize(init=init.Zero(), ctx=ctx)
        self._color_fc_2.initialize(init=init.Xavier(), ctx=ctx)
        self._color_bn_2.initialize(init=init.Zero(), ctx=ctx)
        self._color_out.initialize(init=init.Xavier(), ctx=ctx)
		
    def hybrid_forward(self, F, x):
        x = self._features(x)
		
        clothes_result = self._flatten(x)
        clothes_result = self._clothes_fc_1(clothes_result)
        clothes_result = self._swish(clothes_result)
        clothes_result = self._clothes_bn(clothes_result)
        clothes_result = self._clothes_out(clothes_result)        
		
        color_result = self._flatten(x)
        color_result = self._color_fc_1(color_result)
        color_result = self._swish(color_result)
        color_result = self._color_bn_1(color_result)
        color_result = self._color_fc_2(color_result)
        color_result = self._swish(color_result)
        color_result = self._color_bn_2(color_result)        
        color_result = self._color_out(color_result)        
		
        return clothes_result, color_result