#include <opencv4/opencv2/opencv.hpp> 
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
    dilate(opening, dilated, Mat(), Point(-1, -1), 2, 1, 1);
    return dilated;
}

int features_num = 100;
float error_th = 0.01;

int main(){
    // cout<<"hi";
    string traffic_video = "../trafficvideo.mp4";
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

    string empty_rd = "../empty.jpg";
    Mat empty_road = imread(empty_rd);
    Mat empty_road_transformed = perspective_transform(src, dest, crop_sz, empty_road);

    vector<float> queue_density;
    vector<float> time;
    vector<float> dynamic_density;

    int count = 0;

    Mat old_frame, old_gray;
    vector<Point2f> p0, p1;
    // Take first frame and find corners in it
    cap >> old_frame;
    Mat tr = perspective_transform(src, dest, crop_sz, old_frame);
    cvtColor(tr, old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(old_gray, p0, features_num, 0.3, 7, Mat(), 7, false, 0.04);
    // Create a mask image for drawing purposes
    Mat mask = Mat::zeros(tr.size(), tr.type());

    // Mat frame1, prvs;
    // cap >> frame1;
    // cvtColor(tr, prvs, COLOR_BGR2GRAY);
    while(true){
        Mat frame, frame_gray;
        cap >> frame;
        if (frame.empty())
            break;
        Mat temp = perspective_transform(src, dest, crop_sz, frame);
        cvtColor(temp, frame_gray, COLOR_BGR2GRAY);
        // calculate optical flow
        vector<uchar> status;
        vector<float> err;
        TermCriteria criteria = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err, Size(15,15), 2, criteria);
        vector<Point2f> good_new;

        int num_good_points = 0, num_moving_points = 0;
        for(uint i = 0; i < p0.size(); i++){
            if(status[i] == 1) {
                num_good_points++;
                good_new.push_back(p0[i]);
            }
        }
        for(uint i =0; i < p0.size(); i++){
            if(status[i] == 1) {
                float diff_motion = sqrt((p0[i].x - p1[i].x) * (p0[i].x - p1[i].x) + (p0[i].y - p1[i].y) * (p0[i].y - p1[i].y));
                // cout<<diff_motion<<"\n";
                if (diff_motion >= error_th) num_moving_points++;
                // cout<<err[i]<<"\n";
            }
        }
        // Mat img;
        // add(temp, mask, img);
        // imshow("Frame", img);
        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;
        // Now update the previous frame and previous points
        old_gray = frame_gray.clone();

        if(num_good_points <= features_num/2){
            vector<Point2f> find_new;
            goodFeaturesToTrack(old_gray, find_new, features_num, 0.3, 7, Mat(), 7, false, 0.04);
            p0 = find_new;
        }else{
            p0 = good_new;
        }

        count++;
        time.push_back(count*1.0/15);
        dynamic_density.push_back((num_moving_points * 1.0/num_good_points) * 0.8);
    }
    cap.release();
    cv::destroyAllWindows();

    cout<<"Time, Queue-Density\n";
    for(int i=1;i<dynamic_density.size();i++){
        cout<<time[i]<<", "<<dynamic_density[i-1]<<"\n";
    }
}