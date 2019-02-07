import mxnet as mx
from mxnet.gluon.data import dataset
import os

class MultiOutputImageDataset(dataset.Dataset):
    """
    A dataset for loading images with multi outputs. 
	
	1. Each label should be separated by "_"
    2. Same category should put at the same order
    ex : red_shoe, blue_shirt, black_jean
	3. The folders architectures
	
	red_shirt
	  --img0.jpg
	  --img1.jpg
	red_jean
	  --img0.jpg
	  --img1.jpg
	blue_shirt
      --img0.jpg
	blue_jean
	  --img0.jpg
		
	4.  Example
    
    MultiOutputImageDataset("./images", [{"red" : 0, "blue" : 1}, {"shirt" : 0, "jean" : 1}])

    The results returned by __getitem__ will be
	path of image               number after mapping
	
	root/red_shirt/img0.jpg     [0, 0]
	root/red_jean/img1.jpg      [0, 1]
	root/blue_shirt/img2.jpg    [1, 1]
	root/blue_jean/img3.jpg     [1, 0]	

    Parameters
    ----------
    root : str
        Path to root directory.
    transform : callable, default None
        A function that takes data and label and transforms them:
    ::
        transform = lambda data, label: (data.astype(np.float32)/255, label)
    """
    def __init__(self, root, label_dict, transform=None):
        self._root = os.path.expanduser(root)
        self._label_dict = label_dict
        self._transform = transform
        self._exts = ['.jpg', '.jpeg', '.png']
        self._list_images(self._root)

    def _list_images(self, root):
        self._image_list = []        
        self._label_list = []        
        for dir in os.listdir(root):                         
            for filename in os.listdir(root + "/" + dir):
                name, ext = os.path.splitext(filename)
                if ext.lower() not in self._exts:
                    continue
                else:
                    labels = dir.split("_")
                    num_of_labels = []
                    for i in range(len(labels)):
                        num_of_labels.append(self._label_dict[i][labels[i]])
                    self._image_list.append(root + "/" + dir + "/" + filename)
                    self._label_list.append(num_of_labels)
	                                        
    def __getitem__(self, idx):
        return self._image_list[i], self._label_list[i]

    def __len__(self):
        return len(self._image_list)