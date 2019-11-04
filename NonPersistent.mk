all:
	g++ -std=c++11 NonPersistent.cpp node.cpp metrics.cpp -o NonPersistent && ./NonPersistent | tee NonPersistent.txt