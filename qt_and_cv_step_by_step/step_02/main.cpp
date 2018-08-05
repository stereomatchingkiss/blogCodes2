#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

//propose of namespace are
//1. Decrease the chance of name collison
//2. Help you organizes your codes into logical groups
//Without declaring using namespace std, everytime when you are using
//the classes, functions in the namespace, you have to call with the
//prefix "std::".
using namespace cv;
using namespace std;

/**
 * main function is the global, designated start function of c++.
 * @param argc Number of the parameters of command line
 * @param argv Content of the parameters of command line.
 * @return any integer within the range of int, meaning of the return value is
 * defined by the users
 */
int main(int argc, char *argv[])
{
    if(argc != 2){
        cout<<"Run this example by invoking it like this: "<<endl;
        cout<<"./step_02.exe lena.jpg"<<endl;
        cout<<endl;
        return -1;
    }

    //If you execute by Ctrl+R, argv[0] == "step_02.exe", argv[1] == lena.jpg
    cout<<argv[0]<<","<<argv[1]<<endl;

    //Open the image
    auto const img = imread(argv[1]);
    if(!img.empty()){
        imshow("img", img); //Show the image on screen
        waitKey(); //Do not exist the program until users press a key
    }else{
        cout<<"cannot open image:"<<argv[1]<<endl;

        return -1;
    }//*/

    return 0; //usually we return 0 if everything are normal
}
