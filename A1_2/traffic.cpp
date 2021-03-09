#include <opencv2/opencv.hpp> 
#include <iostream>
#include <boost/program_options.hpp>
#include "perspective.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;
using namespace boost::program_options;

Mat process(Mat img, int th){
    Mat processing;
    threshold(img, processing, th, 255, THRESH_BINARY);
    Mat closing, opening;
    morphologyEx(processing, closing, MORPH_CLOSE, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
    morphologyEx(closing, opening, MORPH_OPEN, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
    Mat dilated;
    dilate(opening, dilated, Mat(), Point(-1, -1), 2, 1, 1);
    return dilated;
}

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
    vector<float> dynamic_density;

    int count = 0;
    Ptr<BackgroundSubtractor> psubtr; 
    psubtr= createBackgroundSubtractorMOG2();
    while(1){

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;
    
        // If the frame is empty, break immediately
        if (frame.empty())
        break;



        Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        // Mat transformed = perspective_transform(src, dest, focus_sz<, frame);
        Mat static_diff;
        absdiff(empty_road_transformed, transformed, static_diff);

        Mat static_diff_bw;
        cvtColor(static_diff, static_diff_bw, COLOR_BGR2GRAY);
        Mat dilated = process(static_diff_bw, 35);
        

        if(ch){
            Mat dynamic_diff;
            Mat dynamic_diff_bw;
            // cvtColor(dynamic_diff, dynamic_diff_bw, COLOR_BGR2GRAY);
            // absdiff(prev, transformed, dynamic_diff);
            // cout<<prev.size();
            // cout<<transformed.size();
            // cout<<empty_road_transformed.size();

            psubtr->apply(transformed,dynamic_diff,-0.4);
            stringstream ss;
            ss << cap.get(CAP_PROP_POS_FRAMES);
            string frameNumberString = ss.str();
            putText(transformed, frameNumberString.c_str(), cv::Point(15, 15),
                FONT_HERSHEY_SIMPLEX, 0.5 , cv::Scalar(0,0,0));
            // dynamic_diff = prev;
            Mat dynamic_dilated = process(dynamic_diff, 5);
            // psubtr->apply(transformed,empty_road_transformed);

            float d_density = countNonZero(dynamic_dilated)*1.0/(dynamic_dilated.rows*dynamic_dilated.cols);
            dynamic_density.push_back(d_density);
            // imshow("trans", transformed);
            // imshow("prev", dynamic_dilated);
            // prev = transformed;

            // imshow("Frame", dynamic_diff);
        }
        ch = true;

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
    for(int i=1;i<queue_density.size();i++){
        cout<<time[i]<<", "<<queue_density[i]<<", "<<dynamic_density[i-1]<<"\n";
    }
        
    return 0;
}