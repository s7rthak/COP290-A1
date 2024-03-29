#include <opencv4/opencv2/opencv.hpp> 

using namespace cv;
using namespace std;

int main( int argc, char** argv)
{
    // Read source image.
    Mat im_src = imread("empty.jpg");
    // Four corners of the book in source image
    vector<Point2f> pts_src;
    pts_src.push_back(Point2f(141, 131));
    pts_src.push_back(Point2f(480, 159));
    pts_src.push_back(Point2f(493, 630));
    pts_src.push_back(Point2f(64, 601));


    // Read destination image.
    Mat im_dst = imread("empty.jpg");
    // Four corners of the book in destination image.
    vector<Point2f> pts_dst;
    pts_dst.push_back(Point2f(472, 52));
    pts_dst.push_back(Point2f(472, 830));
    pts_dst.push_back(Point2f(800, 830));
    pts_dst.push_back(Point2f(800, 52));

    // Calculate Homography
    Mat h = findHomography(pts_src, pts_dst);

    // Output image
    Mat im_out;
    // Warp source image to destination based on homography
    warpPerspective(im_src, im_out, h, im_dst.size());

    // Display images
    imshow("Source Image", im_src);
    imshow("Destination Image", im_dst);
    imshow("Warped Source Image", im_out);

    waitKey(0);
}