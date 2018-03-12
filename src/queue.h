#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <deque>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"
using namespace std;


struct MovingMean
{
  double mean_untilService = 0;
  double mean_Service = 0;
  int num = 0;
  void UpdateMean(deque<Customer>& newNumbers);
};

struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
  deque<Customer> departFirstQueue;
  deque<Customer> departSecondQueue;
  float beforeServiceTimeFirst = 0;
  float beforeServiceTimeSecond = 0;
  bool stationaryModeFirst = false;
  bool stationaryModeSecond = false;
  bool stationaryMode = false;
  const int GRAN = 200;
  const float RATIO_CHANGE = 0.15;
  SystemAprioriInfo sai;

  MovingMean stationaryMeanTime_first;
  MovingMean stationaryMeanTime_second;

  void UpdateStatistics(int);
  void DumpMeanTimes();
  void DumpAllCustomers();
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
  void PrintStatistics();
  void ServiceMidleQueue(ServerState serverState);

  void MakeIteration(ServerState serverState, int ,int);
  int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
  int GenerateBatches(float lambda, int timeToService);
  void UpdateQueues(ServerState serverState, int);

};

#endif
