#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"
using namespace std;

struct Queue
{
  //Queue(QueueState _state): state(_state){};
  queue<Customer> firstLightPrimaryQueue;
  queue<Customer> secondLightHighPriorityQueue;
  queue<Customer> secondLightLowPriorityQueue;
  list<Customer> midleQueue;
  QueueState state;
  float midleQueueSuccProb;

  Queue(QueueState initialState, float succProb);
  void PrintState();
  void ServiceMidleQueue();

  void MakeIteration(SystemAprioriInfo sai, ServerState serverState);
  int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
  void UpdateQueues(PrimaryFlowDistribution firstFlow, PrimaryFlowDistribution secondFlow, ServerState serverState);
};

#endif
