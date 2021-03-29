#include <opencv2/opencv.hpp>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "perspective.hpp"
#include <utility>
#include <vector>

using namespace cv;
using namespace std;
using namespace boost::program_options;
using namespace chrono;

vector<float> queue_density;
vector<pair<int, float>> thread_queue_density;
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

struct frame_info {
    Mat frame;
    Mat prev_frame;
    Rect mysplit;
    Mat empty_road_transformed;
    vector<Point2f> src;
    vector<Point2f> dest;
    vector<int> crop_sz;
    vector<float> tqd;
    vector<float> th_time;
    int threadid;
};

void *find_density(void *frameinfo)
{
    // pthread_detach(pthread_self());
    struct frame_info *info;
    info = (struct frame_info *) frameinfo ;
    int count = 0;
    cout<<info->threadid<<"----000----"<<endl;

    while (count != (int)frames.size())
    {
        Mat transformed;
        pthread_mutex_lock(&lok);
        transformed = frames[count];
        pthread_mutex_unlock(&lok);
        
        if(transformed.empty())
            {cout<<"breaked"<<endl;
            break;}
        // Mat transformed = perspective_transform(info->src, info->dest, info->crop_sz, frame);
        Mat static_diff;
        absdiff(info->empty_road_transformed, transformed(info->mysplit), static_diff); // Difference between images

        Mat static_diff_bw;
        cvtColor(static_diff, static_diff_bw, COLOR_BGR2GRAY); // Convert to b/w
        Mat dilated = process(static_diff_bw, 35);

        float q_density = countNonZero(dilated) * 1.0 / (dilated.rows * dilated.cols); // Measure density
        info->tqd.push_back(q_density);
        // cout << info->threadid << " -> " << count << " q = "<<q_density<<endl;
        count++;
        info->th_time.push_back(count * 1.0 / 15);
        info->prev_frame = info->frame;
    }
    cout << "thread finished : "<<info->threadid << endl;
    
    // pthread_exit(NULL);
    pthread_exit(0);
    // return NULL;
}


