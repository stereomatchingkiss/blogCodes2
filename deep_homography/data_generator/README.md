# Data generator

This small command line program generate train and test data of this project.

# Available options

1. folder--Folder of the images, this program will search the images(png, jpg, jpeg, bmp) recursively.
2. max_size--Maximum size of generated data.Default value is 10000
3. output--The place of saving generated images and information
4. debug--True will print/show debug message/image, false will not. Default value is false
5. color--True will save generated image as color image(b,g,r), false will be gray image.Default value is false
6. save_warped_image--True will save the image after resize and warped and vice versa.Default value is false

# Example

```
data_generator --folder ../../imagenet/ --max_size 500064 --output train_set
```

# Dependencies

1. [Qt5](https://www.qt.io/)
2. [opencv3.3](https://opencv.org/)

# To do

1. Optimize speed by multi-thread