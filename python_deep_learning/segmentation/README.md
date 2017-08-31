# Open semantic segmentation 

This project implement semantic segmentation record in [LinkNet: Exploiting Encoder Representations for Efficient Semantic Segmentation](https://export.arxiv.org/pdf/1707.03718)

The paper presents an algorithm for a real-time semantic network able to run on embedded without sacrifice accuracy.

From left to right is original image, ground truth(I mark all of the pixels do not exist 
in the category as void) and predicted result.

<img src="https://4.bp.blogspot.com/-vkyzSl2mRvE/Waffgnio-WI/AAAAAAAABAU/PjANC0oAzDwwXZopgRqXvG3d9LE4NtyqwCLcBGAs/s1600/model_accuracy.png">
<img src="https://3.bp.blogspot.com/-fw6NUSLPCJQ/Waff7uUUTEI/AAAAAAAABA0/bCLqhhO3axYB0oti1_wVZDwTa0_cLCqhwCLcBGAs/s1600/0016E5_05100_mongtage.jpg" height="220px">
<img src="https://1.bp.blogspot.com/-8mUX_BliJZE/Waff61smTYI/AAAAAAAABAs/9qLXtQerFp8v3fTlg7FQoOs0xlF_U8kswCLcBGAs/s1600/0016E5_01230_mongtage.jpg" height="220px">
<img src="https://2.bp.blogspot.com/-2atvZrqBSEY/Waff_PvOepI/AAAAAAAABBk/TogHIkVKenICax-arsssq0Gc4FhUs2o2gCLcBGAs/s1600/Seq05VD_f03840_mongtage.jpg" height="220px">

To study more details, please check the post [Let us create a semantic segmentation model(LinkNet) by PyTorch](https://www.blogger.com/blogger.g?blogID=4702230343097536610#editor/target=post;postID=8891469693003535684;onPublishedMenu=allposts;onClosedMenu=allposts;postNum=0;src=link)

# dependencies

1. python >= 3.5
2. [PyTorch](http://pytorch.org/)
3. cuda and cudnn
4. numpy
5. matplotlib
6. tkinter

# How to train

1. git clone https://github.com/mostafaizz/camvid
2. git clone https://github.com/stereomatchingkiss/blogCodes2
3. go to /your_path/blogCodes2/segmentation
4. copy label_int.txt to /your_path2/camvid
5. python3 process_camvid_data.py --camvid_folder /your_path2/camvid
6. python3 trainer.py --camvid_folder /your_path2/camvid

# How to test

1. python3 test.py --camvid_folder /your_path/camvid/ --model /your_path2/model_name

# Model

[Model trained with 368 images, 12 labels](
https://mega.nz/#!lhEwlJCT!YIwOLPKqF5QahbZ8EtOZKQQ-qQ_GZGBmxZ31SVgxwTI
).


# To do

1. Train on larger dataset like cityscapes(Have to find another dataset because I do not have the right to access cityscapes).
2. Implement another segmentation algorithm
3. Improve result of LinkNet, like using dilated convolution to replace normal convolution

If you liked this article, please help others find it by clicking the star icon above. Thanks a lot!



