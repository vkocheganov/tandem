#ifndef SERVERS_H
#define SERVERS_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <queue>
#include <list>
#include "generating.h"
using namespace std;


struct PrimaryFlowDistribution
{
  float lambda;
  vector<float> probabilities;
};


enum FirstLightStates
  {
    Primary,
    Secondary
  };
struct FirstLightSpec
{
  int primaryTime;
  float primaryIntensity;
  int secondaryTime;
  float secondaryIntensity;
};
  
enum SecondLightStates
  {
    LowPriority,
    HighPriority,
    Prolongation
  };

struct SecondLightSpec
{
  int lowPriorityTime;
  float lowPriorityIntensity;
  int highPriorityTime;
  float highPriorityIntensity;
  int prolongationTime;
  float prolongationIntensity;
};


struct SystemAprioriInfo
{
  PrimaryFlowDistribution firstFlow, secondFlow;
  FirstLightSpec fls;
  SecondLightSpec sls;
  float midleQueueSuccProb;
  int prolongThres;
  void Print()
  {
    cout <<"--First light info"<<endl;
    
    cout <<"  --Primary flow: ";
    cout <<" lambda = "<<firstFlow.lambda <<"; ";
    cout <<"probabilities = [ ";
    for (auto& a:firstFlow.probabilities)
      cout << a<<" ";
    cout <<"]"<<endl;

    cout <<"  --Server: ";
    cout <<"       primaryTime="<<fls.primaryTime<<", primaryIntensity="<<fls.primaryIntensity<<
      ", secondaryTime="<<fls.secondaryTime<<", secondaryIntensity="<<fls.secondaryIntensity<<endl;



    
    cout <<"--Second light info"<<endl;
    
    cout <<"  --Primary flow: ";
    cout <<" lambda = "<<secondFlow.lambda <<"; ";
    cout <<"probabilities = [ ";
    for (auto& a:secondFlow.probabilities)
      cout << a<<" ";
    cout <<"]"<<endl;

    cout <<"  --Server: ";
    cout <<"       lowPriorityTime="<<sls.lowPriorityTime<<", lowPriorityIntensity="<<sls.lowPriorityIntensity<<", highPriorityTime="<<sls.highPriorityTime<<", highPriorityIntensity="<<sls.highPriorityIntensity<<", prolongationTime="<<sls.prolongationTime<<", prolongationIntensity="<<sls.prolongationIntensity<<endl;

    cout<<"--Midle queue info ";
    cout<<"  queue success probabity: "<<midleQueueSuccProb<<endl;
    
    cout<<"--Prolongation Threshold: ";
    cout<<prolongThres<<endl;
  }
};

struct ServerState
{
  //  ServerState(): nextRegular(-1), nextProlongation(-1) {};
  FirstLightStates state1 = Primary;
  int time1 = 0;
  SecondLightStates state2 = LowPriority;
  int time2 = 0;
  int numCustomersFirstLight = -1;
  int numCustomersSecondLight = -1;
  int timeDuration = 0;
  
  int nextRegular = -1;
  int nextProlongation = -1;
  void Print()
  {
    cout<< "(";
    cout <<(state1 == Primary ? "Pri_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "Pro_"))<< time2;
    cout<< ")"<<" ("<<numCustomersFirstLight<<","<<numCustomersSecondLight<<")";
    cout<<" ("<<nextRegular<<","<<nextProlongation<<")";
    cout <<" ["<<timeDuration<<"]"<<endl;
  }
};
const bool operator == (const ServerState &ss1, const ServerState &ss2);

struct Customer
{
  //Customer():arrivalTime(-1), departureTime(-1){};
  int arrivalTime;
  int departureTime;
  void Print()
  {
    cout<<"Customer: arrival="<<arrivalTime<<", departure="<<departureTime<<endl;
  }
};

struct QueueState
{
  int firstLightPrimary;
  int secondLightSecondary;
  int secondLightPrimary;
  int midleQueue;
  void Print()
  {
    cout<< "Queue state: (";
    cout << firstLightPrimary<<", "<<secondLightSecondary<<", "
	 << secondLightPrimary<<", "<< midleQueue;
    cout<< ")"<<endl;
  }
};

struct Queue
{
  //Queue(QueueState _state): state(_state){};
  queue<Customer> firstLightPrimaryQueue;
  queue<Customer> secondLightHighPriorityQueue;
  queue<Customer> secondLightLowPriorityQueue;
  list<Customer> midleQueue;

