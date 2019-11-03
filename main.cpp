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

int main() {	
    srand(time(0)); 

    float time = 1000;
	cout << endl << "time: " << time << endl;
	std::vector<Node> Nodes;
	int nodeCount = 100;

	float propSpeed = 300000000.0*(2.0/3.0); // Meters/second
	float interNodeDistance = 10.0; // Meters
	float packetSize = 1500; // bits
	float transmissionSpeed = 1000000; // bits/second

	// float time = 1000;
	float average = 12;
	cout << "average: " << average << endl;

	Metrics metrics = Metrics();

	cout << "Generating " << nodeCount << " Nodes" << endl;
	for(int i = 0; i < nodeCount; i++) {
		Node newNode = Node(average, time);
		metrics.TotalPacketCount += newNode.packets.size();
		Nodes.push_back(newNode);
	} 

	cout << "Running Simulation" << endl;
	while(packetsUnprocessed(Nodes)) {
		// for(int i = 0; i < nodeCount; i++) {
		// 	cout << i << ": " << Nodes[i].packets.size() <<","<<Nodes[i].packets.front().value << " ";
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

	return 1;
}

// Check if each node will sense line as busy or will collide.
void processPackets(
	int curr_node_index, 
	std::vector<Node> &Nodes, 
	float propSpeed, 
	float interNodeDistance, 
	float packetSize, 
	float transmissionSpeed, 
	Metrics &metrics) {

	metrics.TransmissionCount++;
	float tProp = interNodeDistance / propSpeed;
	float tTrans = packetSize / transmissionSpeed;
	
	float collisionTime = -1.0;

	// cout << curr_node_index << " " << Nodes[curr_node_index].packets.size() << endl;
	for(int i = 1; curr_node_index-i >= 0 || curr_node_index+i < Nodes.size(); i++) {
		if(curr_node_index-i >= 0 && Nodes[curr_node_index-i].packets.size() > 0) {
			if(Nodes[curr_node_index].packets.front().value + tProp*i > Nodes[curr_node_index-i].packets.front().value) {
				// cout << "  COLLIDE " << curr_node_index-i << " " << Nodes[curr_node_index-i].packets.size() << endl;
				metrics.CollisionCount++;
				metrics.TransmissionCount++;
				if(collisionTime == -1.0) {
					collisionTime = Nodes[curr_node_index-i].packets.front().value;
				}

				Nodes[curr_node_index-i].ProcessCollision(collisionTime, packetSize, transmissionSpeed);
			}
		}

		if(curr_node_index+i < Nodes.size() && Nodes[curr_node_index+i].packets.size() > 0) {
			if(Nodes[curr_node_index].packets.front().value + tProp*i > Nodes[curr_node_index+i].packets.front().value) {
				// cout << "  COLLIDE " << curr_node_index+i << " " << Nodes[curr_node_index+i].packets.size() << endl;
				metrics.CollisionCount++;
				metrics.TransmissionCount++;
				if(collisionTime == -1.0) {
					collisionTime = Nodes[curr_node_index+i].packets.front().value;
				}
				
				Nodes[curr_node_index+i].ProcessCollision(collisionTime, packetSize, transmissionSpeed);
			}
		}
	}

	if(collisionTime != -1.0) {
		metrics.CollisionCount++;
		Nodes[curr_node_index].ProcessCollision(collisionTime, packetSize, transmissionSpeed);
	} else {
		metrics.SuccessCount++;
		Nodes[curr_node_index].packets.pop_front();
		for(int i = 1; curr_node_index-i >= 0 || curr_node_index+i < Nodes.size(); i++) {
			if(curr_node_index-i >= 0 && Nodes[curr_node_index-i].packets.size() > 0) {
				if(Nodes[curr_node_index].packets.front().value + tProp*i <= Nodes[curr_node_index-i].packets.front().value && 
					Nodes[curr_node_index].packets.front().value + tProp*i + tTrans > Nodes[curr_node_index-i].packets.front().value) {
				// cout << "  SUCCESS " << curr_node_index-i << " " << Nodes[curr_node_index-i].packets.size() << endl;
					Nodes[curr_node_index-i].ProcessLineBusy_Persistent(Nodes[curr_node_index].packets.front().value + tProp*i + tTrans, packetSize, transmissionSpeed);
				}
			}

			if(curr_node_index+i < Nodes.size() && Nodes[curr_node_index+i].packets.size() > 0) {
				if(Nodes[curr_node_index].packets.front().value + tProp*i <= Nodes[curr_node_index+i].packets.front().value && 
					Nodes[curr_node_index].packets.front().value + tProp*i + tTrans > Nodes[curr_node_index+i].packets.front().value) {
				// cout << "  SUCCESS " << curr_node_index+i << " " << Nodes[curr_node_index+i].packets.size() << endl;
					Nodes[curr_node_index+i].ProcessLineBusy_Persistent(Nodes[curr_node_index].packets.front().value + tProp*i + tTrans, packetSize, transmissionSpeed);
				}
			}
		}
	}
		
}

// // Check if there are packets unprocesed.
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
			(Nodes[i].packets.size() > 0 && Nodes[i].packets.front().value < Nodes[lowest].packets.front().value)) {
			lowest = i;
		}
	}

	return lowest;
}




