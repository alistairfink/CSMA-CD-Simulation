#include <vector>
#include <iostream>
#include "node.h"
#include "metrics.h"

using namespace std;

void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics);
bool packetsUnprocessed(std::vector<Node> &Nodes);
int nextNode(std::vector<Node> &Nodes);
int abs(int num);

int main() {
	std::vector<Node> Nodes;
	int nodeCount = 20;

	float propSpeed = 300000000.0*(2.0/3.0); // Meters/second
	float interNodeDistance = 10.0; // Meters
	float packetSize = 1500; // bits
	float transmissionSpeed = 1000000; // bits/second

	Metrics metrics = Metrics();
	for(int i = 0; i < nodeCount; i++) {
		Nodes.push_back(Node(0.2, 1000));
	} 

	while(packetsUnprocessed(Nodes)) {
		int nextToProcess = nextNode(Nodes);
		processPackets(nextToProcess, Nodes, propSpeed, interNodeDistance, packetSize, transmissionSpeed, metrics);
	}

	for(int i = 0; i < Nodes.size(); i++) {
		metrics.DroppedCount += Nodes[i].dropped;
	}

	return 1;
}

// Check if each node will sense line as busy or will collide.
void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics) {
	float tCurrPacket = Nodes[curr_node_index].packets.front();
	bool collide = false;
	metrics.TransmissionCount++;

	for(int i = 0; i < Nodes.size(); i++) {
		if(i == curr_node_index) {
			continue;
		}

		float tnodePacket = Nodes[i].packets.front();

		float distance = interNodeDistance*abs(curr_node_index-i);
		float tProp = distance/propSpeed;
		float tTransmission = packetSize/transmissionSpeed;

		if(tnodePacket < tCurrPacket + tProp) {
			collide = true;
			metrics.CollisionCount++;
			Nodes[i].ProcessCollision(packetSize, transmissionSpeed);
		} else if (tnodePacket > tCurrPacket + tProp && tnodePacket < tCurrPacket + tProp + tTransmission) {
			// Packet sensed line and it was busy.
		}
	}

	if(collide) {
		metrics.CollisionCount++;
		Nodes[curr_node_index].ProcessCollision(packetSize, transmissionSpeed);
	} else {
		metrics.SuccessCount++;
		// original node success
	}
}

int abs(int num) {
	if(num < 0) {
		return -num;
	}

	return num;
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
	for(int i = 0; Nodes.size(); i++) {
		if(Nodes[i].packets.front() < Nodes[lowest].packets.front()) {
			lowest = i;
		}
	}

	return lowest;
}