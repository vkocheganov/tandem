#include "servers.h"
#include <iostream>
#include "spec.h"
#include "io.h"
#include "servers.h"
#include "queue.h"

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

  cout <<endl<<"Starting!"<<endl;
  Queue myQueue;
  myQueue.Init(initialQueueState, sai.midleQueueSuccProb);
  myQueue.PrintState();

  Server server;
  cout<<endl<<"generating server states:"<<endl;
  server.allStates.push_back(initialServerState);
  GenerateStates(server.allStates, 0, sai);

  cout <<"find cycles"<< endl;
  

  cout<<"outing"<<endl;
  int count = 0;
  for (auto a: server.allStates)
    {
      cout <<"["<<count<<"] ";
      a.Print();
      count++;
    }
  vector<Cycle> cycles = FindCycles(server.allStates, sai);
  cout <<"Cycles num = "<<cycles.size()<<endl;
  for (auto a:cycles)
    {
      a.Print();
    }
  cout<<"server init"<<endl;

  server.Init(server.allStates[0], sai.prolongThres);
  server.Print();
  myQueue.PrintState();

  const int ITERS = 10000;
  for (int i = 0; i < ITERS; i++)
    {
      
      server.MakeIteration(myQueue.state);
      myQueue.MakeIteration(sai, server.state);
      // cout<<endl<<"after " <<i<<" iteration"<<endl;
      // server.Print();
      // myQueue.PrintState();
    }
  server.Print();
  myQueue.PrintState();
  
  // server.MakeIteration(myQueue.state);
  // myQueue.MakeIteration(sai.fls, sai.sls, server.state);
  // server.Print();
  // myQueue.PrintState();
  
  // server.MakeIteration(myQueue.state);
  // server.Print();
  
  // return 0;
  // vector<ServerState> vs;
  // FirstLightSpec fls = {10, 1, 10, 1};
  // SecondLightSpec sls = {10, 1, 10, 1, 5, 1};
  // ServerState startState;
  // startState.state1 = Primary;
  // startState.time1 = 10;
  // startState.state2 = LowPriority;
  // startState.time2 = 10;
  // GenerateStates(vs, startState, fls, sls);

  return 0;
}
