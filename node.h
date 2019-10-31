#ifndef NODE_H
#define NODE_H

#include <vector>

class Node
{
	private:
		float lambda;
		float simulation_time;
		int backoff_counter;

		std::vector<float> Generate(float lambda, float total_time);
		float x_func(float u, float lamda);
		void ResetQueue();
		void Backoff();
	public:
		std::vector<float> packets;
		int dropped;
		
		Node();
		Node(float lambda, float total_time);
		void ProcessCollision();
		void ProcessSuccess();
}; 

#endif