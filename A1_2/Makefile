all:
	g++ traffic.cpp -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`

test1:
	g++ dynamic.cpp -o test -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`
