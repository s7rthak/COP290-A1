#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "perspective.hpp"
#include <utility>
// #include <X11/Xlib.h>
// #define NUM_THREADS 12

using namespace cv;
using namespace std;
using namespace boost::program_options;

vector<float> queue_density;
vector<pair<int, float> > thread_queue_density;
vector<float> dynamic_density;

// This function is for smoothing out the noise in the b/w image and having a consistent result.
Mat process(Mat img, int th)
{
    Mat processing;
    threshold(img, processing, th, 255, THRESH_BINARY);                                    // Thresholding to obtain binary greyscale image.
    Mat closing, opening;                                                                  // Now, we do closing (random 'noise' or white spots removal through filter) and opening (removal of black spots inside white object).
    morphologyEx(processing, closing, MORPH_CLOSE, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1); // Morphology transformation
    morphologyEx(closing, opening, MORPH_OPEN, MORPH_ELLIPSE, Point(-1, -1), 2, 1, 1);
    Mat dilated;
    dilate(opening, dilated, Mat(), Point(-1, -1), 2, 1, 1); // Dilation is to extend the white spots to form edges for clarity.
    return dilated;
}
struct frame_info {
    // Mat transformed;
    Rect mysplit;
    Mat empty_road_transformed;
    vector<Point2f> src;
    vector<Point2f> dest;
    vector<int> crop_sz;
    VideoCapture cap1;
    vector<float> tqd;
    vector<float> th_time;
    int threadid;
};

void *find_density(void *frameinfo)
{   
    struct frame_info *info;
    info = (struct frame_info *) frameinfo ;
    int count =0;
    cout<<info->threadid<<"----000----"<<endl;
    // VideoCapture cap("trafficvideo.mp4");
    cout<<info->threadid<<"----1111111----"<<endl;
    while (1)
    {
        Mat frame;
        cout<<info->threadid<<"---22222222222-----"<<endl;
        (info->cap1) >> frame;
        cout<<info->threadid<<"---3333-----"<<endl;
        if(count == 128) break;
        if (frame.empty())
            break;
        // imshow(format("orig-%d",info->threadid),frame);
        Mat transformed = perspective_transform(info->src, info->dest, info->crop_sz, frame);
        Mat static_diff;
        cout<<info->threadid<<"----+++++++++++----"<<endl;
        
        // Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        // Mat static_diff;
        absdiff(info->empty_road_transformed, transformed(info->mysplit), static_diff); // Difference between images

        Mat static_diff_bw;
        cvtColor(static_diff, static_diff_bw, COLOR_BGR2GRAY); // Convert to b/w
        Mat dilated = process(static_diff_bw, 35);

        // ch = true;
        float q_density = countNonZero(dilated) * 1.0 / (dilated.rows * dilated.cols); // Measure density
        info->tqd.push_back(q_density);
        // cout <<"thread qd = "<<thread_queue_density[count].first <<" -- "<<thread_queue_density[count].second<<endl;
        count++;
        info->th_time.push_back(count * 1.0 / 15);

        char c = (char)waitKey(25);
        if (c == 27)
            break;
    }
    // thread_queue_density.push_back(make_pair(info->threadid,q_density));
    
    pthread_exit(NULL);
}




