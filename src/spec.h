#ifndef SPEC_H
#define SPEC_H
#include "flow.h"
#include <string>


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

struct OutFiles
{
    string foldName;
    
    string stationaryFileMeans;
    string firstCustomersFile;
    string secondCustomersFile;
    
    string optFile;

    string saiFile;
};

struct SystemAprioriInfo
{
    PrimaryFlowDistribution firstFlow, secondFlow;
    FirstLightSpec fls;
    SecondLightSpec sls;
    float midleSuccProbFactor;
    int prolongThres;
    bool verbose;
    int numMaxIteration;
    int numIterationStationary;
    int numSamples;

    bool fileVerbose = true;
    //  string filename;

    void Print(ofstream&);
    void PrintOpt(ofstream&);
    float GetSuccProb(int timeDur) { return (1 - exp(-timeDur*midleSuccProbFactor));}
    OutFiles outFiles;
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
    float midleSuccProb = 0.;
  
    int nextRegular = -1;
    int nextProlongation = -1;
  
    void Print(ostream&);
    const bool operator == (const ServerState &ss2);
};

struct QueueState
{
    QueueState(bool Rand = false);
    int firstLightPrimary;
    int secondLightSecondary;
    int secondLightPrimary;
    int midleQueue;

    int UPPER_BOUND = 10;
    const void Print();
};

struct SystemState
{
    ServerState serverState;
    QueueState queueState;
    void Print(ostream&);
};

#endif
