import torch
import numpy as np

class flip_horizontal(object):
    """Flip ndarray horizontally with a probability of 0.5, this function will flip the input inplace
       and wouldn't generate a copy
       Args:
            sample(dictionary) : This dictionary contain two keys, 'raw' and 'label'.
                                 Both of the values are ndarray(H x W x C)
    """
    
    def __call__(self, sample):        
        if np.random.randint(0, 2) == 1:            
            raw = sample['raw'][:, ::-1, ...]
            label = sample['label'][:, ::-1, ...]
            #sample['raw'][:, ::-1] do not work, so I use swapaxes to flip the image horizontally
            return {'raw': np.swapaxes(np.swapaxes(raw, 0, 1)[::-1], 0, 1), 'label' : np.swapaxes(np.swapaxes(label, 0, 1)[::-1], 0, 1)}
        else:
            return sample
        
class label_to_integer(object):
    """map each unique pixel color to integer so we could classify with NLLLoss2d.
    This transformation should place as the last one or before to_tensor. This function is
    slow, better preprocess and save the images on hard disk rather than call in again and
    again when training
    
    Args:
        label_color(list) : A list which store the color, color will transforme to integer 
        by the order.
        fail_code(int) : If the color do not exist in label_color, fail_code will be the value
        of the label
        
    Example:
        #(255, 128, 33)->0, (128, 128, 128)->1
        label_color = [(255,128,33), (128,128,128)]
        l_to_i = label_to_integer(label_color, 0)        
    """
    
    def __init__(self, label_color, fail_code):
        self._mfail_code = fail_code
        self._mcolor_to_integer = None
        for color in label_color:
            self._mcolor_to_integer = {color : i for i, color in enumerate(label_color)}
      
    """
    Args:
        label_img(ndarray) : The 'label' is a ndarray(H x W x C)
    """
    def __call__(self, label_img):        
        new_label = np.zeros((label_img.shape[0], label_img.shape[1]), 'int64')
        for row in range(label_img.shape[0]):
            for col in range(label_img.shape[1]):
                pix = label_img[row, col]
                pix = (pix[0], pix[1], pix[2])
                if pix in self._mcolor_to_integer:
                    new_label[row, col] = self._mcolor_to_integer[pix]
                else:
                    new_label[row, col] = self._mfail_code
                    
        return new_label
    
class normalize(object):
    """Normalize input image
    Args:
        mean(list) : list of the mean of r,g,b channels
        copy(bool) : True will copy the data, else it will change the 
        data inplace
    """
    def __init__(self, mean = [104.00699, 116.66877, 122.67892], copy = True):
        self._mcopy = copy
        self._mmean = np.array(mean)        
        
    def __call__(self, raw_img):
        raw = None
        if self._mcopy:
            raw = raw_img.copy()
        else:
            raw = raw_img
            
        if(raw.dtype != 'float32'):
            raw = raw.astype('float32')
        raw -= self._mmean
        raw /= 255.0
        
        return raw

class random_crop(object):
    """Crop randomly the image in a sample. It is recommended to set this
    as a first transformation to take out

    Args:
        output_size (tuple or int): Desired output size. If int, square crop
            is made.
        copy (bool): Create new instance if true, else create a view
    """

    def __init__(self, output_size, copy = True):
        assert isinstance(output_size, (int, tuple))
        if isinstance(output_size, int):
            self._moutput_size = (output_size, output_size)
        else:
            assert len(output_size) == 2
            self._moutput_size = output_size
            
        self._mcopy = copy

    def __call__(self, sample):
        image, label = sample['raw'], sample['label']

        h, w = image.shape[:2]
        new_h, new_w = self._moutput_size

        top = np.random.randint(0, h - new_h)
        left = np.random.randint(0, w - new_w)
        
        image = image[top: top + new_h,
                        left: left + new_w]
        label = label[top: top + new_h,
                      left: left + new_w]        

        if self._mcopy:
            image = image.copy()
            label = label.copy()
        
        return {'raw': image, 'label': label}    
    
    
class to_tensor(object):
    """Convert ndarrays in sample to Tensors."""

    def __call__(self, sample):
        image, label = sample['raw'], sample['label']

        # swap color axis because
        # numpy image: H x W x C
        # torch image: C X H X W
        image = image.transpose((2, 0, 1))
        if len(label.shape) == 3:
            label = label.transpose((2, 0, 1))
        return {'raw': torch.from_numpy(image),
                'label': torch.from_numpy(label)}
