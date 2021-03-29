m1: m1.cpp
	g++ m1.cpp -o m1 -pthread -lboost_program_options -lboost_filesystem -std=c++11 `pkg-config --cflags --libs opencv4`

m2: m2.cpp
	g++ m2.cpp -o m2 -pthread -lboost_program_options -lboost_filesystem -std=c++11 `pkg-config --cflags --libs opencv4`

m3: m3.cpp
	g++ m3.cpp -o m3 -Wall -pthread -lboost_program_options -lboost_filesystem -std=c++11 `pkg-config --cflags --libs opencv4`

m4: m4.cpp
	g++ m4.cpp -o m4 -pthread -lboost_program_options -lboost_filesystem -std=c++11 `pkg-config --cflags --libs opencv4`

plot1: gnuplot1.cpp
	g++ gnuplot1.cpp -o plot1
	./plot1

plot2: gnuplot2.cpp
	g++ gnuplot2.cpp -o plot2
	./plot2