#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

struct PrimaryFlowDistribution
{
  float lambda;
  vector<float> probabilities;
};


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


struct ServerState
{
  //  ServerState(): nextRegular(-1), nextProlongation(-1) {};
  FirstLightStates state1 = Primary;
  int time1 = 0;
  SecondLightStates state2 = LowPriority;
  int time2 = 0;
  int nextRegular = -1;
  int nextProlongation = -1;
  void Print()
  {
    cout<< "(";
    cout <<(state1 == Primary ? "P_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "P_"))<< time2;
    cout<< ")"<<endl;
  }
};
const bool operator == (const ServerState &ss1, const ServerState &ss2);

struct QueueState
{
  int firstLightPrimary;
  int secondLightPrimary;
  int secondLightSecondary;
  int midleQueue;
};

struct Queue
{
Queue(QueueState _state): state(_state){};
  QueueState state;
  PrimaryFlowDistribution firstLight;
  PrimaryFlowDistribution secondLight;
  void MakeIteration(FirstLightSpec fls, SecondLightSpec sls, ServerState serverState)
  {
    
  }
};
  
struct Server
{
  Server (ServerState _state): state(_state){};
  ServerState state;
  vector<ServerState> allStates;
  int prolongationThreshold;
  void MakeIteration(const QueueState qs)
  {
    state = ( qs.secondLightPrimary > prolongationThreshold ? allStates[state.nextProlongation] : allStates[state.nextRegular]);
  }
};

void GenerateStates(vector<ServerState>& vs, ServerState currentState, FirstLightSpec& fls, SecondLightSpec& sls);
