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
void runSim(float time, float average, int nodeCount);
void runDefault();
void customSim();

int main() {
	// Seed random
    srand(time(0));

    // Prompt user to run default sim.
	cout << "Run Default Simulations? (y/n) ";
	char response;
	cin >> response;
	if(response == 'y') {
		runDefault();
	} else {
		customSim();
	}

	return 1;
}

void runDefault() {
	// Run simulation scenarios from lab manual and example scenarios from lab manual
	float time = 1000.0;
	vector<int> nodes5average = {20, 40, 60, 100};
	vector<int> nodes12average = {20, 40, 60, 100};

	for(int i = 0; i < nodes5average.size(); i++) {
		runSim(time, 5.0, nodes5average[i]);
	}

	for(int i = 0; i < nodes12average.size(); i++) {
		runSim(time, 12.0, nodes12average[i]);
	}

	vector<int> labQuestionAverages = {7, 10, 20};
	for(int i = 0; i < labQuestionAverages.size(); i++) {
		for(int j = 20; j <= 100; j += 20) {
			runSim(time, labQuestionAverages[i], j);
		}
	}
}

void customSim() {
	for(;;) {
		// Promp User for simulation parameters then run simulation
		float time, average;
		int nodeCount;
		cout << "Simulation Time (Seconds): ";
		cin >> time;
		cout << "Average Packets/Second: ";
		cin >> average;
		cout << "Number of Nodes: ";
		cin >> nodeCount;
		runSim(time, average, nodeCount);
	}
}

void runSim(float time, float average, int nodeCount) {
	// Ouput inputted time and average packets
	cout << "time: " << time << endl;
	cout << "Average Packets/s " << average << endl;

	std::vector<Node> Nodes;

	// Constants from lab manual.
	float propSpeed = 300000000.0*(2.0/3.0); // Meters/second
	float interNodeDistance = 10.0; // Meters
	float packetSize = 1500; // bits
	float transmissionSpeed = 1000000; // bits/second

	// Instantiating metrics object
	Metrics metrics = Metrics();

	// Instantiate nodeCount Nodes. Each will generate a dequeue of packets.
	cout << "Generating " << nodeCount << " Nodes" << endl;
	for(int i = 0; i < nodeCount; i++) {
		Node newNode = Node(average, time);
		metrics.TotalPacketCount += newNode.packets.size();
		Nodes.push_back(newNode);
	} 

	// Run Simulation. While there are nodes find the next node to process then process it.
	cout << "Running Simulation" << endl;
	while(packetsUnprocessed(Nodes)) {
		int nextToProcess = nextNode(Nodes);
		processPackets(nextToProcess, Nodes, propSpeed, interNodeDistance, packetSize, transmissionSpeed, metrics);
	}

	// Sum up dropped packets.
	for(int i = 0; i < Nodes.size(); i++) {
		metrics.DroppedCount += Nodes[i].dropped;
	}

	// Ouput metrics.
	cout << endl << "Results:" << endl;
	cout << "TransmissionCount: " << metrics.TransmissionCount << endl;
	cout << "CollisionCount: " << metrics.CollisionCount << endl;
	cout << "SuccessCount: " << metrics.SuccessCount << endl;
	cout << "DroppedCount: " << metrics.DroppedCount << endl;
	cout << "Total Packets: " << metrics.TotalPacketCount << endl;
	cout << "Efficiency: " << float(metrics.SuccessCount) / metrics.TransmissionCount << endl;
	cout << "Throughput: " <<  (1500.0 * metrics.SuccessCount) / metrics.SimulationTime << " bits/s" << endl;
	cout << "------------------------------------------------" << endl << endl;
}

// Check if each node will sense line as busy or will collide.
void processPackets(int curr_node_index, std::vector<Node> &Nodes, float propSpeed, float interNodeDistance, float packetSize, float transmissionSpeed, Metrics &metrics) {
	// tCurrPacket is currently processing packet. For convenience. 
	float tCurrPacket = Nodes[curr_node_index].packets.front();

	// Set up collision stuff.
	bool collide = false;
	vector<int> collisionIndex;
	collisionIndex.push_back(curr_node_index);
	float collisionTime = -1.0; 

	// Calulate time for prop delay and time for transmission delay.
	float tProp = interNodeDistance / propSpeed;
	float tTrans = packetSize / transmissionSpeed;

	metrics.SimulationTime = tCurrPacket;

	for(int i = 0; i < Nodes.size(); i++) {
		if(i == curr_node_index || Nodes[i].packets.size() == 0) {
			continue;
		}

		// If Node will send a packet before the current packet + prop delay for that node then it will collide.
		if(Nodes[i].packets.front() < tCurrPacket + tProp*float(abs(i-curr_node_index))) {
			// Set collide to true and push this node's index to collisionindex.
			collide = true;
			collisionIndex.push_back(i);

			// Calculate intersection time between the 2 nodes. If this time is the lowest collision time then use that collision time.
			int left = min(i, curr_node_index);
			int right = max(i, curr_node_index);
			float tempCollisionTime = float(right - left) * interNodeDistance / (2*propSpeed);
			if(collisionTime == -1 || collisionTime > tempCollisionTime) {
				collisionTime = tempCollisionTime;
			}
		}
	}

	if(collide) {
		// If node collided with one or more other nodes then lop through all the colliding nodes and do exponential backoff for each node with the lowest collision time.
		for(int i = 0; i < collisionIndex.size(); i++) {
			metrics.TransmissionCount++;
			metrics.CollisionCount++;
			Nodes[collisionIndex[i]].ProcessCollision(collisionTime, transmissionSpeed);
		}
	} else {
		// If nothing collided then the transmission was successful.
		Nodes[curr_node_index].ProcessSuccess();
		metrics.TransmissionCount++;
		metrics.SuccessCount++;

		// Loop through all the nodes that do sense the current node and make them do exponential backoff before they can sense again.
		for(int i = 0; i < Nodes.size(); i++) {
			if(i == curr_node_index || Nodes[i].packets.size() == 0) {
				continue;
			}

			if(Nodes[i].packets.front() < tCurrPacket + tProp*float(abs(i-curr_node_index)) + tTrans) {
				Nodes[i].ProcessLineBusy_Persistent(tCurrPacket + tProp*float(abs(i-curr_node_index)) + tTrans);
			}
		}

	}
}

int abs(int num) {
	// Return absolute of number
	if(num < 0) {
		return -num;
	}

	return num;
}

int max(int num1, int num2) {
	// Return maximum out of num1 and num2
	if(num1 < num2) {
		return num2;
	}

	return num1;
}

int min(int num1, int num2) {
	// Return mininum out of num1 and num2
	if(num1 < num2) {
		return num1;
	}

	return num2;
}

bool packetsUnprocessed(std::vector<Node> &Nodes) {
	// Check if any nodes still have packets left in deque
	for(int i = 0; i < Nodes.size(); i++) {
		if(Nodes[i].packets.size() > 0) {
			return true;
		} 
	}

	return false;
}

int nextNode(std::vector<Node> &Nodes) {
	// Find node with packets that has the next packet as the closest to current time.
	int lowest = 0;
	for(int i = 0; i < Nodes.size(); i++) {
		if((Nodes[lowest].packets.size() == 0 && Nodes[i].packets.size() > 0) || 
			(Nodes[i].packets.size() > 0 && Nodes[i].packets.front() < Nodes[lowest].packets.front())) {
			lowest = i;
		}
	}

	return lowest;
}