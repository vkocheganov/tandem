#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <deque>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"
using namespace std;


struct MyMean
{
  vector<int> values;
  double mean = 0;
  double mean_sq = 0;
  
  int num = 0;
  void UpdateMean();
};

struct MovingMean
{
  double mean_untilService = 0;
  double mean_untilService_sq = 0;
  
  double mean_Service = 0;
  double mean_Service_sq = 0;
  int numUntil = 0;
  int numServ = 0;
  void UpdateMean(deque<Customer>& newNumbers, bool stationary = false);
};

struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
  deque<Customer> departFirstQueue;
  deque<Customer> departSecondQueue;
  
  bool stationaryMode = false;
  const int GRAN = 10000;
  const float RATIO_CHANGE = 0.1;
  SystemAprioriInfo sai;

  MyMean firstTimeUntilServ;
  MyMean firstTimeServ;
  MyMean secondTimeUntilServ;
  MyMean secondTimeServ;
  
  MyMean firstPrimary;
  MyMean secondHigh;
  MyMean secondLow;
  MyMean middle;

  void UpdateStatistics(int);
  void DumpMeanTimes();
  void DumpAllCustomers();
  void DumpDepartQueues();
  void AddFirstCustomer(Customer);
  void AddSecondCustomer(Customer);
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
