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
  PrimaryFlowDistribution firstLight, secondLight;
  FirstLightSpec fls;
  SecondLightSpec sls;
  float midleQueueSuccProb;
  int prolongThres;
  void Print()
  {
    cout <<"--First light info"<<endl;
    
    cout <<"  --Primary flow: ";
    cout <<" lambda = "<<firstLight.lambda <<"; ";
    cout <<"probabilities = [ ";
    for (auto& a:firstLight.probabilities)
      cout << a<<" ";
    cout <<"]"<<endl;

    cout <<"  --Server: ";
    cout <<"       primaryTime="<<fls.primaryTime<<", primaryIntensity="<<fls.primaryIntensity<<
      ", secondaryTime="<<fls.secondaryTime<<", secondaryIntensity="<<fls.secondaryIntensity<<endl;



    
    cout <<"--Second light info"<<endl;
    
    cout <<"  --Primary flow: ";
    cout <<" lambda = "<<secondLight.lambda <<"; ";
    cout <<"probabilities = [ ";
    for (auto& a:secondLight.probabilities)
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
  
  int nextRegular = -1;
  int nextProlongation = -1;
  void Print()
  {
    cout<< "(";
    cout <<(state1 == Primary ? "Pri_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "Pro_"))<< time2;
    cout<< ")"<<" ("<<numCustomersFirstLight<<","<<numCustomersSecondLight<<")";
    cout<<" ("<<nextRegular<<","<<nextProlongation<<")"<<endl;
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
	 cout<<"generated = "<<generated<<" vs "<<midleQueueSuccProb<<endl;
	 if (generated <= midleQueueSuccProb)
	   {
	     // 	     secondLightHighPriorityQueue.push(*(midleQueue.erase(a)));
	     cout <<"erasing ";
	     a->Print();
	     a = midleQueue.erase(a);
	   }
	 else
	   {
	     a++;
	   }
      }
  }


  void MakeIteration(FirstLightSpec fls, SecondLightSpec sls, ServerState serverState)
  {
    int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
    int firstLightCustomersToServe = timeToService;
    int secondLightCustomersToServe = timeToService;
    int midleQueueToServe;

    firstLightCustomersToServe *= (serverState.state1 == Primary ? fls.primaryIntensity : 0);
    secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sls.highPriorityIntensity : sls.prolongationIntensity) );

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
  void UpdateQueues(PrimaryFlowDistribution firstFlow, PrimaryFlowDistribution secondFlow)
  {
    
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


#endif
