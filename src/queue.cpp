#include "queue.h"
#include <fstream>
#include <cmath>
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

void Queue::PrintStatistics()
{
  cout<<"Stationary mode"<<endl;
  cout << "First queue: ("<<stats.stationaryMeanTime_first.mean_untilService<<","<<stats.stationaryMeanTime_first.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_first.num<<endl;
  cout << "Second queue: ("<<stats.stationaryMeanTime_second.mean_untilService<<","<<stats.stationaryMeanTime_second.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_second.num<<endl;
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
	  stats.departSecondQueue.push_back(customerToRemove);
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
	  stats.departFirstQueue.push_back(customerToRemove);
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
  if ( (iteration + 1) % GRAN == 0)
    {
      float oldBeforeServiceFirst = beforeServiceTimeFirst;
      float oldBeforeServiceSecond = beforeServiceTimeSecond;
      UpdateMeanTimes();
      
      if (oldBeforeServiceFirst > 0 && float(std::abs(float(oldBeforeServiceFirst - beforeServiceTimeFirst)))/oldBeforeServiceFirst <= RATIO_CHANGE)
	stationaryModeFirst = true;
      else
	stationaryModeFirst = false;
      if (oldBeforeServiceSecond > 0 && std::abs(oldBeforeServiceSecond - beforeServiceTimeSecond)/oldBeforeServiceSecond <= RATIO_CHANGE)
	stationaryModeSecond = true;
      else
	stationaryModeSecond = false;
      
      DumpMeanTimes();
      DumpAllCustomers();

      if (stationaryMode)
	{
	  stationaryMeanTime_first.UpdateMean(departFirstQueue);
	  stationaryMeanTime_second.UpdateMean(departSecondQueue);
	}

      departFirstQueue.clear();
      departSecondQueue.clear();
      if (!stationaryMode && stationaryModeFirst && stationaryModeSecond)
	{
	  stationaryMode = true;
	}
    }

}

void Statistics::DumpMeanTimes()
{
  ofstream file(sai.filename, ofstream::out | ofstream::app );
  
  file << "("<<beforeServiceTimeFirst<<","<<beforeServiceTimeSecond<<")"<<"("<<stationaryModeFirst<<","<<stationaryModeSecond <<")"<<endl;
}

void Statistics::DumpAllCustomers()
{
  ofstream file1(sai.firstCustomersFile, ofstream::out | ofstream::app );
  ofstream file2(sai.secondCustomersFile, ofstream::out | ofstream::app );

  for (auto& a : departFirstQueue)
    a.Dump(file1);
  for (auto& a : departSecondQueue)
    a.Dump(file2);
}

void Statistics::DumpDepartQueues()
{
  ofstream file(sai.filename, ofstream::out | ofstream::app );
  file << "1 Light:"<<endl;
  for (auto& a:departFirstQueue)
    {
      a.Dump(file);
    }
  
  file << "2 Light:"<<endl;
  for (auto& a:departSecondQueue)
    {
      a.Dump(file);
    }
}


void Statistics::UpdateMeanTimes()
{
  long long sum = 0;
  int firstSize = departFirstQueue.size(),
    secondSize = departSecondQueue.size();
  
  for (auto& a:departFirstQueue)
    {
      sum += (a.serviceTime-a.arrivalTime);
    }
  beforeServiceTimeFirst = (firstSize > 0 ? sum/float(firstSize) : 0);
  
  sum = 0;
  for (auto& a:departSecondQueue)
    {
      sum += (a.serviceTime-a.arrivalTime);
    }
  beforeServiceTimeSecond = (secondSize > 0 ? sum/float(secondSize) : 0);
}

void MovingMean::UpdateMean(deque<Customer>& newNumbers)
{
  double sum_until = mean_untilService * num,
    sum_service = mean_Service * num;
  for (auto& a: newNumbers)
    {
      sum_until += (a.serviceTime - a.arrivalTime);
      sum_service += (a.departureTime - a.arrivalTime);
    }
  num += newNumbers.size();
  mean_untilService = sum_until/num;
  mean_Service = sum_service/num;
}
