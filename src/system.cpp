#include "system.h"

System::System (QueueState initQueueState, ServerState initServerState, SystemAprioriInfo _sai):
  sQueue(initQueueState, _sai), server(initServerState, _sai), sai(_sai)
{
  cycles = FindCycles(server.allStates, sai);
  if (sai.verbose)
    {
      cout <<"Cycles num = "<<cycles.size()<<endl;
      for (auto a:cycles) a.Print();
    }
  timeTotal = 0;
}

void System::MakeIteration(int iteration)
{
  int prevTotalTime = timeTotal;
  timeTotal += server.MakeIteration(sQueue.secondLightLowPriorityQueue.size(), iteration);
  sQueue.MakeIteration(server.allStates[server.state], prevTotalTime, iteration);
}

void System::Print()
{
  server.Print();
  sQueue.PrintState();
  sQueue.PrintStatistics();
}
