all:
	g++ -std=c++11 Persistent.cpp node.cpp metrics.cpp -o Persistent && ./Persistent | tee Persistent.txt