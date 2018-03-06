#ifndef SPEC_H
#define SPEC_H
#include "flow.h"


enum FirstLightStates
  {
    Primary,
    Secondary
  };
struct FirstLightSpec
{
  int primaryTime;
  float primaryIntensity;
  int secondaryTime;
  float secondaryIntensity;
};
  
enum SecondLightStates
  {
    LowPriority,
    HighPriority,
    Prolongation
  };

struct SecondLightSpec
{
  int lowPriorityTime;
  float lowPriorityIntensity;
  int highPriorityTime;
  float highPriorityIntensity;
  int prolongationTime;
  float prolongationIntensity;
};


struct SystemAprioriInfo
{
  PrimaryFlowDistribution firstFlow, secondFlow;
  FirstLightSpec fls;
  SecondLightSpec sls;
  float midleQueueSuccProb;
  int prolongThres;
  void Print();
};

struct ServerState
{
  //  ServerState(): nextRegular(-1), nextProlongation(-1) {};
  FirstLightStates state1 = Primary;
  int time1 = 0;
  SecondLightStates state2 = LowPriority;
  int time2 = 0;
  int numCustomersFirstLight = -1;
  int numCustomersSecondLight = -1;
  int timeDuration = 0;
  
  int nextRegular = -1;
  int nextProlongation = -1;
  
  void Print();
  const bool operator == (const ServerState &ss2);
};

struct QueueState
{
  int firstLightPrimary;
  int secondLightSecondary;
  int secondLightPrimary;
  int midleQueue;
  void Print();
};

struct SystemState
{
  ServerState serverState;
  QueueState queueState;
  void Print();
};

#endif
