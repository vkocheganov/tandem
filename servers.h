#ifndef SERVERS_H
#define SERVERS_H

#include <vector>
#include <iostream>
#include <algorithm>
#include "generating.h"
#include "spec.h"
using namespace std;







const bool operator == (const ServerState &ss1, const ServerState &ss2);


  
struct Server
{
  //  Server (ServerState _state): state(_state){};
  ServerState state;
  vector<ServerState> allStates;
  int prolongationThreshold;
  void MakeIteration(const QueueState qs);
  void Init(ServerState serverState, int prolongThres);
  void Print();
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
  void CalcStatistics(vector<ServerState>& vs, SystemAprioriInfo sai);
  void Print();
};

/* void CalcCycleStatistics(vector<Cycle>& cycles, vector<ServerState>& vs) */
/* { */
  
/* } */

vector<Cycle> FindCycles(vector<ServerState> vs, SystemAprioriInfo sai);

#endif