  void Init(QueueState initialState, float succProb)
  {
    midleQueueSuccProb = succProb;
    Customer dummy = {0,0};
    for (int i = 0; i < initialState.firstLightPrimary; i++)
      {
	firstLightPrimaryQueue.push(dummy);
      }
    for (int i = 0; i < initialState.secondLightSecondary; i++)
      {
	secondLightHighPriorityQueue.push(dummy);
      }
    for (int i = 0; i < initialState.secondLightPrimary; i++)
      {
	secondLightLowPriorityQueue.push(dummy);
      }
    for (int i = 0; i < initialState.midleQueue; i++)
      {
	midleQueue.push_back(dummy);
      }
  }

  void PrintState()
  {
    cout<<"Queue state: ["<<firstLightPrimaryQueue.size()<<", "
      <<secondLightHighPriorityQueue.size()<<", "
	<<secondLightLowPriorityQueue.size()<<", "
	<<midleQueue.size()<<"]"<<endl;
  }
  QueueState state;
  /* PrimaryFlowDistribution firstLight; */
  /* PrimaryFlowDistribution secondLight; */
  float midleQueueSuccProb;
  void ServiceMidleQueue()
  {
    int queueSize = midleQueue.size();
    float generated;
    for (auto a = midleQueue.begin(); a != midleQueue.end();)
      {
	 generated = float(rand()) / RAND_MAX;
	 //	 cout<<"generated = "<<generated<<" vs "<<midleQueueSuccProb<<endl;
	 if (generated <= midleQueueSuccProb)
	   {
	     // 	     secondLightHighPriorityQueue.push(*(midleQueue.erase(a)));
	     //	     cout <<"erasing ";
	     //	     a->Print();
	     a = midleQueue.erase(a);
	   }
	 else
	   {
	     a++;
	   }
      }
  }


  void MakeIteration(SystemAprioriInfo sai, ServerState serverState)
  {
    UpdateQueues(sai.firstFlow, sai.secondFlow, serverState);

    int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
    int firstLightCustomersToServe = timeToService;
    int secondLightCustomersToServe = timeToService;
    int midleQueueToServe;

    firstLightCustomersToServe *= (serverState.state1 == Primary ? sai.fls.primaryIntensity : 0);
    secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sai.sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );

    if (serverState.state2 == LowPriority)
      {
	int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
	//	cout<<"Second: LowPriority serving: "<<temp_count<<endl;
	for (int i = 0; i < temp_count; i++)
	  {
	    secondLightLowPriorityQueue.pop();
	    //temp
	  }
      }
    else
      {
	int temp_count = std::min(secondLightCustomersToServe,(int)secondLightHighPriorityQueue.size() );
	//	cout<<"Second: High Priority serving: "<<temp_count<<endl;
	for (int i = 0; i < temp_count; i++)
	  {
	    secondLightHighPriorityQueue.pop();
	    //temp
	  }
      }
    ServiceMidleQueue();
    if (serverState.state1 == Primary)
      {
	//	cout<<"First: Primary serving: "<<firstLightCustomersToServe<<endl;
	int temp_count = std::min(firstLightCustomersToServe,(int)firstLightPrimaryQueue.size() );
	for (int i =0; i < temp_count; i++)
	  {
	    midleQueue.push_back(firstLightPrimaryQueue.front());
	    firstLightPrimaryQueue.pop();
	  }
      }

  }

