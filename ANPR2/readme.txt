An exercise of ANPR which try to recognize the license plates from 
http://www.zemris.fer.hr/projects/LicensePlates/hrvatski/rezultati.shtml
Which collect the license plate of Croatia

Assumptions of this project

1 : The width must bigger than height, the ratio is around 3.0
2 : The license plates text is always darker than the license plate background
3 : The shape of the license plates is close to rectangle
4 : This algorithm is design for the standard license plate of Croatia, do not
work for non-standard license plate
5 : This algorithm only detect license plate with 6 characters


Things to imrpove
1 : Localize license with better accuracy
2 : Filter out illegal characters with better accuracy
3 : Improve OCR engine accuracy, but this will need a lot of examples

Conclusion

Not a perfect or near perfect Croatia ANPR system, but it already show you
the steps of building an ANRP system.