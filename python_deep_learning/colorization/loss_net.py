import torch
import torch.nn as nn
import torchvision

"""
This is the pretrained net used to calculate preceptual loss.
Accept a list to indicate which output value of the layers
want to extract.

example:

loss_net = vgg_net([5, 10, 15])
"""
class vgg_net(nn.Module):
    def __init__(self, select):
        super(vgg_net, self).__init__()
        
        self._m_model = torchvision.models.vgg16(pretrained=True)
        self._m_select = select
        self._m_select_size = len(self._m_select)                
        
        for param in self._m_model.parameters():
            param.requires_grad = False
        
    def forward(self, x):
        features = []
        n = 0
        layers = list(self._m_model.features.children())        
        for i, layer in enumerate(layers):
            #print("i:", i)
            x = layer(x)            
            if i in self._m_select:
                n = n + 1                
                features.append(x)
            if n >= self._m_select_size:
                return features
                
        return features
