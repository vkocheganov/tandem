#include "servers.h"

void GenerateStates(vector<ServerState>& vs, ServerState currentState, FirstLightSpec& fls, SecondLightSpec& sls)
{
  int serverToFinish;
  int temp, timeToFinish;
  int eligibleToProlong;
  ServerState newState;

  if (currentState.time1 < currentState.time2)
    {
      serverToFinish = 1;
      timeToFinish = currentState.time1;
    }
  else if (currentState.time1 > currentState.time2)
    {
      serverToFinish = 2;
      timeToFinish = currentState.time2;
    }
  else
    {
      serverToFinish = 0;
      timeToFinish = currentState.time1;
    }

  eligibleToProlong = (serverToFinish == 0 || serverToFinish == 2) && currentState.state2 > LowPriority;
  if (serverToFinish == 1)
    {
      newState.state1 = (currentState.state1 == Primary ? Secondary : Primary);
      newState.time1 = (newState.state1 == Primary ? fls.primaryTime : fls.secondaryTime);
      newState.state2 = currentState.state2;
      newState.time2 = currentState.time2 - timeToFinish;
    }
  else if (serverToFinish == 2)
    {
      newState.state1 = currentState.state1;
      newState.time1 = currentState.time1 - timeToFinish;
      newState.state2 = (currentState.state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sls.lowPriorityTime : sls.highPriorityTime);
    }
  else
    {
      newState.state1 = (currentState.state1 == Primary ? Secondary : Primary);
      newState.time1 = (newState.state1 == Primary ? fls.primaryTime : fls.secondaryTime);
      newState.state2 = (currentState.state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sls.lowPriorityTime : sls.highPriorityTime);
    }

  auto tmp = find(vs.begin(), vs.end(), newState);
  if (tmp == vs.end())
    {
      cout <<"Adding new regular state:";
      newState.Print();
      vs.push_back(newState);
      currentState.nextRegular = vs.size() - 1;
      GenerateStates(vs, newState, fls, sls);
    }
  else
    {
      currentState.nextRegular = tmp - vs.begin();
    }
  if (eligibleToProlong)
    {
      newState.state2 = Prolongation;
      newState.time2 = sls.prolongationTime;
      auto tmp = find(vs.begin(), vs.end(), newState);
      if ( tmp == vs.end())
	{
	  cout <<"Adding new prolongation state:";
	  newState.Print();
	  vs.push_back(newState);
	  currentState.nextProlongation = vs.size() - 1;
	  GenerateStates(vs, newState, fls, sls);
	}
      else
	{
	  currentState.nextProlongation = tmp - vs.begin();
	}
    }
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
