#include <opencv4/opencv2/opencv.hpp> 
#include <iostream>
#include <boost/program_options.hpp>
#include "perspective.hpp"

using namespace cv;
using namespace std;
using namespace boost::program_options;

// This function is for smoothing out the noise in the b/w image and having a consistent result.
Mat process(Mat img, int th){
    Mat processing;
    threshold(img, processing, th, 255, THRESH_BINARY); // Thresholding to obtain binary greyscale image.
    Mat closing, opening; // Now, we do closing (random 'noise' or white spots removal through filter) and opening (removal of black spots inside white object).
    morphologyEx(processing, closing, MORPH_CLOSE, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1); // Morphology transformation 
    morphologyEx(closing, opening, MORPH_OPEN, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1); 
    Mat dilated;
    dilate(opening, dilated, Mat(), Point(-1, -1), 2, 1, 1); // Dilation is to extend the white spots to form edges for clarity.
    return dilated;
}

int main(int argc, char* argv[]){

    try
    {
    
        options_description desc("Allowed options"); // Wrapper to store various arguments
        desc.add_options() // Add in the needed/optional arguments
            ("help,h", "No arguments needed! Just make sure you have the \'trafficvideo.mp4\' and \'empty.jpg\' in the same directory before running this program.");
    
        variables_map vm{}; // Map from key to values for arguments
        store(parse_command_line(argc, argv, desc), vm); // Parse the command line arguments
        notify(vm);
    
        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 1;
        }
    }

    catch(boost::program_options::error& e)
    {
        cout << "Invalid command! Please use the -h option to see available arguments\n";
        return 0;
    }

    string traffic_video = "trafficvideo.mp4"; 
    VideoCapture cap(traffic_video); // Opening the video file.

    vector<Point2f> src; 
    src.push_back( Point2f(974, 217) ); src.push_back( Point2f(378, 973) ); src.push_back( Point2f(1523, 971) ); src.push_back( Point2f(1272, 209) );

    vector<Point2f> dest; 
    dest.push_back( Point2f(472, 52) ); dest.push_back( Point2f(472, 830) ); dest.push_back( Point2f(800, 830) ); dest.push_back( Point2f(800,52) );

    vector<int> crop_sz;
    crop_sz.push_back(372); crop_sz.push_back(52); crop_sz.push_back(458); crop_sz.push_back(808);

    vector<int> focus_sz;
    focus_sz.push_back(486); focus_sz.push_back(254); crop_sz.push_back(344); crop_sz.push_back(606);

    Mat prev;
    bool ch = false;

    string empty_rd = "empty.jpg";
    Mat empty_road = imread(empty_rd); // Empty road matrix
    Mat empty_road_transformed = perspective_transform(src, dest, crop_sz, empty_road);

    vector<float> queue_density;
    vector<float> time;
    vector<float> dynamic_density;

    int count = 0;
    Ptr<BackgroundSubtractor> psubtr;
    psubtr= createBackgroundSubtractorMOG2(); // Background subtraction using the a pre-wriiten algorithm in opencv.
    while(1){

        Mat frame;
        cap >> frame;
    
        if (frame.empty())
        break;

        Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        Mat static_diff;
        absdiff(empty_road_transformed, transformed, static_diff); // Difference between images

        Mat static_diff_bw;
        cvtColor(static_diff, static_diff_bw, COLOR_BGR2GRAY); // Convert to b/w
        Mat dilated = process(static_diff_bw, 35);
        

        if(ch){
            Mat dynamic_diff;
            Mat dynamic_diff_bw;

            psubtr->apply(transformed, dynamic_diff,-0.4);
            Mat dynamic_dilated = process(dynamic_diff, 5);

            float d_density = countNonZero(dynamic_dilated)*1.0/(dynamic_dilated.rows*dynamic_dilated.cols);
            dynamic_density.push_back(d_density);
        }

        ch = true;
        float q_density = countNonZero(dilated)*1.0/(dilated.rows*dilated.cols); // Measure density
        queue_density.push_back(q_density);

        count++;
        time.push_back(count*1.0/15);

        char c=(char)waitKey(25);
        if(c==27)
        break;
    }
 
    cap.release();
    cv::destroyAllWindows(); // Destroy frames after use.

    cout<<"Time, Queue-Density, Frame_num\n";
    for(int i=1;i<queue_density.size();i++){
        cout<<time[i]<<", "<<queue_density[i]<<", "<<dynamic_density[i-1]<<", "<<round(time[i]*15)<<"\n";
    }
        
    return 0;
}