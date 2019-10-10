#ifndef GENERATOR_H
#define GENERATOR_H

#include <math.h>
#include <cstdlib>
#include <vector>

using namespace std;

float x_func(float u, float lamda);
vector<float> generate_packets(float lambda, float total_time);

#endif