int main(int argc, char *argv[])
{
    // XInitThreads();
    string vid, emp;
    try
    {

        options_description desc("Allowed options"); // Wrapper to store various arguments
        desc.add_options()                           // Add in the needed/optional arguments
            ("help,h", "No arguments needed! Just make sure you have the \'trafficvideo.mp4\' and \'empty.jpg\' in the same directory before running this program.")("video,v", value<string>(&vid)->default_value("trafficvideo.mp4"), "The video file containing footage of traffic at an intersection.")("empty,e", value<string>(&emp)->default_value("empty.jpg"), "The empty image not containing any vehicle that acts as background.");

        variables_map vm{};                              // Map from key to values for arguments
        store(parse_command_line(argc, argv, desc), vm); // Parse the command line arguments
        notify(vm);

        if (vm.count("help"))
        {
            cout << desc << "\n";
            return 1;
        }
        vid = vm["video"].as<string>();
        emp = vm["empty"].as<string>();
        if (!(boost::filesystem::exists(vid) && boost::filesystem::exists(emp)))
        {
            cout << "ya";
            throw invalid_argument("Argument file not found. Use -h option");
        }
    }

    catch (boost::program_options::error &e)
    {
        cout << "Invalid command! Please use the -h option to see available arguments\n";
        return 0;
    }
    int NUM_THREADS = stoi(argv[1]);
    string traffic_video = vid;
    // VideoCapture cap(traffic_video);
    VideoCapture capp[NUM_THREADS];
    for(int i = 0; i <NUM_THREADS;i++){
        VideoCapture cap(traffic_video);
        capp[i] = cap;
        cout <<cap<<endl;
    }
    // VideoCapture cap(traffic_video); // Opening the video file.
    Mat fr1;
    capp[0] >>fr1;
    imshow("cap1",fr1);
    waitKey(0);
    std::vector<cv::Point2f> src;
    src.push_back(Point2f(974, 217));
    src.push_back(Point2f(378, 973));
    src.push_back(Point2f(1523, 971));
    src.push_back(Point2f(1272, 209));

    vector<Point2f> dest;
    dest.push_back(Point2f(472, 52));
    dest.push_back(Point2f(472, 830));
    dest.push_back(Point2f(800, 830));
    dest.push_back(Point2f(800, 52));

    vector<int> crop_sz;
    crop_sz.push_back(372);
    crop_sz.push_back(52);
    crop_sz.push_back(458);
    crop_sz.push_back(808);

    vector<int> focus_sz;
    focus_sz.push_back(486);
    focus_sz.push_back(254);
    crop_sz.push_back(344);
    crop_sz.push_back(606);

    Mat prev;
    bool ch = false;

    string empty_rd = emp;
    Mat empty_road = imread(empty_rd); // Empty road matrix
    Mat empty_road_transformed = perspective_transform(src, dest, crop_sz, empty_road);

    vector<float> time;

    int count = 0;
    Ptr<BackgroundSubtractor> psubtr;
    psubtr = createBackgroundSubtractorMOG2(); // Background subtraction using the a pre-wriiten algorithm in opencv.
    // Mat transformed = perspective_transform(src, dest, crop_sz, frame);
    int width = empty_road_transformed.cols;
    int height = empty_road_transformed.rows;
    float gridwidth = width*1.0/sqrtf(NUM_THREADS);
    float gridheight = height*1.0 /sqrtf(NUM_THREADS);
    float stripheight = height*1.0/NUM_THREADS;
    vector<Rect> splits;
    pthread_t threads[NUM_THREADS];
    struct frame_info split[NUM_THREADS];
    vector<float> thread_time[NUM_THREADS];
    vector<float> thread_density[NUM_THREADS];
    int tid = 0 ;
    float q_d =0.0;
    // cout<<width<<" "<<height<<" "<<gridwidth<<" "<<gridheight<<endl;
    for(int y =0;y < height  ;y+=gridheight ){
        for(int x =0; x < width;x+=gridwidth){
            // int k = x*y + x;
            cout<<width<<" "<<height<<" "<<gridwidth<<" "<<gridheight<<" "<<x<<" "<<y<<endl;
            Rect grid_rect (x,y,gridwidth,gridheight);
            // cout << grid_rect<<endl;
            splits.push_back(grid_rect);
            // rectangle(frame,grid_rect,Scalar(0,255,0),1);
            // imshow("frame",frame);
            // imshow(format("grid-%d , %d",x, y),transformed(grid_rect));
            // waitKey(0);
        }
    }
    cout<<"fffffffffffff"<<endl;
    for(tid = 0;tid<NUM_THREADS;tid++){
        // imshow(format("thread_frame-%d",tid),frame(splits[tid]));
        // waitKey(0);
        // split[tid].transformed = transformed(splits[tid]);
        split[tid].empty_road_transformed = empty_road_transformed(splits[tid]);
        split[tid].threadid = tid;
        split[tid].cap1 = capp[tid];
        split[tid].tqd = thread_density[tid];
        split[tid].th_time = thread_time[tid];
        split[tid].mysplit = splits[tid];
        split[tid].src = src;
        split[tid].dest = dest;
        split[tid].crop_sz = crop_sz;
        int tc = pthread_create(&threads[tid], NULL,find_density,(void *)&split[tid]);
        if(tc){
            cout<<"ERROR thread not created : "<<tid<<endl;
            exit(-1);
        }
        cout<<"thread created : "<<tid<<endl;
        // q_d +=split[tid].qd;
        // cout<<"q_________d = "<<q_d<<endl;
    }
    // pthread_exit(NULL);
    // capp.release();
    for(int i = 0; i <NUM_THREADS;i++){
        capp[i].release();
    }
    cv::destroyAllWindows(); // Destroy frames after use.
    cout << "Time, Queue-Density, Frame_num"<<endl;
    for(int k=0; k<NUM_THREADS;k++){
        for(int i =0;i<thread_density[k].size();i++){
                    cout << thread_time[k][i] << ", " << thread_density[k][i] << ", "<< round(thread_time[k][i] * 15) << endl;;
        }
    }
    // for(int count=0;count<thread_queue_density.size();count++){
    //     cout <<"thread qd = "<<thread_queue_density[count].first <<" -- "<<thread_queue_density[count].second<<endl;
    // }
    cout << "Time, Queue-Density, Frame_num"<<endl;
    // for (int i = 1; i < queue_density.size(); i++)
    // {
    //     cout << time[i] << ", " << queue_density[i] << ", " << dynamic_density[i - 1] << ", " << round(time[i] * 15) << "\n";
    // }

    return 0;
}