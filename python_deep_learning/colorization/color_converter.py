import numpy as np
import torch

def ycrcb_to_rgb(input_img, delta = 0.5):
    y, cr, cb = input_img[:,:,0], input_img[:,:,1], input_img[:,:,2]
    r = np.expand_dims(y + 1.403 * (cr - delta), 2)
    g = np.expand_dims(y - 0.714 * (cr - delta) - 0.344 * (cb - delta), 2)
    b = np.expand_dims(y + 1.773 * (cb - delta), 2)
    
    return np.concatenate([r, g, b], 2)

def ycrcb_to_rgb_torch(input_tensor, delta = 0.5):
    y, cr, cb = input_tensor[:,0,:,:], input_tensor[:,1,:,:], input_tensor[:,2,:,:]
    r = torch.unsqueeze(y + 1.403 * (cr - delta), 1)
    g = torch.unsqueeze(y - 0.714 * (cr - delta) - 0.344 * (cb - delta), 1)
    b = torch.unsqueeze(y + 1.773 * (cb - delta), 1)
    
    return torch.cat([r, g, b], 1)
