#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <deque>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"
#include "statistics.h"
using namespace std;



struct Queue
{
    //Queue(QueueState _state): state(_state){};
    queue<Customer> firstLightPrimaryQueue;
    queue<Customer> secondLightHighPriorityQueue;
    queue<Customer> secondLightLowPriorityQueue;
    list<Customer> midleQueue;

    Statistics stats;
  

    SystemAprioriInfo sai;
    float midleQueueSuccProb;

  

    Queue(QueueState initialState, SystemAprioriInfo sai);
    void PrintState();
    //  void PrintStatistics();
    void ServiceMidleQueue(ServerState serverState);

    void MakeIteration(ServerState prevServerState, ServerState serverState, int ,int);
    int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
    int GenerateBatches(float lambda, int timeToService);
    void UpdateQueues(ServerState serverState, int);
};

#endif
