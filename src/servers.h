#ifndef SERVERS_H
#define SERVERS_H

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "generating.h"
#include "spec.h"
using namespace std;

  
struct Server
{
    Server (ServerState initialState, SystemAprioriInfo sai);
    int state = -1;
    int lastState = -1;
    vector<ServerState> allStates;
    int prolongationThreshold;
    SystemAprioriInfo sai;
    int MakeIteration(int,int);
    void Print(ostream&);
};


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
    void Print(ofstream&);
    void Print_Ext(ofstream&);
};

/* void CalcCycleStatistics(vector<Cycle>& cycles, vector<ServerState>& vs) */
/* { */
  
/* } */

vector<Cycle> FindCycles(vector<ServerState> vs, SystemAprioriInfo sai);

#endif
