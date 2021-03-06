#include "queue.h"
#include <fstream>
#include <cmath>
#include <random>
#include "flow.h"
using namespace std;

long long Customer::count = 0;

mt19937 PrimaryFlowDistribution::generator;

Queue::Queue(QueueState initialState, SystemAprioriInfo _sai): sai(_sai), stats(_sai)
{
    for (int i = 0; i < initialState.firstLightPrimary; i++)
    {
        firstLightPrimaryQueue.push(Customer(0));
    }
    for (int i = 0; i < initialState.secondLightSecondary; i++)
    {
        secondLightHighPriorityQueue.push(Customer(0));
    }
    for (int i = 0; i < initialState.secondLightPrimary; i++)
    {
        secondLightLowPriorityQueue.push(Customer(0));
    }
    for (int i = 0; i < initialState.midleQueue; i++)
    {
        midleQueue.push_back(Customer(0));
    }

    if (sai.verbose)
        PrintState(cout);
}

void Queue::PrintState(ostream& outStream)
{
    outStream<<"Queue state: ["<<firstLightPrimaryQueue.size()<<", "
        <<secondLightHighPriorityQueue.size()<<", "
        <<secondLightLowPriorityQueue.size()<<", "
        <<midleQueue.size()<<"]"<<endl;
}

// void Queue::PrintStatistics()
// {
//   cout<<"Stationary mode"<<endl;
//   cout << "First queue: ("<<stats.stationaryMeanTime_first.mean_untilService<<","<<stats.stationaryMeanTime_first.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_first.numServ<<endl;
//   cout << "Second queue: ("<<stats.stationaryMeanTime_second.mean_untilService<<","<<stats.stationaryMeanTime_second.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_second.numServ<<endl;
// }

void Queue::ServiceMidleQueue(ServerState serverState)
{
    uniform_real_distribution<float> distribution(0., 1.);
    int queueSize = midleQueue.size();
    float generated;
    for (auto a = midleQueue.begin(); a != midleQueue.end();)
    {
        generated = distribution(PrimaryFlowDistribution::generator);
        if (generated <= serverState.midleSuccProb)
	{
            secondLightHighPriorityQueue.push(*a);
            a = midleQueue.erase(a);
	}
        else
	{
            a++;
	}
    }
}

void Queue::MakeIteration(ServerState prevServerState, ServerState serverState, int currentTime, int iteration)
{
    // if (!stats.stationaryMode || (iteration % 10) == 0 )
    // if (!stats.stationaryMode || (iteration % 10) == 0 )

    if (prevServerState.state2 != serverState.state2)
    {
        stats.timesLocate[serverState.state2]++;
        if (prevServerState.state2 == HighPriority)
        {
            stats.loadStatistics.prolNum++;
        }
        if (serverState.state2 == Prolongation)
            stats.loadStatistics.prolTime += serverState.timeDuration;
    }
    stats.timesLocateTimes[serverState.state2] += serverState.timeDuration;
    
    if (prevServerState.state2 != LowPriority &&  serverState.state2 == LowPriority)
        stats.secondLow.values.push_back(secondLightLowPriorityQueue.size());
    if (prevServerState.state2 == LowPriority &&  serverState.state2 != LowPriority)
        stats.secondHigh.values.push_back(secondLightHighPriorityQueue.size());
      
    if (prevServerState.state1 != Primary  &&  serverState.state1 == Primary)
    {
        stats.firstPrimary.values.push_back(firstLightPrimaryQueue.size());
        stats.middle.values.push_back(midleQueue.size());
    }

    UpdateQueues(serverState, currentTime);

    int timeToService = serverState.timeDuration;
    int firstLightCustomersToServe = timeToService;
    int secondLightCustomersToServe = timeToService;
    int midleQueueToServe;

    firstLightCustomersToServe *= (serverState.state1 == Primary ? sai.fls.primaryIntensity : 0);
    secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sai.sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );


    uniform_int_distribution<int> distribution(0, timeToService);
    stats.loadStatistics.inputNumLow += sai.secondFlow.totalLambda * timeToService;
    stats.loadStatistics.inputNumHigh += sai.firstFlow.totalLambda * timeToService;
    if (serverState.state2 == LowPriority)
    {
        stats.loadStatistics.theoreticalNumLow += secondLightCustomersToServe;
        int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
        stats.outputThirdCust += temp_count;
        for (int i = 0; i < temp_count; i++)
  	{
            Customer customerToRemove = secondLightLowPriorityQueue.front();
            int localService = distribution(PrimaryFlowDistribution::generator);
            customerToRemove.serviceTime = std::max(currentTime + localService, customerToRemove.arrivalTime);
            customerToRemove.departureTime = currentTime + timeToService;

            stats.AddSecondCustomer(customerToRemove);
            secondLightLowPriorityQueue.pop();
  	}
    }
    else
    {
        stats.loadStatistics.theoreticalNumHigh += secondLightCustomersToServe;
        int temp_count = std::min(secondLightCustomersToServe,(int)secondLightHighPriorityQueue.size() );
        stats.outputFirstCust += temp_count;
        for (int i = 0; i < temp_count; i++)
  	{
            Customer customerToRemove = secondLightHighPriorityQueue.front();
            customerToRemove.departureTime=currentTime + timeToService;
            secondLightHighPriorityQueue.pop();
            stats.AddFirstCustomer(customerToRemove);
  	}
    }
    ServiceMidleQueue(serverState);
    if (serverState.state1 == Primary)
    {
        int temp_count = std::min(firstLightCustomersToServe,(int)firstLightPrimaryQueue.size() );
        for (int i =0; i < temp_count; i++)
  	{
            Customer customerToMove = firstLightPrimaryQueue.front();
            int localService;

            if (timeToService - std::max(currentTime, customerToMove.arrivalTime) + currentTime == 0)
                localService = 0;
            else
                localService = distribution(PrimaryFlowDistribution::generator) %
                    (timeToService - std::max(currentTime, customerToMove.arrivalTime) + currentTime);

            firstLightPrimaryQueue.pop();
            customerToMove.serviceTime = std::max(currentTime, customerToMove.arrivalTime) + localService;
            static int count = 0;
            count++;
            // if (count %10000 == 0)
            //     cout <<"first until "<< customerToMove.serviceTime - customerToMove.arrivalTime<<"("<<(timeToService - std::max(currentTime, customerToMove.arrivalTime) + currentTime) <<")"<<endl;
            midleQueue.push_back(customerToMove);
  	}
    }
    if ( (iteration + 1) % stats.GRAN == 0)
    {
        stats.UpdateStatistics(iteration);
    }
}

