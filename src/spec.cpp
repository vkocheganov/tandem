#include "spec.h"
#include "iostream"
#include <fstream>
using namespace std;

void SystemAprioriInfo::Print()
{
  cout <<"--First light info"<<endl;
    
  cout <<"  --Primary flow: ";
  cout <<" lambda = "<<firstFlow.lambda <<"; ";
  cout <<"probabilities = [ ";
  for (auto& a:firstFlow.probabilities)
    cout << a<<" ";
  cout <<"]"<<endl;

  cout <<"  --Server: ";
  cout <<"       primaryTime="<<fls.primaryTime<<", primaryIntensity="<<fls.primaryIntensity<<
    ", secondaryTime="<<fls.secondaryTime<<", secondaryIntensity="<<fls.secondaryIntensity<<endl;



    
  cout <<"--Second light info"<<endl;
    
  cout <<"  --Primary flow: ";
  cout <<" lambda = "<<secondFlow.lambda <<"; ";
  cout <<"probabilities = [ ";
  for (auto& a:secondFlow.probabilities)
    cout << a<<" ";
  cout <<"]"<<endl;

  cout <<"  --Server: ";
  cout <<"       lowPriorityTime="<<sls.lowPriorityTime<<", lowPriorityIntensity="<<sls.lowPriorityIntensity<<", highPriorityTime="<<sls.highPriorityTime<<", highPriorityIntensity="<<sls.highPriorityIntensity<<", prolongationTime="<<sls.prolongationTime<<", prolongationIntensity="<<sls.prolongationIntensity<<endl;

  cout<<"--Midle queue info ";
  cout<<"  queue success probabity: "<<midleQueueSuccProb<<endl;
    
  cout<<"--Prolongation Threshold: ";
  cout<<prolongThres<<endl;
}



void SystemAprioriInfo::PrintOpt(ofstream& outStream)
{
  outStream<<"["<<fls.primaryTime<<","<<fls.secondaryTime<<"]   ";
  outStream <<"["<<sls.lowPriorityTime<<","<<sls.highPriorityTime<<","<<sls.prolongationTime <<"] ("<< prolongThres<<")   ";
}

void ServerState::Print()
{
  cout<< "(";
  cout <<(state1 == Primary ? "Pri_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "Pro_"))<< time2;
  cout<< ")"<<" ("<<numCustomersFirstLight<<","<<numCustomersSecondLight<<")";
  cout<<" ("<<nextRegular<<","<<nextProlongation<<")";
  cout <<" ["<<timeDuration<<"]"<<endl;
}


const void QueueState::Print()
{
  cout<< "Queue state: (";
  cout << firstLightPrimary<<", "<<secondLightSecondary<<", "
       << secondLightPrimary<<", "<< midleQueue;
  cout<< ")"<<endl;
}

const bool ServerState::operator == (const ServerState &ss2)
{
  if (state1 == ss2.state1 &&
      time1 == ss2.time1 &&
      state2 == ss2.state2 &&
      time2 == ss2.time2)
    return true;
  else
    return false;
}

void SystemState::Print()
{
  serverState.Print();
  queueState.Print();
}
