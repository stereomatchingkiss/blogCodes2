from color_converter import ycrcb_to_rgb_torch

import torch
import torch.nn as nn

class conv_block(nn.Module):
    def __init__(self, in_size, out_size, kernal = 3, stride = 1, activate = True):
        super(conv_block, self).__init__()
        
        padding = (kernal-1)//2
        self._m_conv = nn.Sequential(
            nn.Conv2d(in_size, out_size, kernal, stride, padding),
            nn.BatchNorm2d(out_size)            
        )
        if activate:
            self._m_conv.add_module('conv_block_relu', nn.ReLU())
        
    def forward(self, x):
        out = self._m_conv(x)
        return out
    
class res_block(nn.Module):
    def __init__(self, in_size, out_size, kernal = 3, stride = 1):
        super(res_block, self).__init__()
                
        self._m_conv1 = conv_block(in_size, out_size, kernal, stride)
        self._m_conv2 = conv_block(out_size, out_size, kernal, stride)
        
    def forward(self, x):
        out = self._m_conv1(x)
        out = self._m_conv2(out)        
        out = out + x
        return out

"""
This net accept Y channel of YCrCb(convert from rgb image which follow the formula from opencv) 
within [0,255].
"""
class colorization_net(nn.Module):
    def __init__(self):
        super(colorization_net, self).__init__()
        
        self._m_conv1 = conv_block(1, 64, 9)
        self._m_res1 = res_block(64, 64)
        self._m_res2 = res_block(64, 64)
        self._m_res3 = res_block(64, 64)
        self._m_res4 = res_block(64, 64)
        self._m_conv2 = nn.Sequential(
            #nn.Convd2d(64, 2, 9, 1, 4), #for solution 1 and 2
            nn.Conv2d(64, 3, 9, 1, 4), #for solution 3
            nn.Tanh()
        )
        self._m_tanh = nn.Tanh()
        
    def forward(self, x):        
        out = self._m_conv1(x)
        out = self._m_res1(out)
        out = self._m_res2(out)
        out = self._m_res3(out)
        out = self._m_res4(out)
        out = self._m_conv2(out)
        out = out * 127 + 128 #out predict CrCb for solution 1 and 2, predict rgb for solution 3
        #out = torch.cat([x, out], 1) #Now out become YCrCb
        
        #I give 3 solutions a try, none of them work
                        
        #Solution 1 : clamp value to [0,255]
        #Converted value may not fall in [0,255], so I clamp it to [0, 255]
        #but this solution will generate image with the image only with one color
        #If I do not clamp it, generated color is very weird
        #This function convert ycrcb to rgb, follow the formula from opencv
        #I transform out back to rgb because vgg expect the color space of input as RGB
        #out = ycrcb_to_rgb_torch(out, 128)
        #out = out.clamp(0, 255)
        
        #Solution 2 : instead of clamp the output to [0,255]
        #pass it to activation again, this generate image
        #close to plain white
        #out = ycrcb_to_rgb_torch(out, 128)
        #out = self._m_tanh(out)
        #out = out * 127 + 128 
        
        #Solution 3 : I change the lass convolution net from 
        #nn.Conv2d(64, 2, 9, 1, 4) to nn.Conv2d(64, 3, 9, 1, 4)
        #assume the net will generate the rgb image directly
        
        return out
