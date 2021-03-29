#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "perspective.hpp"
#include <utility>
#include <vector>
// #include <X11/Xlib.h>
// #define NUM_THREADS 12

using namespace cv;
using namespace std;
using namespace boost::program_options;

vector<float> queue_density;
vector<pair<int, float> > thread_queue_density;
vector<float> dynamic_density;
vector<Mat> frames; 
pthread_mutex_t lok;

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
bool isEqual(const cv::Mat Mat1, const cv::Mat Mat2)
{
  if( Mat1.dims == Mat2.dims && 
    Mat1.size == Mat2.size && 
    Mat1.elemSize() == Mat2.elemSize())
  {
    if( Mat1.isContinuous() && Mat2.isContinuous())
    {
      return 0==memcmp( Mat1.ptr(), Mat2.ptr(), Mat1.total()*Mat1.elemSize());
    }
    else
    {
      const cv::Mat* arrays[] = {&Mat1, &Mat2, 0};
      uchar* ptrs[2];
      cv::NAryMatIterator it( arrays, ptrs, 2);
      for(unsigned int p = 0; p < it.nplanes; p++, ++it)
        if( 0!=memcmp( it.ptrs[0], it.ptrs[1], it.size*Mat1.elemSize()) )
          return false;

      return true;
    }
  }

  return false;
}
struct frame_info {
    Mat frame;
    Mat prev_frame;
    Rect mysplit;
    Mat empty_road_transformed;
    vector<Point2f> src;
    vector<Point2f> dest;
    vector<int> crop_sz;
    // VideoCapture cap1;
    vector<float> tqd;
    vector<float> th_time;
    int threadid;
    int numthread;
};

void *find_density(void *frameinfo)
{   
    struct frame_info *info;
    info = (struct frame_info *) frameinfo ;
    int count = info->threadid * frames.size()*1.0/info->numthread*1.0;
    int last = (info->threadid+1) * frames.size()*1.0/info->numthread*1.0;
    cout<<info->threadid<<"----000----"<<endl;
    while (count!=last || count < last)
    {
        Mat frame;
        pthread_mutex_lock(&lok);
        frame = frames[count];
        pthread_mutex_unlock(&lok);
        if(frame.empty())
            {cout<<"breaked"<<endl;
            break;}
        // imshow(format("orig-%d",info->threadid),frame);
        Mat transformed = perspective_transform(info->src, info->dest, info->crop_sz, frame);
        Mat static_diff;
        // cout<<info->threadid<<"----+++++++++++----"<<endl;
        
        // Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        // Mat static_diff;
        absdiff(info->empty_road_transformed, transformed, static_diff); // Difference between images

        Mat static_diff_bw;
        cvtColor(static_diff, static_diff_bw, COLOR_BGR2GRAY); // Convert to b/w
        Mat dilated = process(static_diff_bw, 35);

        // ch = true;
        float q_density = countNonZero(dilated) * 1.0 / (dilated.rows * dilated.cols); // Measure density
        info->tqd.push_back(q_density);
        // cout<<"frame : "<<count<<info->threadid<<" td= "<<q_density<<endl;
        // cout <<"thread qd = "<<thread_queue_density[count].first <<" -- "<<thread_queue_density[count].second<<endl;
        count++;
        info->th_time.push_back(count * 1.0 / 15);
        info->prev_frame = info->frame;
        // cout<<info->threadid<<" "<<count<<" "<<last<<"\n";
    }
    cout << "thread finished : "<<info->threadid << endl;
    // thread_queue_density.push_back(make_pair(info->threadid,q_density));
    // return NULL;
    pthread_exit(0);
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
    int fps = 15.0/stol(argv[2]);
    string traffic_video = vid;
    VideoCapture cap(traffic_video);
    cout<<traffic_video<<endl;
    int index=0;
    while(1){
        Mat frame;
        bool f = cap.read(frame);
        
        // waitKey(0);
        if(f==false){
            cout<<"file end Breaked"<<endl;
            break;
        }
        // imwrite(format("\\frames\\frame%d.jpg",index),frame);
        // int index = cap.get(CV_CAP_POS_FRAMES);
        if(index%fps==0) {frames.push_back(frame);}
        index++;
    }
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
    struct frame_info split[NUM_THREADS];
    pthread_t threads[NUM_THREADS];
    vector<float> thread_time[NUM_THREADS];
    vector<float> thread_density[NUM_THREADS];
    cout<<"fffffffffffff"<<endl;
    // VideoCapture cap(traffic_video);
    if (pthread_mutex_init(&lok, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(int tid = 0;tid<NUM_THREADS;tid++){
        split[tid].empty_road_transformed = empty_road_transformed;
        split[tid].threadid = tid;
        
        // split[tid].cap1 = cap;
        split[tid].tqd = thread_density[tid];
        split[tid].th_time = thread_time[tid];
        // split[tid].mysplit = splits[tid];
        split[tid].src = src;
        split[tid].dest = dest;
        split[tid].crop_sz = crop_sz;
        split[tid].numthread = NUM_THREADS;
        int tc = pthread_create(&threads[tid], &attr,find_density,(void *)&split[tid]);
        if(tc){
            cout<<"ERROR thread not created : "<<tid<<endl;
            exit(-1);
        }
        cout<<"thread created : "<<tid<<endl;
    }
    // cout<<"here"<<endl;
    // while(1){
    //     Mat frame;
    //     cap >> frame;
    //     // vid.push_back(frame);
    //     // imshow("framw",frame);
    //     if(frame.empty()) {cout<<"break lol\n";break ;}

    //     for(int i = 0; i <NUM_THREADS;i++){
    //         if(isEqual(frame,split[i].prev_frame)) split[i].frame = frame;
    //     }
    // }
    // pthread_exit(NULL);
    // capp.release();
    pthread_attr_destroy(&attr);
    for(int i = 0; i <NUM_THREADS;i++){
        cout<<"Thread joined "<<i<<endl;
        pthread_join(threads[i],NULL);
    }
    pthread_mutex_destroy(&lok);
    // cv::destroyAllWindows(); // Destroy frames after use.
    // cout << "Time, Queue-Density, Frame_num"<<endl;
    // vector<float> tqd;
    // vector<float> th_time;
    // for(int k=0; k<NUM_THREADS;k++){
    //     for(int i =0;i<split[k].tqd.size();i++){
    //                 cout << split[k].th_time[i] << ", " << split[k].tqd[i] << ", "<< round(split[k].th_time[i] * 15) << endl;;
    //     }
    // }
    return 0;
}