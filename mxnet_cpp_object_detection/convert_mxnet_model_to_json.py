import gluoncv as gcv
from gluoncv.utils import export_block

net = gcv.model_zoo.get_model('yolo3_darknet53_coco', pretrained=True)
export_block('yolo3_darknet53_coco', net)