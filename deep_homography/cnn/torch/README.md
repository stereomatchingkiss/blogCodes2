# cnn

This is the core of the train and test of the cnn network.

# How to train

1. Generate train data by [data_generator](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/data_generator)
2. Train the network by [train.lua](https://github.com/stereomatchingkiss/blogCodes2/blob/master/deep_homography/cnn/torch/train.lua)

## Example 

```
th train.lua -folder ../../data/imagenet_train_gray_500032 -batch_size 64 -total_steps 90000 -save_as 'squeeze_homo' -loss_record_step 10000
```

# How to test

1. Generate train data by [data_generator](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/data_generator)
2. Train the network by [test.lua](https://github.com/stereomatchingkiss/blogCodes2/blob/master/deep_homography/cnn/torch/test.lua)

## Example

```
th test.lua -folder ../../data/ms_coco_train_gray_10000 -model ./squeeze_net_500032/squeeze_homo -batch_size 1
```

# Dependency

1. [torch](http://torch.ch/)