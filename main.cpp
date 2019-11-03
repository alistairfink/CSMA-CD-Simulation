#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include "node.h"
#include "metrics.h"

using namespace std;

void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics);
bool packetsUnprocessed(std::vector<Node> &Nodes);
int nextNode(std::vector<Node> &Nodes);
int abs(int num);
int max(int num1, int num2);
int min(int num1, int num2);

int main() {
    srand(time(0)); 
	for(float time = 1000.0; time <= 1000.0; time+= 1000.0) {
		cout << endl << "time: " << time << endl;
		std::vector<Node> Nodes;
		int nodeCount = 100;

		float propSpeed = 300000000.0*(2.0/3.0); // Meters/second
		float interNodeDistance = 10.0; // Meters
		float packetSize = 1500; // bits
		float transmissionSpeed = 1000000; // bits/second

		// float time = 1000;
		float average = 12.0;
		Metrics metrics = Metrics();

		cout << "Generating " << nodeCount << " Nodes" << endl;
		for(int i = 0; i < nodeCount; i++) {
			Node newNode = Node(average, time);
			metrics.TotalPacketCount += newNode.packets.size();
			Nodes.push_back(newNode);
		} 

		cout << "Running Simulation" << endl;
		while(packetsUnprocessed(Nodes)) {
			// for(int i = 0; i < Nodes.size(); i++) {
			// 	cout << i << ": " << Nodes[i].packets.size() << " ";
			// }

			// cout << endl;
			int nextToProcess = nextNode(Nodes);
			processPackets(nextToProcess, Nodes, propSpeed, interNodeDistance, packetSize, transmissionSpeed, metrics);
		}

		for(int i = 0; i < Nodes.size(); i++) {
			metrics.DroppedCount += Nodes[i].dropped;
		}

		cout << endl << "Results:" << endl;
		cout << "TransmissionCount: " << metrics.TransmissionCount << endl;
		cout << "CollisionCount: " << metrics.CollisionCount << endl;
		cout << "SuccessCount: " << metrics.SuccessCount << endl;
		cout << "DroppedCount: " << metrics.DroppedCount << endl;
		cout << "Total Packets: " << metrics.TotalPacketCount << endl;
		cout << "Efficiency: " << float(metrics.SuccessCount) / metrics.TransmissionCount << endl;
	}
	return 1;
}

