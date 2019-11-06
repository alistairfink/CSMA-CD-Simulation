#include <math.h>
#include <deque>
#include <stdlib.h>
#include <random>
#include <chrono>
#include <iostream>

#include "node.h"

using namespace std;

Node::Node() {
    // Default Constructor. Sets stuff to defaults. Not Used.
	lambda = 0.2;
	simulation_time =  1000;
	packets = Generate(lambda, simulation_time);

    // Set counters to 0.
    backoff_counter = 0;
    dropped = 0;
    nonPersistent_backoff_counter = 0;
}

Node::Node(float lambda_, float total_time) {
    // 2nd Constructor. Takes input then generates nodes based off it.
	lambda = lambda_;
	simulation_time = total_time;
	packets = Generate(lambda, simulation_time);

    // Set counters to 0.
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
    // Keeps pushing nodes onto deque until past simulation time.
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

void Node::ProcessCollision(float collissionTime, float transmissionSpeed) {
    // If backoff counter is 10 or greater than drop packet and reset counters (the reason ProcessSuccess() is being called.)
    if(backoff_counter >= 10) {
        dropped++;
        ProcessSuccess();
        return;
    } else {
        // Else increment counter then get random number between 0 and (2^i)-1
        backoff_counter++;
        int min = 0;
        int max = pow(2, backoff_counter)-1;
        int random = min + rand() % (( max + 1 ) - min);
        float Tp = 512.0*(1.0/transmissionSpeed); // 512 * 1 bit time
        // Final wait time is the random number * (512 bit times(bit time = 1/transmission speed))
        AddTime(collissionTime+Tp*random);
    }
}

void Node::ProcessSuccess() {
    // "Success" means reset counters and pop first packet. Incidentally this is what also needs to happen when exponential backoff counters are >= 10
    backoff_counter = 0;
    nonPersistent_backoff_counter = 0;
    packets.pop_front();
}

void Node::ProcessLineBusy_Persistent(float newTime) {
    // Nodes try to broadcast as soon as the line is free so just set the time of nodes lower to the end of the current busy time.
    AddTime(newTime);
}

void Node::ProcessLineBusy_NonPersistent(float transmissionSpeed) {
    // Same as above. If counter is >= 10 then drop it and reset counters.
    if(nonPersistent_backoff_counter >= 10) {
        dropped++;
        ProcessSuccess();
        return;
    } else {
        // Same as above. Increment counter then choose random time of 512 * bit time * random[0, (2^i)-1]
        nonPersistent_backoff_counter++;
        int min = 0;
        int max = pow(2, nonPersistent_backoff_counter)-1;
        int random = min + rand() % (( max + 1 ) - min);
        float Tp = 512.0*(1.0/transmissionSpeed); // 512 * 1 bit time
        AddTime(packets.front()+Tp*random);
    }
    
}

void Node::AddTime(float newTime) {
    // Sets all packets that are lower than the new time to the new time.
    int i = 0;
    while(i < packets.size() && packets[i] < newTime) {
        packets[i] = newTime;
        i++;
    }
}