  int GenerateCustomersInBatch(PrimaryFlowDistribution flow)
  {
    float generated = float(rand()) / RAND_MAX;
    int idx = 0;
    float sum = flow.probabilities[idx++];
    while (sum < 1 - 0.00001)
      {
	if (generated <= sum)
	  {
	    return idx;
	  }
	sum += flow.probabilities[idx++];
      }
    return idx;
  }

  void UpdateQueues(PrimaryFlowDistribution firstFlow, PrimaryFlowDistribution secondFlow, ServerState serverState)
  {
    int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
    int firstLightBatches = (firstFlow.lambda * timeToService);
    int secondLightBatches = (secondFlow.lambda * timeToService);

    Customer dummy = {0,0};
    
    for (int i = 0; i < firstLightBatches; i++)
      {
	int custInBatch = GenerateCustomersInBatch(firstFlow);
	for (int j = 0; j < custInBatch; j++)
	  {
	    firstLightPrimaryQueue.push(dummy);
	  }
      }

    for (int i = 0; i < secondLightBatches; i++)
      {
	int custInBatch = GenerateCustomersInBatch(secondFlow);
	for (int j = 0; j < custInBatch; j++)
	  {
	    secondLightLowPriorityQueue.push(dummy);
	  }
      }
  }
};
  
struct Server
{
  //  Server (ServerState _state): state(_state){};
  ServerState state;
  vector<ServerState> allStates;
  int prolongationThreshold;
  void MakeIteration(const QueueState qs)
  {
    state = ( qs.secondLightPrimary > prolongationThreshold ? allStates[state.nextProlongation] : allStates[state.nextRegular]);
  }
  void Init(ServerState serverState, int prolongThres)
  {
    prolongationThreshold = prolongThres;
    state = serverState;
  }
  void Print()
  {
    state.Print();
  }
};

void GenerateStates(vector<ServerState>& vs, int currentState, SystemAprioriInfo sai);

struct Cycle
{
  vector<int> idxs;
  int firstLightTime = 0;
  int secondLightTime = 0;
  float firstLightIncome;
  float secondLightIncome;
  
  int primaryFlowServed = 0;
  int highPriorityFlowServed = 0;
  int lowPriorityFlowServed = 0;
  void CalcStatistics(vector<ServerState>& vs, SystemAprioriInfo sai)
  {
    for (auto a:idxs)
      {
	firstLightTime += vs[a].timeDuration;
	secondLightTime += vs[a].timeDuration;
	if (vs[a].state1 == Primary)
	  {
	    primaryFlowServed += vs[a].numCustomersFirstLight;
	  }
	if (vs[a].state2 == LowPriority)
	  {
	    lowPriorityFlowServed += vs[a].numCustomersSecondLight;
	  }
	else
	  {
	    highPriorityFlowServed += vs[a].numCustomersSecondLight;
	  }
      }
    float firstSum = 0;
    int count = 1;
    for (auto a: sai.firstFlow.probabilities)
      {
      firstSum += count * a;
      count++;
      }
    float secondSum = 0;
    count = 1;
    for (auto a: sai.secondFlow.probabilities)
      {
	secondSum += count * a;
	count++;
      }

    firstLightIncome = firstLightTime * sai.firstFlow.lambda * firstSum;
    secondLightIncome = secondLightTime * sai.secondFlow.lambda * secondSum;
  }
  void Print()
  {
    cout<<"[ ";
    for (auto a:idxs)
      {
	cout<<a<<" ";
      }
    cout <<"]"<<" FirstLightIncome="<<firstLightIncome<<", SecondLightIncome="<<secondLightIncome<<", PrimaryFirstLight_sum{l}="<<primaryFlowServed<<", LowPriority_sum{l}="<<lowPriorityFlowServed<<endl;
  }
};

/* void CalcCycleStatistics(vector<Cycle>& cycles, vector<ServerState>& vs) */
/* { */
  
/* } */

vector<Cycle> FindCycles(vector<ServerState> vs, SystemAprioriInfo sai);

#endif