// Check if each node will sense line as busy or will collide.
void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics) {
	float tCurrPacket = Nodes[curr_node_index].packets.front();

	bool collide = false;
	vector<int> collisionIndex;
	collisionIndex.push_back(curr_node_index);
	float collisionTime = -1.0; 

	float tProp = interNodeDistance / propSpeed;
	float tTrans = packetSize / transmissionSpeed;

	for(int i = 0; i < Nodes.size(); i++) {
		if(i == curr_node_index || Nodes[i].packets.size() == 0) {
			continue;
		}

		if(Nodes[i].packets.front() < tCurrPacket + tProp*float(abs(i-curr_node_index))) {
			collide = true;
			collisionIndex.push_back(i);
			int left = min(i, curr_node_index);
			int right = max(i, curr_node_index);
			float tempCollisionTime = float(right - left) * interNodeDistance / (2*propSpeed);
			if(collisionTime == -1 || collisionTime > tempCollisionTime) {
				collisionTime = tempCollisionTime;
			}
		}
	}

	if(collide) {
		for(int i = 0; i < collisionIndex.size(); i++) {
			metrics.TransmissionCount++;
			metrics.CollisionCount++;
			Nodes[collisionIndex[i]].ProcessCollision(collisionTime, transmissionSpeed);
		}
	} else {
		Nodes[curr_node_index].ProcessSuccess();
		metrics.TransmissionCount++;
		metrics.SuccessCount++;
		for(int i = 0; i < Nodes.size(); i++) {
			if(i == curr_node_index || Nodes[i].packets.size() == 0) {
				continue;
			}

			if(Nodes[i].packets.front() < tCurrPacket + tProp*float(abs(i-curr_node_index)) + tTrans) {
				Nodes[i].ProcessLineBusy_Persistent(tCurrPacket + tProp*float(abs(i-curr_node_index)) + tTrans);
			}
		}

	}


	// cout << "Node: " << curr_node_index << " " << tCurrPacket << " " << Nodes[curr_node_index].packets.size() << endl;
	// for(int i = 0; i < Nodes.size(); i++) {
	// 	if(i == curr_node_index || Nodes[i].packets.size() == 0) {
	// 		continue;
	// 	}

	// 	float tnodePacket = Nodes[i].packets.front();

	// 	float distance = interNodeDistance*float(abs(curr_node_index-i));
	// 	float tProp = distance/propSpeed;
	// 	float tTransmission = packetSize/transmissionSpeed;

	// 	// cout << "    Distance: " << distance << " tProP: " << tProp << " tTransmission: " << tTransmission << " Node: " << i << " " << tnodePacket << " " << Nodes[i].packets.size() << " ";
	// 	if(tnodePacket < tCurrPacket + tProp) {
	// 		// cout << "Collission: " << i << endl;
	// 		collide = true;
	// 		metrics.TransmissionCount++;
	// 		metrics.CollisionCount++;
	// 		collisionIndex.push_back(i);
	// 		if(collissionTime == -1 || collissionTime < tnodePacket) {
	// 			collissionTime = tnodePacket;
	// 		}
	// 	}
	// }

	// if(collide) {
	// 	metrics.CollisionCount++;
	// 	Nodes[curr_node_index].ProcessCollision( collissionTime, transmissionSpeed);
	// 	for(int i = 0; i < collisionIndex.size(); i++) {
	// 		Nodes[collisionIndex[i]].ProcessCollision(collissionTime, transmissionSpeed);
	// 	}
	// } else {
	// 	metrics.SuccessCount++;
	// 	Nodes[curr_node_index].ProcessSuccess();
	// 	for(int i = 0; i < Nodes.size(); i++) {
	// 		if(i == curr_node_index || Nodes[i].packets.size() == 0) {
	// 			continue;
	// 		}

	// 		float tnodePacket = Nodes[i].packets.front();

	// 		float distance = interNodeDistance*float(abs(curr_node_index-i));
	// 		float tProp = distance/propSpeed;
	// 		float tTransmission = packetSize/transmissionSpeed;

	// 		// cout << "    Distance: " << distance << " tProP: " << tProp << " tTransmission: " << tTransmission << " Node: " << i << " " << tnodePacket << " " << Nodes[i].packets.size() << " ";
	// 		if (tnodePacket > tCurrPacket + tProp && tnodePacket < tCurrPacket + tProp + tTransmission) {
	// 			// cout << "Sensed: " << i << endl;
	// 			Nodes[i].ProcessLineBusy_Persistent(tCurrPacket + tProp + tTransmission);
	// 		}
	// 	}
	// }
}

int abs(int num) {
	if(num < 0) {
		return -num;
	}

	return num;
}

int max(int num1, int num2) {
	if(num1 < num2) {
		return num2;
	}

	return num1;
}

int min(int num1, int num2) {
	if(num1 < num2) {
		return num1;
	}

	return num2;
}

// Check if there are packets unprocesed.
bool packetsUnprocessed(std::vector<Node> &Nodes) {
	for(int i = 0; i < Nodes.size(); i++) {
		if(Nodes[i].packets.size() > 0) {
			return true;
		} 
	}

	return false;
}

int nextNode(std::vector<Node> &Nodes) {
	int lowest = 0;
	for(int i = 0; i < Nodes.size(); i++) {
		if((Nodes[lowest].packets.size() == 0 && Nodes[i].packets.size() > 0) || 
			(Nodes[i].packets.size() > 0 && Nodes[i].packets.front() < Nodes[lowest].packets.front())) {
			lowest = i;
		}
	}

	return lowest;
}