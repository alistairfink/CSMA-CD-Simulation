#include <vector>
#include <iostream>
#include "node.h"

using namespace std;

int main() {
	std::vector<Node> Nodes;
	int nodeCount = 20;
	for(int i = 0; i < nodeCount; i++) {
		Nodes.push_back(Node(0.2, 1000));
	} 

	cout << Nodes.size() << endl;
	return 1;
}