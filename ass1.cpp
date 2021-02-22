#include <opencv4/opencv2/opencv.hpp> 
#include <iostream>
#include <boost/program_options.hpp>
// #include <stdafx.h>

using namespace cv;
using namespace std;
using namespace boost::program_options;

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

	string im{}; // variable to store image path
 
	options_description desc("Allowed options"); // Wrapper to store various arguments
	desc.add_options() // Add in the needed/optional arguments
		("help,h", "produce help message")
		("imag,i", value<string>(&im)->default_value("empty.jpg"), "Path to image");
 
	variables_map vm{}; // Map from key to values for arguments
	store(parse_command_line(argc, argv, desc), vm); // Parse the command line arguments
	notify(vm);
 
	if (vm.size() == 0 || vm.count("help"))
	{
		cout << desc << "\n";
		return 1;
	}
 
	if(vm.count("string")) cout << "String value was set to \"" << im << "\"" << endl;
    

    Mat img = imread(vm["imag"].as<string>()); // Reading the image given as argument
    if ( img.empty() ) // Handling errors when reading the image
    { 
          cout << "Error loading the image" << endl;
          return -1; 
    }
    
    vector<Point2f> corner_dest; // Destination corner upon applying homography (here it is fixed to some coordinates).
    corner_dest.push_back( Point2f(472,52) ) ;
    corner_dest.push_back( Point2f(472,830) ) ;
    corner_dest.push_back( Point2f(800,830) ) ;
    corner_dest.push_back( Point2f(800,52) ) ;
    Image image;
    image.img = img;
    namedWindow("Choose point",1);
    imshow("Choose point",image.img);
    setMouseCallback("Choose point",MouseInput,&image); // Taking the coordinates of the image through mouseclicks for transformation to destination
    waitKey(0);
    cout<< image.corners.size()<<endl;
    Mat H = findHomography(image.corners,corner_dest); // Finding the matrix H given the input and destination

    cout << "H:\n" << H << endl;
    Mat warp_img;
    warpPerspective(img,warp_img,H,img.size()); // Computing the perspective
    imshow("warpped",warp_img);
    imwrite("warpped.jpg",warp_img);
    waitKey(0);
    Rect crop(472,52,328,778);
    Mat cropped_img = warp_img(crop); // Cropping the image 
    imshow("crop",cropped_img);
    imwrite("crop.jpg",cropped_img);
    // Mat H = findHomography()
    waitKey(0);

    return 0;
}