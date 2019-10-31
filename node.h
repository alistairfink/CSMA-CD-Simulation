#ifndef NODE_H
#define NODE_H

#include <deque>

class Node
{
	private:
		float lambda;
		float simulation_time;
		int backoff_counter;
		int nonPersistent_backoff_counter;

		std::deque<float> Generate(float lambda, float total_time);
		float x_func(float u, float lamda);
		void AddTime(float pushBackTime, float packetLength, float transmissionSpeed);
	public:
		std::deque<float> packets;
		int dropped;
		
		Node();
		Node(float lambda, float total_time);
		void ProcessCollision(float packetLength, float transmissionSpeed);
		void ProcessSuccess();
		void ProcessLineBusy_Persistent();
		void ProcessLineBusy_NonPersistent();
}; 

#endif