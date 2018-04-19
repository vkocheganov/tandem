#include "spec.h"
#include "iostream"
#include <fstream>
using namespace std;

void SystemAprioriInfo::Print(ofstream& outStream)
{
    outStream <<"--First light info"<<endl;
    
    outStream <<"  --Primary flow: ";
    outStream <<" lambda = "<<firstFlow.lambda <<"; ";
    outStream <<"probabilities = [ ";
    for (auto& a:firstFlow.probabilities)
        outStream << a<<" ";
    outStream <<"]"<<endl;

    outStream <<"  --Server: ";
    outStream <<"       primaryTime="<<fls.primaryTime<<", primaryIntensity="<<fls.primaryIntensity<<
        ", secondaryTime="<<fls.secondaryTime<<", secondaryIntensity="<<fls.secondaryIntensity<<endl;



    
    outStream <<"--Second light info"<<endl;
    
    outStream <<"  --Primary flow: ";
    outStream <<" lambda = "<<secondFlow.lambda <<"; ";
    outStream <<"probabilities = [ ";
    for (auto& a:secondFlow.probabilities)
        outStream << a<<" ";
    outStream <<"]"<<endl;

    outStream <<"  --Server: ";
    outStream <<"       lowPriorityTime="<<sls.lowPriorityTime<<", lowPriorityIntensity="<<sls.lowPriorityIntensity<<", highPriorityTime="<<sls.highPriorityTime<<", highPriorityIntensity="<<sls.highPriorityIntensity<<", prolongationTime="<<sls.prolongationTime<<", prolongationIntensity="<<sls.prolongationIntensity<<endl;

    outStream<<"--Midle queue info ";
    outStream<<"  queue success probabity factor: "<<midleSuccProbFactor<<endl;
    
    outStream<<"--Prolongation Threshold: ";
    outStream<<prolongThres<<endl;
}



void SystemAprioriInfo::PrintOpt(ofstream& outStream)
{
    outStream<<"["<<fls.primaryTime<<","<<fls.secondaryTime<<"]   ";
    outStream <<"["<<sls.lowPriorityTime<<","<<sls.highPriorityTime<<","<<sls.prolongationTime <<"] ("<< prolongThres<<")   ";
}

void ServerState::Print(ostream& outStream)
{
    outStream<< "(";
    outStream <<(state1 == Primary ? "Pri_" : "S_") << time1<< " | "<< (state2 == LowPriority ? "L_" : (state2 == HighPriority ? "H_" : "Pro_"))<< time2;
    outStream<< ")"<<" ("<<numCustomersFirstLight<<","<<numCustomersSecondLight<<")";
    outStream<<" ("<<nextRegular<<","<<nextProlongation<<")";
    outStream <<" ["<<timeDuration<<"]"<<endl;
}


const void QueueState::Print()
{
    cout<< "Queue state: (";
    cout << firstLightPrimary<<", "<<secondLightSecondary<<", "
         << secondLightPrimary<<", "<< midleQueue;
    cout<< ")"<<endl;
}
QueueState::QueueState(bool Rand)
{
    if (Rand)
    {
        uniform_int_distribution<int> distribution(0, UPPER_BOUND);
        firstLightPrimary = distribution(PrimaryFlowDistribution::generator);
        secondLightSecondary = distribution(PrimaryFlowDistribution::generator);
        secondLightPrimary = distribution(PrimaryFlowDistribution::generator);
        midleQueue = distribution(PrimaryFlowDistribution::generator);
    }
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

void SystemState::Print(ostream& outStream)
{
    serverState.Print(outStream);
    queueState.Print();
}


