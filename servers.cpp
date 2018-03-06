#include "servers.h"


void GenerateStates(vector<ServerState>& vs, int currentState, SystemAprioriInfo sai);

Server::Server(ServerState initialState, SystemAprioriInfo sai)
{
  allStates.push_back(initialState);
  GenerateStates(allStates, 0, sai);
  state = 0;
  prolongationThreshold = sai.prolongThres;

  Print();

  //   cout <<"find cycles"<< endl;
  // int count = 0;
  // for (auto a: server.allStates)
  //   {
  //     cout <<"["<<count<<"] ";
  //     a.Print();
  //     count++;
  //   }

}

void Server::MakeIteration(const QueueState qs)
{
  state = ( qs.secondLightPrimary > prolongationThreshold ? allStates[state].nextProlongation : allStates[state].nextRegular);
}
void Server::Print()
{
  allStates[state].Print();
}

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
  


  



void Cycle::CalcStatistics(vector<ServerState>& vs, SystemAprioriInfo sai)
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

void Cycle::Print()
{
  cout<<"[ ";
  for (auto a:idxs)
    {
      cout<<a<<" ";
    }
  cout <<"]"<<" FirstLightIncome="<<firstLightIncome<<", SecondLightIncome="<<secondLightIncome<<", PrimaryFirstLight_sum{l}="<<primaryFlowServed<<", LowPriority_sum{l}="<<lowPriorityFlowServed<<endl;
}





