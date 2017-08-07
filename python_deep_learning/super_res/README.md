# Perceptual losses for super resoltuion

This is a keras implementation of the paper [Perceptual Losses for Real-Time Style Transfer
and Super-Resolution](https://arxiv.org/pdf/1603.08155.pdf)
by Justin Johnson, Alexandre Alahi, Li Fei-Fei.

The paper presents an algorithm for combining the content of one image with the style of another image using
convolutional neural networks. Here are some examples, from left to right are original image, image upscale 
4x by bicubic, image upscale by super resolution by 4x.

<img src="https://2.bp.blogspot.com/-Do1xrz0fwDg/WYgVmg1jNYI/AAAAAAAAA-Y/tQ_2VutgPrk-_NFkaid8q6XHdcBfhTDRQCLcBGAs/s1600/girl_montage.jpg" height="220px">
<img src="https://4.bp.blogspot.com/-ziJkTQO7iCs/WYgVm59_upI/AAAAAAAAA-c/ilij17E1e2c95HW7V_4afKd1ZEcYzjQHgCLcBGAs/s1600/fruits_montage.jpg" height="220px">
<img src="https://4.bp.blogspot.com/-R33w8OCHsS8/WYgVl1WAcDI/AAAAAAAAA-Q/235HB0I7zmY4fraJdUiNPghmkH9OCFTlQCLcBGAs/s1600/baby_montage.jpg" width="220px">
<img src="https://4.bp.blogspot.com/-3ktXh1uElUA/WYgVl7SiB3I/AAAAAAAAA-U/ah4SguWeAoQ2p0OOcJAokwL0R31Q9id7gCLcBGAs/s1600/butterfly_montage.jpg" width="220px">

To study more details, please check the post [Performance of perceptual losses for super resolution](http://qtandopencv.blogspot.my/2017/08/performance-of-perceptual-losses-for_7.html)

# Pretrained model

Click [here](
https://mega.nz/#!V9klRDYJ!Qz5eMgnuQh4Km34bystYG6khLSVWO2wF4E_iRLJjdLo
) to download it, this model trained on 80000 images from imagenet.

If you liked this article, please help others find it by clicking the star icon above. Thanks a lot!


