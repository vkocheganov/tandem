#include "servers.h"
#include <iostream>
#include "io.h"

using namespace std;

int main()
{
  SystemAprioriInfo sai = ReadSpecs("sample_spec");
  sai.Print();
  ServerState initialServerState;
  QueueState initialQueueState;
  ReadStates("sample_states", initialServerState, initialQueueState);
  initialServerState.Print();
  initialQueueState.Print();

  cout <<endl<<"Starting!"<<endl;
  Queue myQueue;
  myQueue.Init(initialQueueState);
  myQueue.PrintState();

  Server server;
  cout<<endl<<"generating server states:"<<endl;
  server.allStates.push_back(initialServerState);
  GenerateStates(server.allStates, 0, sai.fls, sai.sls);

  cout<<"outing"<<endl;
  int count = 0;
  for (auto a: server.allStates)
    {
      cout <<"["<<count<<"] ";
      a.Print();
      count++;
    }
  cout<<"server init"<<endl;

  server.Init(server.allStates[0]);
  server.Print();
  cout<<"server afte iteration"<<endl;
  server.MakeIteration(myQueue.state);
  server.Print();
  server.MakeIteration(myQueue.state);
  server.Print();
  server.MakeIteration(myQueue.state);
  server.Print();
  
  return 0;
  vector<ServerState> vs;
  FirstLightSpec fls = {10, 1, 10, 1};
  SecondLightSpec sls = {10, 1, 10, 1, 5, 1};
  ServerState startState;
  startState.state1 = Primary;
  startState.time1 = 10;
  startState.state2 = LowPriority;
  startState.time2 = 10;
  // GenerateStates(vs, startState, fls, sls);

  return 0;
}
