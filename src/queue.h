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

  queue<Customer> departFirstQueue;
  queue<Customer> departSecondQueue;
  int untilServiceTimeFirst = 0;
  int untilServiceTimeSecond = 0;
  bool stationaryModeFirst = false;
  bool stationaryModeSecond = false;
  

  SystemAprioriInfo sai;
  float midleQueueSuccProb;

  

  Queue(QueueState initialState, SystemAprioriInfo sai);
  void PrintState();
  void ServiceMidleQueue();

  void MakeIteration(ServerState serverState, int ,int);
  int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
  void UpdateQueues(ServerState serverState, int);

  void UpdateMeanTimes();
  void DumpDepartQueues();
};

#endif
