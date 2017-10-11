# Introduction

This command line program generate train results, draw ground truth region and predict region on the images

# Available options

1. folder--Folder of the images
2. model--Location of model
3. save_at--Where to save generated results

# How to use it

1. Generate data by [data_generator](https://github.com/stereomatchingkiss/blogCodes2/tree/master/deep_homography/data_generator)

## Example

```
data_generator --folder ../ms_coco_test_2017/ --max_size 12 --output ms_coco_test_set --save_warped_image true
```

2. Execute generate_train_result to generate the images

## Example 

```
generate_train_result --folder ./ms_coco_test_set --model ../squeeze_homo --save_at ../ms_coco_test_set
```

# Dependencies

1. [Qt5](http://torch.ch/)
2. [opencv3.3](https://opencv.org/)
