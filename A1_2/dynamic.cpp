#include <opencv2/opencv.hpp> 
#include <iostream>
#include <boost/program_options.hpp>
#include "perspective.hpp"

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
    dilate(opening, dilated, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
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


    Mat frame1, prvs;
    cap >> frame1;
    Mat tr = perspective_transform(src, dest, crop_sz, frame1);
    cvtColor(tr, prvs, COLOR_BGR2GRAY);
    while(true){
        Mat frame2, next;
        cap >> frame2;
        if (frame2.empty())
            break;
        Mat transformed = perspective_transform(src, dest, crop_sz, frame2);
        cvtColor(transformed, next, COLOR_BGR2GRAY);
        Mat flow(prvs.size(), CV_32FC2);
        calcOpticalFlowFarneback(prvs, next, flow, 0.5, 3, 15, 3, 5, 1.2, 0);
        // visualization
        Mat flow_parts[2];
        split(flow, flow_parts);
        Mat magnitude, angle, magn_norm;
        cartToPolar(flow_parts[0], flow_parts[1], magnitude, angle, true);
        normalize(magnitude, magn_norm, 0.0f, 1.0f, NORM_MINMAX);
        angle *= ((1.f / 360.f) * (180.f / 255.f));
        //build hsv image
        Mat _hsv[3], hsv, hsv8, bgr;
        _hsv[0] = angle;
        _hsv[1] = Mat::ones(angle.size(), CV_32F);
        _hsv[2] = magn_norm;
        merge(_hsv, 3, hsv);
        hsv.convertTo(hsv8, CV_8U, 255.0);
        cvtColor(hsv8, bgr, COLOR_HSV2BGR);
        Mat black_white;
        cvtColor(bgr, black_white, COLOR_BGR2GRAY);
        black_white = process(black_white, 10);
        float d_density = countNonZero(black_white)*1.0/(black_white.rows*black_white.cols);
        dynamic_density.push_back(d_density);
        imshow("frame2", black_white);
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        prvs = next;
        count++;
        time.push_back(count*1.0/15);
    }
    cap.release();

    // Closes all the frames
    cv::destroyAllWindows();

    cout<<"Time, Queue-Density\n";
    for(int i=1;i<dynamic_density.size();i++){
        cout<<time[i]<<", "<<dynamic_density[i-1]<<"\n";
    }
}