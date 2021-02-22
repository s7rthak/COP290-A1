all:
	g++ ass1.cpp -o ass1 -pthread -lboost_program_options -std=c++11 `pkg-config --cflags --libs opencv4`

run:
	./ass1

clean:
	rm -rf "*.o"; rm -rf "*.out"