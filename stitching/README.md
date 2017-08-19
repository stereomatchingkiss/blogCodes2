# stitching

A cpp example demonstrate how to do image stitching by opencv3

I give two solutions a shot, one of them is based on the [pyimagesearch blog](http://www.pyimagesearch.com/2016/01/11/opencv-panorama-stitching/),
another one use the Stitcher class of opencv. 

#Images we want to stitch

<img src="https://s12.postimg.org/d7ybs83lp/PICT0107a.jpg" height="220px">
<img src="https://s12.postimg.org/l38v71d8d/PICT0108a.jpg" height="220px">
<img src="https://s12.postimg.org/ir0v9g0n1/green_nature1.jpg" width="220px">
<img src="https://s12.postimg.org/kk3rxrltp/green_nature2.jpg" width="220px">

# Results of PyimageSearch solution

<img src="https://s12.postimg.org/8sfwmyr7h/PICT0107a_stitch_resut.jpg" width="220px">
<img src="https://s12.postimg.org/3whbsdxdp/green_nature1_stitch_resut.jpg">

# Results of OpenCV solution

<img src="https://s12.postimg.org/8sfwmyr7h/PICT0107a_stitch_resut.jpg" width="220px">
<img src="https://s12.postimg.org/3whbsdxdp/green_nature1_stitch_resut.jpg">

Almost every images I test, OpenCV solution able to produce more appealing results than 
PyImageSearch solution, but it is also slower(around 2 times slower). Stitch class of OpenCV
have a lot of parameters to play with, I only scratch the surface of it in this small demo.