int Queue::GenerateBatches(float lambda, int timeToService)
{
    float mean = lambda * timeToService;
  
    poisson_distribution<int> distribution(mean);
  
    return distribution(PrimaryFlowDistribution::generator);
}

int Queue::GenerateCustomersInBatch(PrimaryFlowDistribution flow)
{
    discrete_distribution<int> distribution(flow.probabilities.begin(), flow.probabilities.end());
    return (distribution(PrimaryFlowDistribution::generator) + 1);
}

void Queue::UpdateQueues(ServerState serverState, int currentTime)
{
    int timeToService = serverState.timeDuration;
    int firstLightBatches = GenerateBatches(sai.firstFlow.lambda, timeToService),
        secondLightBatches = GenerateBatches(sai.secondFlow.lambda, timeToService),
        custInBatch,
        realTime;
    vector<double> tmpAr(firstLightBatches + secondLightBatches);

    uniform_real_distribution<double> realDistribution(0.0, 1.0);
    
    double sum = 0;
    for (int i = 0; i < firstLightBatches; i++)
    {
        tmpAr[i] = sum = sum - log(realDistribution(PrimaryFlowDistribution::generator));
    }
    sum = sum - log(realDistribution(PrimaryFlowDistribution::generator));
    for (int i = 0; i < firstLightBatches; i++)
    {
        tmpAr[i] /= sum;
    }

    sum = 0;
    for (int i = firstLightBatches; i < firstLightBatches + secondLightBatches; i++)
    {
        tmpAr[i] = sum = sum - log(realDistribution(PrimaryFlowDistribution::generator));
    }
    sum = sum - log(realDistribution(PrimaryFlowDistribution::generator));
    for (int i = firstLightBatches; i < firstLightBatches + secondLightBatches; i++)
    {
        tmpAr[i] /= sum;
    }

    // uniform_int_distribution<int> distribution(currentTime, currentTime + timeToService);
    
    for (int i = 0; i < firstLightBatches; i++)
    {
        custInBatch = GenerateCustomersInBatch(sai.firstFlow);
        stats.inputFirstCust += custInBatch;
        realTime = currentTime + timeToService*tmpAr[i];
        // realTime = distribution(PrimaryFlowDistribution::generator);
        for (int j = 0; j < custInBatch; j++)
  	{
            firstLightPrimaryQueue.push(Customer(realTime));
  	}
    }

    for (int i = 0; i < secondLightBatches; i++)
    {
        custInBatch = GenerateCustomersInBatch(sai.secondFlow);
        stats.inputThirdCust += custInBatch;
        realTime = currentTime + timeToService*tmpAr[i + firstLightBatches];
        // realTime = distribution(PrimaryFlowDistribution::generator);
        for (int j = 0; j < custInBatch; j++)
  	{
            secondLightLowPriorityQueue.push(Customer(realTime));
  	}
    }
}

