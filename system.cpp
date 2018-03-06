#include "system.h"

System::System (QueueState initQueueState, ServerState initServerState, SystemAprioriInfo _sai):
  sQueue(initQueueState, _sai.midleQueueSuccProb), server(initServerState, _sai), sai(_sai)
{
  cycles = FindCycles(server.allStates, sai);
  cout <<"Cycles num = "<<cycles.size()<<endl;
  for (auto a:cycles) a.Print();
  timeTotal = 0;
}

void System::MakeIteration()
{
  int prevTotalTime = timeTotal;
  timeTotal += server.MakeIteration(sQueue.secondLightLowPriorityQueue.size());
  sQueue.MakeIteration(sai, server.allStates[server.state], prevTotalTime);
}

void System::Print()
{
  server.Print();
  sQueue.PrintState();
}
