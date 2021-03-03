#include <opencv4/opencv2/opencv.hpp> 
#include <iostream>
#include <boost/program_options.hpp>
#include "perspective.hpp"

using namespace cv;
using namespace std;
using namespace boost::program_options;

int main(){
    string traffic_video = "trafficvideo.mp4";
    VideoCapture cap(traffic_video);

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
    Mat empty_road = imread(empty_rd);
    Mat empty_road_transformed = perspective_transform(src, dest, crop_sz, empty_road);
    // Mat empty_road_transformed = perspective_transform(src, dest, focus_sz, empty_road);

    vector<float> queue_density;
    vector<float> time;

    int count = 0;

    while(1){

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
    
        // If the frame is empty, break immediately
        if (frame.empty())
        break;



        Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        // Mat transformed = perspective_transform(src, dest, focus_sz, frame);
        Mat diff;
        absdiff(empty_road_transformed, transformed, diff);

        // if(ch) absdiff(prev, transformed, diff);
        // ch = true;
        Mat diff_bw;
        cvtColor(diff, diff_bw, COLOR_BGR2GRAY);
        Mat processing;
        threshold(diff_bw, processing, 35, 255, THRESH_BINARY);
        Mat closing, opening;
        morphologyEx(processing, closing, MORPH_CLOSE, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
        morphologyEx(closing, opening, MORPH_OPEN, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
        Mat dilated;
        dilate(opening, dilated, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);

        // Rect focus_region(114, 202, 344, 606);
        // Mat focus_img = dilated(focus_img);


        // float q_density = countNonZero(focus_img)*1.0/(focus_img.rows*focus_img.cols);
        // queue_density.push_back(q_density);
        float q_density = countNonZero(dilated)*1.0/(dilated.rows*dilated.cols);
        queue_density.push_back(q_density);

        count++;
        time.push_back(count*1.0/15);

        // Display the resulting frame
        // imshow( "Frame", dilated );

        // prev = transformed;

        // Press  ESC on keyboard to exit
        char c=(char)waitKey(25);
        if(c==27)
        break;
    }
 
    // When everything done, release the video capture object
    cap.release();

    // Closes all the frames
    cv::destroyAllWindows();

    cout<<"Time, Queue-Density\n";
    for(int i=0;i<queue_density.size();i++){
        cout<<time[i]<<", "<<queue_density[i]<<"\n";
    }
        
    return 0;
}