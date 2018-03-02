#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

enum FirstServerStates
  {
    Primary,
    Secondary
  };
struct FirstServerTimes
{
  int primary;
  int secondary;
};
  
enum SecondServerStates
  {
    LowPriority,
    HighPriority,
    Prolongation
  };

struct SecondServerTimes
{
  int lowPriority;
  int highPriority;
  int prolongation;
};


struct ServerState
{
  //  ServerState(): nextRegular(-1), nextProlongation(-1) {};
  FirstServerStates state1 = Primary;
  int time1 = 0;
  SecondServerStates state2 = LowPriority;
  int time2 = 0;
  int nextRegular = -1;
  int nextProlongation = -1;
  void Print()
  {
    cout<< "(";
    cout <<(state1 == Primary ? "P_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "P_"))<< time2;
    cout<< ")"<<endl;
  }
};

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


void GenerateStates(vector<ServerState>& vs, ServerState currentState, FirstServerTimes& fst, SecondServerTimes& sst)
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
      newState.time1 = (newState.state1 == Primary ? fst.primary : fst.secondary);
      newState.state2 = currentState.state2;
      newState.time2 = currentState.time2 - timeToFinish;
    }
  else if (serverToFinish == 2)
    {
      newState.state1 = currentState.state1;
      newState.time1 = currentState.time1 - timeToFinish;
      newState.state2 = (currentState.state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sst.lowPriority : sst.highPriority);
    }
  else
    {
      newState.state1 = (currentState.state1 == Primary ? Secondary : Primary);
      newState.time1 = (newState.state1 == Primary ? fst.primary : fst.secondary);
      newState.state2 = (currentState.state2  > LowPriority ? LowPriority : HighPriority);
      newState.time2 = (newState.state2 == LowPriority ? sst.lowPriority : sst.highPriority);
    }
  
  if (find(vs.begin(), vs.end(), newState) == vs.end())
    {
      cout <<"Adding new regular state:";
      newState.Print();
      vs.push_back(newState);
      currentState.nextRegular = vs.size() - 1;
      GenerateStates(vs, newState, fst, sst);
    }
  if (eligibleToProlong)
    {
      newState.state2 = Prolongation;
      newState.time2 = sst.prolongation;
      if (find(vs.begin(), vs.end(), newState) == vs.end())
	{
	  cout <<"Adding new prolongation state:";
	  newState.Print();
	  vs.push_back(newState);
	  currentState.nextProlongation = vs.size() - 1;
	  GenerateStates(vs, newState, fst, sst);
	}
    }
}