// // Check if each node will sense line as busy or will collide.
// void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics) {
// 	float tCurrPacket = Nodes[curr_node_index].packets.front();
// 	bool collide = false;
// 	metrics.TransmissionCount++;
// 	float collisionTime = 0.0; 

// 	std::vector<int> collideIndex;
// 	// cout << "Node: " << curr_node_index << " " << tCurrPacket << " " << Nodes[curr_node_index].packets.size() << endl;
// 	for(int i = 0; i < Nodes.size(); i++) {
// 		if(i == curr_node_index || Nodes[i].packets.size() == 0) {
// 			continue;
// 		}

// 		float tnodePacket = Nodes[i].packets.front();

// 		float distance = interNodeDistance*float(abs(curr_node_index-i));
// 		float tProp = distance/propSpeed;
// 		float tTransmission = packetSize/transmissionSpeed;

// 		// cout << "    Distance: " << distance << " tProP: " << tProp << " tTransmission: " << tTransmission << " Node: " << i << " " << tnodePacket << " " << Nodes[i].packets.size() << " ";
// 		if(tnodePacket < tCurrPacket + tProp) {
// 			// cout << "Collission: " << i << endl;
// 			collide = true;
// 			metrics.TransmissionCount++;
// 			metrics.CollisionCount++;
// 			// collideIndex.push_back(i);
// 			// if(Nodes[i].packets.front() > collisionTime) {
// 			// 	collisionTime = Nodes[i].packets.front();
// 			// }
// 			Nodes[i].ProcessCollision(transmissionSpeed);
// 		} else if (tnodePacket >= tCurrPacket + tProp && tnodePacket < tCurrPacket + tProp + tTransmission) {
// 			// cout << "Sensed: " << i << endl;
// 			Nodes[i].ProcessLineBusy_Persistent(tCurrPacket + tProp + tTransmission);
// 		}
// 	}

// 	if(collide) {
// 		metrics.CollisionCount++;
// 		Nodes[curr_node_index].ProcessCollision(transmissionSpeed);

// 		// for(int i = 0; i < collideIndex.size(); i++) {
// 		// 	Nodes[i].ProcessCollision(collisionTime, transmissionSpeed);
// 		// }
// 	} else {
// 		metrics.SuccessCount++;
// 		Nodes[curr_node_index].ProcessSuccess();
// 	}
// }

// int abs(int num) {
// 	if(num < 0) {
// 		return -num;
// 	}

// 	return num;
// }

// // Check if there are packets unprocesed.
// bool packetsUnprocessed(std::vector<Node> &Nodes) {
// 	for(int i = 0; i < Nodes.size(); i++) {
// 		if(Nodes[i].packets.size() > 0) {
// 			return true;
// 		} 
// 	}

// 	return false;
// }

// int nextNode(std::vector<Node> &Nodes) {
// 	int lowest = 0;
// 	for(int i = 0; i < Nodes.size(); i++) {
// 		if((Nodes[lowest].packets.size() == 0 && Nodes[i].packets.size() > 0) || 
// 			(Nodes[i].packets.size() > 0 && Nodes[i].packets.front() < Nodes[lowest].packets.front())) {
// 			lowest = i;
// 		}
// 	}

// 	return lowest;
// }