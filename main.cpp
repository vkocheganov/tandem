#include "servers.h"
#include <iostream>

using namespace std;

int main()
{
  cout <<"hello\n";
  vector<ServerState> vs;
  FirstServerTimes fst = {10, 10};
  SecondServerTimes sst = {10, 10,5};
  ServerState startState;
  startState.state1 = Primary;
  startState.time1 = 10;
  startState.state2 = LowPriority;
  startState.time2 = 10;
  GenerateStates(vs, startState, fst, sst);

  return 0;
}