int main(int argc, char *argv[])
{
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
    // VideoCapture cap;
    // Mat fr1;
    // object capp[NUM_THREADS];
    // for(int i = 0; i <NUM_THREADS;i++){
    //     // VideoCapture cap(traffic_video);
    //     capp[i] = cap(traffic_video);
    //     capp[i] >> fr1;
    //     imshow(format("capp%d",i),fr1);
    // }
    // VideoCapture cap(traffic_video); // Opening the video file.
    // Mat fr1;
    // capp[0] >>fr1;
    // imshow("cap1",fr1);
    // waitKey(0);
    int fps = 15.0 / stol(argv[2]);
    VideoCapture cap(traffic_video);
    cout << traffic_video << endl;

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
    int index = 0;
    while (1)
    {
        Mat frame;
        bool f = cap.read(frame);

        // waitKey(0);
        if (f == false)
        {
            cout << "file end Breaked" << endl;
            break;
        }
        //imwrite(format("\\frames\\frame%d.jpg",index),frame);
        // int index = cap.get(CV_CAP_POS_FRAMES);
        Mat transformed = perspective_transform(src, dest, crop_sz, frame);
        if (index % fps == 0)
        {
            frames.push_back(transformed);
        }
        index++;
    }

    string empty_rd = emp;
    Mat empty_road = imread(empty_rd); // Empty road matrix
    Mat empty_road_transformed = perspective_transform(src, dest, crop_sz, empty_road);

    vector<float> time;

    auto start = high_resolution_clock::now();

    int count = 0;
    Ptr<BackgroundSubtractor> psubtr;
    psubtr = createBackgroundSubtractorMOG2(); // Background subtraction using the a pre-wriiten algorithm in opencv.
    int width = empty_road_transformed.cols;
    int height = empty_road_transformed.rows;
    cout << "width = " << width << " height = " << height << endl;
    vector<Rect> splits;
    pthread_t threads[NUM_THREADS];
    struct frame_info split[NUM_THREADS];
    vector<float> thread_time[NUM_THREADS];
    vector<float> thread_density[NUM_THREADS];
    int tid = 0;
    float q_d = 0.0;
    // cout<<width<<" "<<height<<" "<<gridwidth<<" "<<gridheight<<endl;
    if (!(NUM_THREADS == 1 || NUM_THREADS == 4 || NUM_THREADS == 9 || NUM_THREADS == 16))
    {
        float stripheight = height * 1.0 / NUM_THREADS;
        for (int h = 0; h <= height - stripheight; h += stripheight)
        {
            Rect strip_rect(0, h, width, stripheight);
            cout << strip_rect << endl;
            splits.push_back(strip_rect);
            // imshow(format("strip%d",h),frames[0](strip_rect));
            // waitKey(0);
        }
    }
    else
    {
        float gridwidth = width * 1.0 / sqrtf(NUM_THREADS);
        float gridheight = height * 1.0 / sqrtf(NUM_THREADS);
        for (int y = 0; y <= height - gridheight; y += gridheight)
        {
            for (int x = 0; x <= width - gridwidth; x += gridwidth)
            {
                // int k = x*y + x;
                // cout << width << " " << height << " " << gridwidth << " " << gridheight << " " << x << " " << y << endl;
                Rect grid_rect(x, y, gridwidth, gridheight);
                cout << grid_rect<<endl;
                splits.push_back(grid_rect);
            }
        }
    }
    if (pthread_mutex_init(&lok, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    for(int tid = 0;tid<NUM_THREADS;tid++){
        split[tid].empty_road_transformed = empty_road_transformed(splits[tid]);
        split[tid].threadid = tid;
        
        split[tid].tqd = thread_density[tid];
        split[tid].th_time = thread_time[tid];
        split[tid].mysplit = splits[tid];
        split[tid].src = src;
        split[tid].dest = dest;
        split[tid].crop_sz = crop_sz;
        int tc = pthread_create(&threads[tid], &attr,find_density,(void *)&split[tid]);
        if(tc){
            cout<<"ERROR thread not created : "<<tid<<endl;
            exit(-1);
        }
        cout<<"thread created : "<<tid<<endl;
    }
    pthread_attr_destroy(&attr);
    for(int i = 0; i <NUM_THREADS;i++){
        cout<<"Thread joined "<<i<<endl;
        pthread_join(threads[i],NULL);
    }
    pthread_mutex_destroy(&lok);

// pthread_exit(NULL);
// pthread_join(pthread_self(), NULL);
auto stop = high_resolution_clock::now();
auto duration = duration_cast<microseconds>(stop - start);
cv::destroyAllWindows(); // Destroy frames after use.
cout << "Time, Queue-Density, Frame_num" << endl;
cout << "Time taken (ms) :  " << duration.count() / 1000.0 << endl;
// vector<float> tqd;
// vector<float> th_time;
for (int i = 0; i < frames.size(); i++)
{
    float qd =0.0;
    for (int k = 0; k < NUM_THREADS; k++){
        qd+=split[k].tqd[i];
    }
    qd = qd/NUM_THREADS * 1.0;
    queue_density.push_back(qd);
    // cout << split[k].th_time[i] << ", " << split[k].tqd[i] << ", "<< round(split[k].th_time[i] * 15) << endl;;
}

for(int i=0;i<queue_density.size();i++)
{
    cout << " frame no = "<<i<<" qd = "<<queue_density[i]<<endl;
}
// for(int count=0;count<thread_queue_density.size();count++){
//     cout <<"thread qd = "<<thread_queue_density[count].first <<" -- "<<thread_queue_density[count].second<<endl;
// }
// for (int i = 1; i < queue_density.size(); i++)
// {
//     cout << time[i] << ", " << queue_density[i] << ", " << dynamic_density[i - 1] << ", " << round(time[i] * 15) << "\n";
// }

return 0;
}
