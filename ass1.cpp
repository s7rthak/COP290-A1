#include <opencv2/opencv.hpp>
#include <iostream>
// #include <stdafx.h>

using namespace cv;
using namespace std;

struct Image
{
    Mat img;
    vector<Point2f> corners;
};


void MouseInput(int event,int x,int y,int flags,void* userdata){
    if(event != EVENT_LBUTTONDOWN){
        return;
    }
    cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
    Image *  image = (Image *)userdata;
    
    circle(image->img,Point(x,y),3,Scalar(255,0,0),3,LINE_4);
    imshow("Choose point",image->img);
    // namedWindow("Choose point",1);

    image->corners.push_back(Point2f(x,y));
    cout<< image->corners.size()<<endl;

    if(image->corners.size()==4){
        // waitKey(0);
        destroyWindow("Choose point");
        return;
        
    }
    
}


int main(int argc, char* argv[]){
    Mat img = imread(argv[1]);
    if ( img.empty() ) 
    { 
          cout << "Error loading the image" << endl;
          return -1; 
    }
    
    vector<Point2f> corner_dest;
    corner_dest.push_back( Point2f(472,52) ) ;
    corner_dest.push_back( Point2f(472,830) ) ;
    corner_dest.push_back( Point2f(800,830) ) ;
    corner_dest.push_back( Point2f(800,52) ) ;
    Image image;
    image.img = img;
    namedWindow("Choose point",1);
    imshow("Choose point",image.img);
    setMouseCallback("Choose point",MouseInput,&image);
    waitKey(0);
    cout<< image.corners.size()<<endl;
    Mat H = findHomography(image.corners,corner_dest);

    cout << "H:\n" << H << endl;
    Mat warp_img;
    warpPerspective(img,warp_img,H,img.size());
    imshow("warpped",warp_img);
    imwrite("warpped.jpg",warp_img);
    waitKey(0);
    Rect crop(472,52,328,778);
    Mat cropped_img = warp_img(crop);
    imshow("crop",cropped_img);
    imwrite("crop.jpg",cropped_img);
    // Mat H = findHomography()
    waitKey(0);

    return 0;
}