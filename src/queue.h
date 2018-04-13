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
    double diff = 0;
    double diff_std = 0;
  
    int num = 0;
    void UpdateMean();
    void AddMeans(MyMean&);
    void Print(){ cout<<"mean="<<mean<<", sigma="<<std::sqrt(double(mean_sq - mean*mean))<<", n_samples="<<num<<endl;}
    /* void DumpMeans(ofstream& _stream){ _stream<<mean<<" ";} */
    void DumpMeans(ofstream& _stream){ _stream<<mean<<" "<<diff<<"  ";}
    void DumpMeansDiffs(ofstream& _stream){ _stream<<diff<<" ";}
    void DumpStd(ofstream& _stream){ _stream<<std::sqrt(double(mean_sq - mean*mean))<<" ";}
    void DumpStdDiffs(ofstream& _stream){ _stream<<diff_std<<" ";}
    void Clear() {values.clear(); mean = mean_sq = num = 0;}
};

struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
    deque<Customer> departFirstQueue;
    deque<Customer> departSecondQueue;

    long long unsigned inputFirstCust = 0;
    long long unsigned outputFirstCust = 0;
    long long unsigned inputThirdCust = 0;
    long long unsigned outputThirdCust = 0;
  
    bool stationaryMode = false;
    const int GRAN = 100;
    const float RATIO_CHANGE = 0.01;
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
    void DumpAllCustomers();
    void DumpDepartQueues();
    void AddFirstCustomer(Customer);
    void AddSecondCustomer(Customer);
    void AddStatistics(Statistics&);
    void Print();
    void DumpStatsMean(ofstream&);
    void DumpStatsMeanDiffs(ofstream&);
    void DumpStatsStd(ofstream&);
    void DumpStatsStdDiffs(ofstream&);
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
