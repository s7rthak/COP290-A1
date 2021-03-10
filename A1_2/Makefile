all: traffic.cpp gnuplot.cpp
	g++ traffic.cpp -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4` 
	./a.out>plot.txt 
	g++ gnuplot.cpp -o plot 
	./plot

compile: traffic.cpp
	g++ traffic.cpp -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`

test: dynamic.cpp
	g++ dynamic.cpp -o test -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`

plot: gnuplot.cpp
	g++ gnuplot.cpp -o plot
	./plot

clean:
	rm -rf "*.o"; rm -rf "*.out"
