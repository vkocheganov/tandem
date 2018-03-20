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
  void AddMeans(MyMean&);
  void Print(){ cout<<"mean="<<mean<<", sigma="<<std::sqrt(double(mean_sq - mean*mean))<<", n_samples="<<num<<endl;}
  void Clear() {values.clear(); mean = mean_sq = num = 0;}
};

struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
  deque<Customer> departFirstQueue;
  deque<Customer> departSecondQueue;
  
  bool stationaryMode = false;
  const int GRAN = 100;
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
  void ClearStatistics();
  void DumpMeanTimes();
  void DumpAllCustomers();
  void DumpDepartQueues();
  void AddFirstCustomer(Customer);
  void AddSecondCustomer(Customer);
  void AddStatistics(Statistics&);
  void Print();
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
  //  void PrintStatistics();
  void ServiceMidleQueue(ServerState serverState);

  void MakeIteration(ServerState prevServerState, ServerState serverState, int ,int);
  int GenerateCustomersInBatch(PrimaryFlowDistribution flow);
  int GenerateBatches(float lambda, int timeToService);
  void UpdateQueues(ServerState serverState, int);
};

#endif
