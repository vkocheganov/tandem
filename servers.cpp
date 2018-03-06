#include "servers.h"

void GenerateStates(vector<ServerState>& vs, int currentState, SystemAprioriInfo sai)
{
  int serverToFinish;
  int temp, timeToFinish;
  int eligibleToProlong;
  ServerState newState;

  int firstLightCustomersToServe;
  int secondLightCustomersToServe;

  // firstLightCustomersToServe *= (serverState.state1 == Primary ? fls.primaryIntensity : 0);
  // secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sls.highPriorityIntensity : sls.prolongationIntensity) );
  
  if (vs[currentState].time1 < vs[currentState].time2)
    {
      serverToFinish = 1;
      timeToFinish = vs[currentState].time1;
    }
  else if (vs[currentState].time1 > vs[currentState].time2)
    {
      serverToFinish = 2;
      timeToFinish = vs[currentState].time2;
    }
  else
    {
      serverToFinish = 0;
      timeToFinish = vs[currentState].time1;
    }
  vs[currentState].timeDuration = timeToFinish;

  if (vs[currentState].numCustomersFirstLight < 0 || vs[currentState].numCustomersSecondLight < 0)
    {
      firstLightCustomersToServe = timeToFinish;
      secondLightCustomersToServe = timeToFinish;
      firstLightCustomersToServe *= (vs[currentState].state1 == Primary ? sai.fls.primaryIntensity : 0);
      secondLightCustomersToServe *= (vs[currentState].state2 == LowPriority ? sai.sls.lowPriorityIntensity : (vs[currentState].state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );
      vs[currentState].numCustomersFirstLight = firstLightCustomersToServe;
      vs[currentState].numCustomersSecondLight = secondLightCustomersToServe;
    }

  
  eligibleToProlong = (serverToFinish == 0 || serverToFinish == 2) && vs[currentState].state2 > LowPriority;
  if (serverToFinish == 1)
    {
      newState.state1 = (vs[currentState].state1 == Primary ? Secondary : Primary);
      newState.time1 = (newState.state1 == Primary ? sai.fls.primaryTime : sai.fls.secondaryTime);
      newState.state2 = vs[currentState].state2;
      newState.time2 = vs[currentState].time2 - timeToFinish;
      newState.timeDuration = timeToFinish;
    }
  else if (serverToFinish == 2)
    {
      newState.state1 = vs[currentState].state1;
      newState.time1 = vs[currentState].time1 - timeToFinish;
      newState.state2 = (vs[currentState].state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sai.sls.lowPriorityTime : sai.sls.highPriorityTime);
    }
  else
    {
      //      cout<<"both to finish"<<endl;
      newState.state1 = (vs[currentState].state1 == Primary ? Secondary : Primary);
      newState.time1 = (newState.state1 == Primary ? sai.fls.primaryTime : sai.fls.secondaryTime);
      newState.state2 = (vs[currentState].state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sai.sls.lowPriorityTime : sai.sls.highPriorityTime);
    }

  auto tmp = find(vs.begin(), vs.end(), newState);
  if (tmp == vs.end())
    {
      // cout <<"Adding new regular state:";
      // newState.Print();
      vs.push_back(newState);
      vs[currentState].nextRegular = vs.size() - 1;
      // cout<<"CurrentState :" <<eligibleToProlong;
      // vs[currentState].Print();
      GenerateStates(vs, vs.size()-1, sai);
    }
  else
    {
      vs[currentState].nextRegular = tmp - vs.begin();
    }
  if (eligibleToProlong)
    {
      newState.state2 = Prolongation;
      newState.time2 = sai.sls.prolongationTime;
      auto tmp = find(vs.begin(), vs.end(), newState);
      if ( tmp == vs.end())
	{
	  // cout <<"Adding new prolongation state:";
	  // newState.Print();
      // cout<<"CurrentState :";
      // vs[currentState].Print();
	  vs.push_back(newState);
	  vs[currentState].nextProlongation = vs.size() - 1;
	  GenerateStates(vs, vs.size()-1, sai);
	}
      else
	{
	  vs[currentState].nextProlongation = tmp - vs.begin();
      	  // cout<<"found CurrentState :";
	  // vs[currentState].Print();
	  // vs[0].Print();
	  // tmp->Print();
	}
    }
}


void IterateFindCycles(vector<ServerState>& vs, int currentState, vector<bool>& processed, vector<Cycle>& cycles, int currentCycle)
{
  cycles[currentCycle].idxs.push_back(currentState);
  processed[currentState] = true;
  int nextRegular = vs[currentState].nextRegular;
  int nextProlongation = vs[currentState].nextProlongation;

  if (vs[currentState].state2 != Prolongation)
    {
      if (!processed[nextRegular])
      	{
      	  IterateFindCycles(vs, nextRegular, processed, cycles, currentCycle);
      	}
      if (nextProlongation != -1 && !processed[nextProlongation])
      	{
      	  Cycle dummy;
      	  cycles.push_back(dummy);
      	  IterateFindCycles(vs, nextProlongation, processed, cycles, cycles.size()-1);
      	}
    }
  else
    {
      if (nextProlongation != -1 && !processed[nextProlongation])
      	{
      	  IterateFindCycles(vs, nextProlongation, processed, cycles, currentCycle);
      	}
      if (!processed[nextRegular])
      	{
      	  Cycle dummy;
      	  cycles.push_back(dummy);
      	  IterateFindCycles(vs, nextRegular, processed, cycles, cycles.size()-1);
      	}
    }
}

vector<Cycle> FindCycles(vector<ServerState> vs, SystemAprioriInfo sai)
{
  vector<bool> processed(vs.size(),0);
  vector<Cycle> cycles;
  Cycle dummy;
  cycles.push_back(dummy);
  IterateFindCycles(vs, 0, processed, cycles, 0);
  for (auto& a:cycles)
    a.CalcStatistics(vs, sai);
  return cycles;
}
  

const bool operator == (const ServerState &ss1, const ServerState &ss2)
{
  if (ss1.state1 == ss2.state1 &&
      ss1.time1 == ss2.time1 &&
      ss1.state2 == ss2.state2 &&
      ss1.time2 == ss2.time2)
    return true;
  else
    return false;
}
