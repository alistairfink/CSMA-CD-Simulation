#include <vector>
#include <iostream>
#include "node.h"

using namespace std;

void processPackets(int curr_node_index, std::vector  <Node> &Nodes);
bool packetsUnprocessed(std::vector<Node> &Nodes);
int nextNode(std::vector<Node> &Nodes);

int main() {
	std::vector<Node> Nodes;
	int nodeCount = 20;
	for(int i = 0; i < nodeCount; i++) {
		Nodes.push_back(Node(0.2, 1000));
	} 

	while(packetsUnprocessed(Nodes)) {
		int nextToProcess = nextNode(Nodes);
		processPackets(nextToProcess, Nodes);
	}

	return 1;
}

// Check if each node will sense line as busy or will collide.
void processPackets(int curr_node_index, std::vector<Node> &Nodes) {
	
	for(int i = 0; i < Nodes.size(); i++) {

	}
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
		if(Nodes[i].packets[0] < Nodes[lowest].packets[0]) {
			lowest = i;
		}
	}

	return lowest;
}