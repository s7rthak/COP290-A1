all:
	g++ homography.cpp -o homography -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`

run:
	./homography

clean:
	rm -rf "*.o"; rm -rf "*.out"