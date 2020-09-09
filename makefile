all: main.cpp sim_mem.h ‫‪sim_mem.cpp
	g++ main.cpp ‫‪sim_mem.cpp -o ex4
all-GDB: main.cpp
	g++ -g main.cpp ‫‪sim_mem.cpp -o ex4
