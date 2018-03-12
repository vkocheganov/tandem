#ifndef FLOW_H
#define FLOW_H

#include <vector>
#include <random>
using namespace std;

struct PrimaryFlowDistribution
{
static mt19937 generator;
  float lambda;
  vector<float> probabilities;
};



#endif
