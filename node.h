#ifndef NODE_H
#define NODE_H

#include <deque>

class Packet
{
	public:
		float value;
		int backoff_counter;
		int nonPersistent_backoff_counter;

		Packet(float val);
};

class Node
{
	private:
		float lambda;
		float simulation_time;

		std::deque<Packet> Generate(float lambda, float total_time);
		float x_func(float u, float lamda);
		void AddTime(float pushBackTime, float packetSize, float transmissionSpeed);
	public:
		std::deque<Packet> packets;
		int dropped;
		
		Node();
		Node(float lambda, float total_time);
		void ProcessCollision(float collisionTime, float packetSize, float transmissionSpeed);
		// void ProcessSuccess();
		void ProcessLineBusy_Persistent(float newTime, float packetSize, float transmissionSpeed);
		// void ProcessLineBusy_NonPersistent();
// 	private:
// 		float lambda;
// 		float simulation_time;
// 		int backoff_counter;
// 		int nonPersistent_backoff_counter;

// 		std::deque<float> Generate(float lambda, float total_time);
// 		float x_func(float u, float lamda);
// 		void AddTime(float pushBackTime);
// 	public:
// 		std::deque<float> packets;
// 		int dropped;
		
// 		Node();
// 		Node(float lambda, float total_time);
// 		void ProcessCollision(float transmissionSpeed);
// 		void ProcessSuccess();
// 		void ProcessLineBusy_Persistent(float newTime);
// 		void ProcessLineBusy_NonPersistent();
};

#endif