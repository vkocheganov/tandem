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
  
    System (QueueState, ServerState, SystemAprioriInfo);
    void MakeIteration(int);
    void Print(ostream&);
    bool CheckStationaryMode(System& ,int);
    bool StopCriteria();
};
#endif
