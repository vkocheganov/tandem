#include "servers.h"
#include <iostream>
#include "spec.h"
#include "io.h"
#include "servers.h"
#include "queue.h"
#include "system.h"

using namespace std;

int main()
{
  SystemAprioriInfo sai = ReadSpecs("sample_spec_0");
  sai.Print();
  ServerState initialServerState;
  QueueState initialQueueState;
  ReadStates("sample_states_0", initialServerState, initialQueueState);
  initialServerState.Print();
  initialQueueState.Print();

  System system(initialQueueState, initialServerState, sai);

  vector<Cycle> cycles = FindCycles(system.server.allStates, sai);

  const int ITERS = 1;
  for (int i = 0; i < ITERS; i++)
    {
      system.MakeIteration();
    }

  system.Print();
  
  return 0;
}
