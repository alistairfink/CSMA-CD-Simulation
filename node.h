#ifndef NODE_H
#define NODE_H

#include <vector>

class Node
{
	private:
		float lambda;
		float simulation_time;

		std::vector<float> Generate(float lambda, float total_time);
		float x_func(float u, float lamda);
	public:
		std::vector<float> packets;
		
		Node();
		Node(float lambda, float total_time);
}; 

#endif