import torch
import torch.nn.functional as F
import torch.nn as nn

class cross_entropy_loss_2d(nn.Module):
    def __init__(self, weight=None, size_average=True):
        super(cross_entropy_loss_2d, self).__init__()
        
        self.nll_loss = nn.NLLLoss2d(weight, size_average)

    def forward(self, inputs, targets):
        return self.nll_loss(F.log_softmax(inputs), targets) 
