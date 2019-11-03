#include <math.h>
#include <deque>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <iostream>

#include "node.h"

using namespace std;

Node::Node() {
	lambda = 0.2;
	simulation_time =  1000;
	packets = Generate(lambda, simulation_time);
    backoff_counter = 0;
    dropped = 0;
    nonPersistent_backoff_counter = 0;
}

Node::Node(float lambda_, float total_time) {
	lambda = lambda_;
	simulation_time = total_time;
	packets = Generate(lambda, simulation_time);
    backoff_counter = 0;
    dropped = 0;
    nonPersistent_backoff_counter = 0;
}

float Node::x_func(float u, float lamda) {
    // Compute the Inverse of the exponential cumulatice distribution
    float k = -1.0/lamda;
    return k*log(1-u);
}

std::deque<float> Node::Generate(float lambda, float total_time) {
    deque<float> result;
    float curr_time = 0;
    while(curr_time <= total_time) {
		// Generate random number and pass that and lambda into x_func to get x. Push x to deque.
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

void Node::ProcessCollision(float transmissionSpeed) {
    if(backoff_counter >= 10) {
        dropped++;
        ProcessSuccess();
        return;
    } else {
        backoff_counter++;
        int min = 0;
        int max = pow(2, backoff_counter)-1;
        int random = min + rand() % (( max + 1 ) - min);
        float Tp = 512.0*(1.0/transmissionSpeed); // 512 * 1 bit time
        AddTime(packets.front()+Tp*random);
    }
}

void Node::ProcessSuccess() {
    backoff_counter = 0;
    nonPersistent_backoff_counter = 0;
    packets.pop_front();
}

void Node::ProcessLineBusy_Persistent(float newTime) {
    AddTime(newTime);
}

void Node::ProcessLineBusy_NonPersistent() {
    
}

void Node::AddTime(float newTime) {
    int i = 0;
    while(i < packets.size() && packets[i] < newTime) {
        packets[i] = newTime;
        i++;
    }
}