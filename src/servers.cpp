#include "servers.h"


void GenerateStates(vector<ServerState>& vs, int currentState, SystemAprioriInfo sai);

Server::Server(ServerState initialState, SystemAprioriInfo _sai): sai(_sai)
{
    allStates.push_back(initialState);
    GenerateStates(allStates, 0, sai);
    state = 0;
    prolongationThreshold = sai.prolongThres;

   if (sai.verbose)
    {
        cout <<"find cycles"<< endl;
        int count = 0;
        for (auto a: allStates)
	{
            cout <<"["<<count<<"] ";
            a.Print(cout);
            count++;
	}
    }
}

int Server::MakeIteration(int lowPriorityQueueSize, int )
{
    lastState = state;
    state = ( allStates[state].nextProlongation != -1 && lowPriorityQueueSize <= prolongationThreshold ? allStates[state].nextProlongation :
              (allStates[state].nextRegular == -1 ? allStates[state].nextProlongation : allStates[state].nextRegular)
                  );
    // cout <<lowPriorityQueueSize<<" > "<<prolongationThreshold<<" ? "<<(state == allStates[state].nextProlongation)<<endl;
    if (state == -1)
    {
        cout<<"Error!!!"<<endl;
        exit(1);
    }
    return allStates[state].timeDuration;
}
void Server::Print(ostream& outStream)
{
    allStates[state].Print(outStream);
}

