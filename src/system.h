#ifndef SYSTEM_H
#define SYSTEM_H

#include "queue.h"
#include "servers.h"
#include "spec.h"
struct System
{
  Queue sQueue;
  Server server;
  vector<Cycle> cycles;
  SystemAprioriInfo sai;
  int timeTotal;
  
  System (QueueState, ServerState, SystemAprioriInfo);
  void MakeIteration(int);
  void Print();
  void CheckStationaryMode(System& ,int);
};
#endif
