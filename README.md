# COP290-A1
First assignment of COP290 involving homography for camera-angle correction

To run: ```g++ ass1.cpp -o ass1 -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4` ``` \
To install Boost: ``` sudo apt-get install libboost-all-dev```

For getting the static and dynamic queue density output, run ```make``` (You may have to check the Makefile for different version of opencv). \
Then run ```./a.out>plot.txt``` and finally run the gnuplot executable ```./gnuplot``` \
For running the dynamic density, compile the executable to form the ```test``` executable and then run ```./test>test.txt``` \
To get the plot, there is no cpp file so you have to run from shell. \

Change the header from opencv4 to opencv2 if opencv2 is installed.
