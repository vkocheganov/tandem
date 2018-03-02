#include "servers.h"
#include <iostream>

using namespace std;

int main()
{
  cout <<"hello\n";
  vector<ServerState> vs;
  FirstLightSpec fls = {10, 1, 10, 1};
  SecondLightSpec sls = {10, 1, 10, 1, 5, 1};
  ServerState startState;
  startState.state1 = Primary;
  startState.time1 = 10;
  startState.state2 = LowPriority;
  startState.time2 = 10;
  GenerateStates(vs, startState, fls, sls);

  return 0;
}
