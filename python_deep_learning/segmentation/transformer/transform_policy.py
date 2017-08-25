import torch
import numpy as np

class flip_horizontal(object):
    """Flip ndarray horizontally with a probability of 0.5
    """
    
    def __call__(self, sample):        
        if np.random.randint(0, 2) == 1:            
            #return {'raw' : sample['raw'][:, ::-1, :], 'label' : sample['label'][:, ::-1, :]}
            print('shape of fh:', sample['raw'].shape)
            raw = sample['raw'][:, ::-1, ...]
            label = sample['label'][:, ::-1, ...]
            return {'raw': np.swapaxes(np.swapaxes(raw, 0, 1)[::-1], 0, 1), 'label' : np.swapaxes(np.swapaxes(label, 0, 1)[::-1], 0, 1)}
        else:
            return sample

class random_crop(object):
    """Crop randomly the image in a sample.

    Args:
        output_size (tuple or int): Desired output size. If int, square crop
            is made.
    """

    def __init__(self, output_size):
        assert isinstance(output_size, (int, tuple))
        if isinstance(output_size, int):
            self._moutput_size = (output_size, output_size)
        else:
            assert len(output_size) == 2
            self._moutput_size = output_size

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

        return {'raw': image, 'label': label}    
    
    
class to_tensor(object):
    """Convert ndarrays in sample to Tensors."""

    def __call__(self, sample):
        image, label = sample['raw'], sample['label']

        # swap color axis because
        # numpy image: H x W x C
        # torch image: C X H X W
        image = image.transpose((2, 0, 1))
        return {'raw': torch.from_numpy(image),
                'label': torch.from_numpy(label)}
