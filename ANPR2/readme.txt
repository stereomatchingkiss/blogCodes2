A practice of ANPR which try to recognize the license plates from 
http://www.zemris.fer.hr/projects/LicensePlates/hrvatski/rezultati.shtml
Which collect the license plate of Croatia

Assumptions of this project

1 : The width must bigger than height, the ratio is around 3.0
2 : The license plates text is always darker than the license plate background
3 : The shape of the license plates is close to rectangle
4 : This algorithm is design for the standard license plate of Croatia, do not
work for non-standard license plate

Limitations

1 : Cannot localize every license plate
2 : Localized license plate may not crop perfectly
3 : The accuracy of OCR are limited, I guess it is because the characters are lack of training sample
only 18 characters for each category