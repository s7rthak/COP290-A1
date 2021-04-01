#include <opencv4/opencv2/opencv.hpp> 
#include <iostream>

using namespace cv;
using namespace std;

Mat perspective_transform(vector<Point2f> source, vector<Point2f> destination, vector<int> crop_sz, Mat m){
    Mat H = findHomography(source, destination);
    Mat warp_img;
    warpPerspective(m, warp_img, H, m.size());
    Rect crop(crop_sz[0], crop_sz[1], crop_sz[2], crop_sz[3]);
    return warp_img(crop);
}