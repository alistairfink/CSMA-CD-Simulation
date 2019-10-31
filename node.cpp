#include <math.h>
#include <vector>
#include <stdlib.h>
#include <random>
#include <chrono>

#include "node.h"

using namespace std;

Node::Node() {
	lambda = 0.2;
	simulation_time =  1000;
	packets = Generate(lambda, simulation_time);
    backoff_counter = 0;
}

Node::Node(float lambda_, float total_time) {
	lambda = lambda_;
	simulation_time = total_time;
	packets = Generate(lambda, simulation_time);
    backoff_counter = 0;
}

float Node::x_func(float u, float lamda) {
    // Compute the Inverse of the exponential cumulatice distribution
    float k = -1.0/lamda;
    return k*log(1-u);
}

std::vector<float> Node::Generate(float lambda, float total_time) {
    srand(time(0)); 
    vector<float> result;
    float curr_time = 0;
    while(curr_time <= total_time) {
		// Generate random number and pass that and lambda into x_func to get x. Push x to vector.
        float rndNum = rand() / ((double) RAND_MAX);
        // If rndNum is 1 then regenerate otherwise will get a domain error in the log function.
        while(rndNum == 1) {
            rndNum = rand() / ((double) RAND_MAX);
        }

        float num = x_func(rndNum, lambda);
        curr_time += num;
        result.push_back(curr_time);
    }

    return result;
}

void Node::ResetQueue() {

}

void Node::Backoff() {

}

void Node::FailedTransmission(float currTime) {

}