void GenerateStates(vector<ServerState>& vs, int currentState, SystemAprioriInfo sai)
{
    int serverToFinish;
    int temp, timeToFinish;
    int eligibleToProlong;
    ServerState newState;

    int firstLightCustomersToServe;
    int secondLightCustomersToServe;

    // firstLightCustomersToServe *= (serverState.state1 == Primary ? fls.primaryIntensity : 0);
    // secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sls.highPriorityIntensity : sls.prolongationIntensity) );
  
    if (vs[currentState].time1 < vs[currentState].time2)
    {
        serverToFinish = 1;
        timeToFinish = vs[currentState].time1;
    }
    else if (vs[currentState].time1 > vs[currentState].time2)
    {
        serverToFinish = 2;
        timeToFinish = vs[currentState].time2;
    }
    else
    {
        serverToFinish = 0;
        timeToFinish = vs[currentState].time1;
    }
    vs[currentState].timeDuration = timeToFinish;
    vs[currentState].midleSuccProb = sai.GetSuccProb(vs[currentState].timeDuration);

    if (vs[currentState].numCustomersFirstLight < 0 || vs[currentState].numCustomersSecondLight < 0)
    {
        firstLightCustomersToServe = timeToFinish;
        secondLightCustomersToServe = timeToFinish;
        firstLightCustomersToServe *= (vs[currentState].state1 == Primary ? sai.fls.primaryIntensity : 0);
        secondLightCustomersToServe *= (vs[currentState].state2 == LowPriority ? sai.sls.lowPriorityIntensity : (vs[currentState].state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );
        vs[currentState].numCustomersFirstLight = firstLightCustomersToServe;
        vs[currentState].numCustomersSecondLight = secondLightCustomersToServe;
    }

  
    newState.timeDuration = timeToFinish;
    vs[currentState].midleSuccProb = sai.GetSuccProb(newState.timeDuration);
    if (serverToFinish == 1)
    {
        newState.state1 = (vs[currentState].state1 == Primary ? Secondary : Primary);
        newState.time1 = (newState.state1 == Primary ? sai.fls.primaryTime : sai.fls.secondaryTime);
        newState.state2 = vs[currentState].state2;
        newState.time2 = vs[currentState].time2 - timeToFinish;
    }
    else if (serverToFinish == 2)
    {
        newState.state1 = vs[currentState].state1;
        newState.time1 = vs[currentState].time1 - timeToFinish;
        newState.state2 = (vs[currentState].state2  > LowPriority ? LowPriority : HighPriority);
        newState.time2 = (newState.state2 == LowPriority ? sai.sls.lowPriorityTime : sai.sls.highPriorityTime);
    }
    else
    {
        //      cout<<"both to finish"<<endl;
        newState.state1 = (vs[currentState].state1 == Primary ? Secondary : Primary);
        newState.time1 = (newState.state1 == Primary ? sai.fls.primaryTime : sai.fls.secondaryTime);
        newState.state2 = (vs[currentState].state2  > LowPriority ? LowPriority : HighPriority);
        newState.time2 = (newState.state2 == LowPriority ? sai.sls.lowPriorityTime : sai.sls.highPriorityTime);
    }

    bool newStateIsRegular = newState.state2 != Prolongation;
    eligibleToProlong = ((serverToFinish == 0 || serverToFinish == 2) && vs[currentState].state2 > LowPriority ) ||
        (serverToFinish == 1  && vs[currentState].state2 == Prolongation );

    if (newStateIsRegular)
    {
        auto tmp = find(vs.begin(), vs.end(), newState);
        if (tmp == vs.end())
        {
            vs.push_back(newState);
            vs[currentState].nextRegular = vs.size() - 1;
            GenerateStates(vs, vs.size()-1, sai);
        }
        else
        {
            vs[currentState].nextRegular = tmp - vs.begin();
        }
        if (eligibleToProlong)
        {
            newState.state2 = Prolongation;
            newState.time2 = sai.sls.prolongationTime;
            auto tmp = find(vs.begin(), vs.end(), newState);
            if ( tmp == vs.end())
            {
                vs.push_back(newState);
                vs[currentState].nextProlongation = vs.size() - 1;
                GenerateStates(vs, vs.size()-1, sai);
            }
            else
            {
                vs[currentState].nextProlongation = tmp - vs.begin();
            }
        }
    }
    else
    {
        auto tmp = find(vs.begin(), vs.end(), newState);
        if (tmp == vs.end())
        {
            vs.push_back(newState);
            vs[currentState].nextProlongation = vs.size() - 1;
            GenerateStates(vs, vs.size()-1, sai);
        }
        else
        {
            vs[currentState].nextProlongation = tmp - vs.begin();
        }
    }
}


void IterateFindCycles(vector<ServerState>& vs, int currentState, vector<bool>& processed, vector<Cycle>& cycles, int currentCycle)
{
    cycles[currentCycle].idxs.push_back(currentState);
    processed[currentState] = true;
    int nextRegular = vs[currentState].nextRegular;
    int nextProlongation = vs[currentState].nextProlongation;

    if (vs[currentState].state2 != Prolongation)
    {
        if (!processed[nextRegular])
      	{
            IterateFindCycles(vs, nextRegular, processed, cycles, currentCycle);
      	}
        if (nextProlongation != -1 && !processed[nextProlongation])
      	{
            Cycle dummy;
            dummy.isProlongation = true;
            cycles.push_back(dummy);
            IterateFindCycles(vs, nextProlongation, processed, cycles, cycles.size()-1);
      	}
    }
    else
    {
        if (nextProlongation != -1 && !processed[nextProlongation])
      	{
            IterateFindCycles(vs, nextProlongation, processed, cycles, currentCycle);
      	}
        if (nextRegular != -1 && !processed[nextRegular])
      	{
            Cycle dummy;
            dummy.isProlongation = true;
            cycles.push_back(dummy);
            IterateFindCycles(vs, nextRegular, processed, cycles, cycles.size()-1);
      	}
    }
    // cout<<"iterate end"<<endl;
}

vector<Cycle> FindCycles(vector<ServerState> vs, SystemAprioriInfo sai)
{
    vector<bool> processed(vs.size(),0);
    vector<Cycle> cycles;
    Cycle dummy;
    cycles.push_back(dummy);
    IterateFindCycles(vs, 0, processed, cycles, 0);
    for (auto& a:cycles)
        a.CalcStatistics(vs, sai);
    return cycles;
}


void Cycle::CalcStatistics(vector<ServerState>& vs, SystemAprioriInfo sai)
{
    for (auto a:idxs)
    {
        timeDuration += vs[a].timeDuration;
        if (vs[a].state1 == Primary)
	{
            primaryFlowServed += vs[a].numCustomersFirstLight;
	}
        if (vs[a].state2 == LowPriority)
	{
            lowPriorityFlowServed += vs[a].numCustomersSecondLight;
	}
        else
	{
            highPriorityFlowServed += vs[a].numCustomersSecondLight;
	}
    }

    firstLightIncome = timeDuration * sai.firstFlow.totalLambda;
    secondLightIncome = timeDuration * sai.secondFlow.totalLambda;
}

bool Cycle::IsStationar()
{
    bool ret;
    if (timeDuration == 0)
        return true;
    ret = (firstLightIncome < primaryFlowServed);
    ret = ret && (std::min(float(primaryFlowServed),firstLightIncome) < highPriorityFlowServed);

    if (!this->isProlongation)
    {
        ret = ret && (secondLightIncome < lowPriorityFlowServed);
    }
    return ret;
}

void Cycle::Print(ofstream& outStream)
{
    outStream<<"Duration: "<<timeDuration<<"; States: [ ";
    for (auto a:idxs)
    {
        outStream<<a<<" ";
    }
    //  outStream <<"]"<<" FirstLightIncome="<<firstLightIncome<<", SecondLightIncome="<<secondLightIncome<<", PrimaryFirstLight_sum{l}="<<primaryFlowServed<<", LowPriority_sum{l}="<<lowPriorityFlowServed<<endl;
    outStream <<"] isStationar"<<this->IsStationar()<<endl<<
        "      FirstLight: primaryIncome="<<firstLightIncome<<", primaryServed="<<primaryFlowServed<<""<<endl<<
        "      SecondLight: lowPriorityIncome="<<secondLightIncome<<", lowPriorityServed="<<lowPriorityFlowServed<<", highPriorityServed"<<highPriorityFlowServed<<")"<<endl;
}

void Cycle::Print_Ext(ofstream& outStream)
{
    outStream<<"[ ";
    for (auto a:idxs)
    {
        outStream<<a<<" ";
    }
    //  outStream <<"]"<<" FirstLightIncome="<<firstLightIncome<<", SecondLightIncome="<<secondLightIncome<<", PrimaryFirstLight_sum{l}="<<primaryFlowServed<<", LowPriority_sum{l}="<<lowPriorityFlowServed<<endl;
    outStream <<"]"<<" (FirstLightIncome"<<", SecondLightIncome"<<", PrimaryFirstLight_sum{l}"<<", LowPriority_sum{l}, HighPriority_sum{l}) = ("<<firstLightIncome<<","<<primaryFlowServed<<","<<secondLightIncome<<","<<lowPriorityFlowServed<<", "<<highPriorityFlowServed<<")"<<endl;
}





