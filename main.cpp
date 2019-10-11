#include <vector>
#include <iostream>
#include "node.h"

using namespace std;

int main() {
	Node test = Node(0.2, 1000);
	for(size_t i = 0; i < test.packets.size(); ++i) {
		cout << test.packets[i] << endl;
	}

	Node testT = Node(0.2, 2000);
	for(size_t i = 0; i < testT.packets.size(); ++i) {
		cout << testT.packets[i] << endl;
	}

	return 1;
}