#include "servers.h"
#include <iostream>
#include "io.h"

using namespace std;

int main()
{
  SystemAprioriInfo sai = ReadSpecs("sample_spec");
  sai.Print();
  return 0;
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
