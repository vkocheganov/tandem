#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"
using namespace std;



struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
  queue<Customer> departFirstQueue;
  queue<Customer> departSecondQueue;
  int beforeServiceTimeFirst = 0;
  int beforeServiceTimeSecond = 0;
  bool stationaryModeFirst = false;
  bool stationaryModeSecond = false;
  const int GRAN = 100;
  SystemAprioriInfo sai;

  void UpdateStatistics(int);
  void DumpMeanTimes();
  void UpdateMeanTimes();
  void DumpDepartQueues();
};
  

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
  void ServiceMidleQueue();

  void MakeIteration(ServerState serverState, int ,int);
  int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
  void UpdateQueues(ServerState serverState, int);

};

#endif
