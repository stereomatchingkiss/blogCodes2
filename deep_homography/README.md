# Deep homography

This project implement the algorithm record in the paper [Deep Image Homography Estimation](https://arxiv.org/pdf/1606.03798.pdf).
The network is trained and tested by torch, the other part are implemented by c++.
If you want to know how to algorithm work, please check this [blog](http://qtandopencv.blogspot.my/2017/10/modern-way-to-estimate-homography.html)

# Project layout

This project composed by 4 parts, they are 

1. cnn, this part incharge training and testing of cnn.
2. data_generator, this part generate the train data and test data
3. generate_train_result, this part use the trained model to generate the results

# How to train and test the network

1. [Generate train and test data](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/data_generator)
2. [Call the script to train and test the generated data](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/cnn/torch)
3. (Optional)[Generate train results on image](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/generate_train_result)

# Dependencies

1. [Qt5](http://torch.ch/)
2. [opencv3.3](https://opencv.org/), you should clone the source codes from github and 
build the library by yourself, by the time I am developing, there are some layers in this 
project do not support by opencv3.3 yet.
3. [torch](http://torch.ch/)

# Portability

The model is trained by torch and able to load by the opencv dnn module, that means you can 
run the model on all of the major platforms(windows, mac, linux, android, ios).

# Results

<img src="https://4.bp.blogspot.com/-l8LxRg6n5c0/Wd2gYC7geaI/AAAAAAAABFE/JNew1x2akPsQkgy6oEgjWb96svAwdKkMwCLcBGAs/s1600/montage.jpg">

Results of my implementation is outperform the paper(my average loss 2.58, paper is 9.2), 
overall performance of my model is better than the paper more than 3 times(9.2/2.58 = 3.57).
What makes the performance improve so much?A few of reasons I could think of are

1. I change the network architectures from vgg like to squeezeNet1.1 like.
2. I do not apply any data augmentation, maybe blurring or occlusion cause the model harder to train.
3. The paper use data augmentation to generate 500000 data for training, but I use 500032 images from imagenet
as my data for training. I guess this potentially increase variety of the data, the end result is network 
become easier to train and more robust(but they may not work well for blur or occlusion).

# Trained model 

[Here](https://mega.nz/#!8oMW0bpT!IjkEj_2FfmwZ3svvySFrtRZc6mGTxoR4naMshQcDbP8), 7.2MByte only, suit for mobile apps

If you liked this project, please help others find it by clicking the star icon above. Thanks a lot!