import mxnet as mx
from mxnet.gluon.data import dataset
import os

class MultiLabelImageDataset(dataset.Dataset):
    """
    A dataset for loading images with multi labels. 
	
	1. Each label should be separated by "_"
	2. The images are store as 
	
	label0_label1_label2
	  --img0.jpg
	  --img1.jpg
	label1_label4_label5
	  --img4.jpg
	  --img5.jpg
	....
	....
	....		
		
	3.  The results returned by __getitem__ will be
	path of image     number after mapping
	
	root/img0.jpg     [0, 1]
	root/img1.jpg     [0, 1]
	root/img2.jpg     [2, 3]
	root/img3.jpg     [2, 3]	

    Parameters
    ----------
    root : str
        Path to root directory.
    transform : callable, default None
        A function that takes data and label and transforms them:
    ::
        transform = lambda data, label: (data.astype(np.float32)/255, label)
    """
    def __init__(self, root, transform=None):
        self._root = os.path.expanduser(root)
        self._transform = transform
        self._exts = ['.jpg', '.jpeg', '.png']
        self._list_images(self._root)

    def _list_images(self, root):
        self._image_list = []
        self._label_dict = {}
        self._label_list = []
        label_index = 0;
        for dir in os.listdir(root):                         
            for filename in os.listdir(root + "/" + dir):
                name, ext = os.path.splitext(filename)
                if ext.lower() not in self._exts:
                    continue
                else:
                    labels = dir.split("_")
                    num_of_labels = []
                    for l in labels:
                        if l not in self._label_dict:					
                            self._label_dict[l] = label_index
                            label_index = label_index + 1
                        num_of_labels.append(self._label_dict[l])
                    self._image_list.append(root + "/" + dir + "/" + filename)
                    self._label_list.append(num_of_labels)
	                                        
    def __getitem__(self, idx):
        return self._image_list[i], self._label_list[i]

    def __len__(self):
        return len(self._image_list)