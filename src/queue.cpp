#include "queue.h"
#include <fstream>
using namespace std;

long long Customer::count = 0;

Queue::Queue(QueueState initialState, SystemAprioriInfo _sai): sai(_sai), stats(_sai)
{
  midleQueueSuccProb = sai.midleQueueSuccProb;
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
    PrintState();
}

void Queue::PrintState()
{
  cout<<"Queue state: ["<<firstLightPrimaryQueue.size()<<", "
      <<secondLightHighPriorityQueue.size()<<", "
      <<secondLightLowPriorityQueue.size()<<", "
      <<midleQueue.size()<<"]"<<endl;
}

void Queue::ServiceMidleQueue()
{
  int queueSize = midleQueue.size();
  float generated;
  for (auto a = midleQueue.begin(); a != midleQueue.end();)
    {
      generated = float(rand()) / RAND_MAX;
      if (generated <= midleQueueSuccProb)
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


void Queue::MakeIteration(ServerState serverState, int currentTime, int iteration)
{
  UpdateQueues(serverState, currentTime);

  int timeToService = serverState.timeDuration;
  int firstLightCustomersToServe = timeToService;
  int secondLightCustomersToServe = timeToService;
  int midleQueueToServe;

  firstLightCustomersToServe *= (serverState.state1 == Primary ? sai.fls.primaryIntensity : 0);
  secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sai.sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );

  if (serverState.state2 == LowPriority)
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
      for (int i = 0; i < temp_count; i++)
  	{
	  Customer customerToRemove = secondLightLowPriorityQueue.front();
	  customerToRemove.serviceTime = std::max(currentTime,customerToRemove.arrivalTime);
	  customerToRemove.departureTime = currentTime + timeToService;
	  stats.departSecondQueue.push(customerToRemove);
  	  secondLightLowPriorityQueue.pop();
  	}
    }
  else
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightHighPriorityQueue.size() );
      for (int i = 0; i < temp_count; i++)
  	{
	  Customer customerToRemove = secondLightHighPriorityQueue.front();
	  customerToRemove.departureTime=currentTime + timeToService;
  	  secondLightHighPriorityQueue.pop();
	  stats.departFirstQueue.push(customerToRemove);
  	}
    }
  ServiceMidleQueue();
  if (serverState.state1 == Primary)
    {
      int temp_count = std::min(firstLightCustomersToServe,(int)firstLightPrimaryQueue.size() );
      for (int i =0; i < temp_count; i++)
  	{
	  Customer customerToMove = firstLightPrimaryQueue.front();
  	  firstLightPrimaryQueue.pop();
	  customerToMove.serviceTime = std::max(currentTime, customerToMove.arrivalTime);
  	  midleQueue.push_back(customerToMove);
  	}
    }
  stats.UpdateStatistics(iteration);
}

int Queue::GenerateCustomersInBatch(PrimaryFlowDistribution flow)
{
  float generated = float(rand()) / RAND_MAX;
  int idx = 0;
  float sum = flow.probabilities[idx++];
  while (sum < 1 - 0.00001 && idx < flow.probabilities.size())
    {
      if (generated <= sum)
	{
	  return idx;
	}
      sum += flow.probabilities[idx++];
    }
  return idx;
}

void Queue::UpdateQueues(ServerState serverState, int currentTime)
{
  // int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
  int timeToService = serverState.timeDuration,
    firstLightBatches = (sai.firstFlow.lambda * timeToService),
    secondLightBatches = (sai.secondFlow.lambda * timeToService),
    custInBatch,
    realTime;
  
    
  for (int i = 0; i < firstLightBatches; i++)
    {
      custInBatch = GenerateCustomersInBatch(sai.firstFlow);
      realTime = currentTime + (rand() % timeToService);
      for (int j = 0; j < custInBatch; j++)
  	{
	  firstLightPrimaryQueue.push(Customer(realTime));
  	}
    }

  for (int i = 0; i < secondLightBatches; i++)
    {
      custInBatch = GenerateCustomersInBatch(sai.secondFlow);
      realTime = currentTime + (rand() % timeToService);
      for (int j = 0; j < custInBatch; j++)
  	{
  	  secondLightLowPriorityQueue.push(Customer(realTime));
  	}
    }
}

void Statistics::UpdateStatistics(int iteration)
{
  if ((iteration > 0) &&  (iteration % GRAN == 0))
    {
      int oldBeforeServiceFirst = beforeServiceTimeFirst;
      int oldBeforeServiceSecond = beforeServiceTimeSecond;
      UpdateMeanTimes();
      //      if (!(stationaryModeFirst && stationaryModeSecond))
	{
	  if (std::abs(oldBeforeServiceFirst - beforeServiceTimeFirst) <= 2 && beforeServiceTimeFirst > 0)
	    stationaryModeFirst = true;
	  else
	    stationaryModeFirst = false;
	  if (std::abs(oldBeforeServiceSecond - beforeServiceTimeSecond) <= 2 && beforeServiceTimeSecond)
	    stationaryModeSecond = true;
	  else
	    stationaryModeSecond = false;
	  DumpMeanTimes();
	  // if (stationaryModeFirst && stationaryModeSecond)
	  //   {
	  //   cout <<"stationary reached at "<<iteration<<" iteration"<<endl;
	  //   cout << oldBeforeServiceSecond<< " vs "<< beforeServiceTimeSecond<< " and "<< oldBeforeServiceFirst << " vs "<< beforeServiceTimeFirst<<endl;
	  //   }
	}
    }

}

void Statistics::DumpMeanTimes()
{
  ofstream file(sai.filename, ofstream::out | ofstream::app );
  
  file << "("<<beforeServiceTimeFirst<<","<<beforeServiceTimeSecond<<")"<<"("<<stationaryModeFirst<<","<<stationaryModeSecond <<")"<<endl;
}

void Statistics::DumpDepartQueues()
{
  ofstream file(sai.filename, ofstream::out | ofstream::app );
  file << "1 Light:"<<endl;
  while (!departFirstQueue.empty())
    {
      departFirstQueue.front().Dump(file);
      departFirstQueue.pop();
    }
  
  file << "2 Light:"<<endl;
  while (!departSecondQueue.empty())
    {
      departSecondQueue.front().Dump(file);
      departSecondQueue.pop();
    }
}


void Statistics::UpdateMeanTimes()
{
  long long sum = 0;
  int firstSize = departFirstQueue.size(),
    secondSize = departSecondQueue.size();
  
  while (!departFirstQueue.empty())
    {
      Customer cust = departFirstQueue.front();
      sum += (cust.serviceTime-cust.arrivalTime);
      departFirstQueue.pop();
    }
  beforeServiceTimeFirst = (firstSize > 0 ? sum/firstSize : 0);
  
  sum = 0;
  while (!departSecondQueue.empty())
    {
      Customer cust = departSecondQueue.front();
      sum += (cust.serviceTime-cust.arrivalTime);
      departSecondQueue.pop();
    }
  beforeServiceTimeSecond = (secondSize > 0 ? sum/secondSize : 0);
}

