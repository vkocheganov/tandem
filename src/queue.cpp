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
  cout << "First queue: ("<<stats.stationaryMeanTime_first.mean_untilService<<","<<stats.stationaryMeanTime_first.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_first.numServ<<endl;
  cout << "Second queue: ("<<stats.stationaryMeanTime_second.mean_untilService<<","<<stats.stationaryMeanTime_second.mean_Service<<")"<<" num="<<stats.stationaryMeanTime_second.numServ<<endl;
}

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

void Queue::MakeIteration(ServerState serverState, int currentTime, int iteration)
{
  UpdateQueues(serverState, currentTime);

  int timeToService = serverState.timeDuration;
  int firstLightCustomersToServe = timeToService;
  int secondLightCustomersToServe = timeToService;
  int midleQueueToServe;

  firstLightCustomersToServe *= (serverState.state1 == Primary ? sai.fls.primaryIntensity : 0);
  secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sai.sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );

  if (serverState.nextProlongation != -1)
    {
      stats.firstPrimarySizes.values.push_back(firstLightPrimaryQueue.size());
      stats.secondHighPrioritySizes.values.push_back(secondLightHighPriorityQueue.size());
      stats.secondLowPrioritySizes.values.push_back(secondLightLowPriorityQueue.size());
      stats.middleSizes.values.push_back(midleQueue.size());
    }

  if (serverState.state2 == LowPriority)
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
      for (int i = 0; i < temp_count; i++)
  	{
	  Customer customerToRemove = secondLightLowPriorityQueue.front();
	  customerToRemove.serviceTime = std::max(currentTime,customerToRemove.arrivalTime);
	  customerToRemove.departureTime = currentTime + timeToService;
	  stats.AddSecondCustomer(customerToRemove);
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
	  stats.AddFirstCustomer(customerToRemove);
	  stats.departFirstQueue.push_back(customerToRemove);
  	}
    }
  ServiceMidleQueue(serverState);
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

  uniform_int_distribution<int> distribution(currentTime, currentTime + timeToService);
    
  for (int i = 0; i < firstLightBatches; i++)
    {
      custInBatch = GenerateCustomersInBatch(sai.firstFlow);
      realTime = distribution(PrimaryFlowDistribution::generator);
      for (int j = 0; j < custInBatch; j++)
  	{
	  firstLightPrimaryQueue.push(Customer(realTime));
  	}
    }

  for (int i = 0; i < secondLightBatches; i++)
    {
      custInBatch = GenerateCustomersInBatch(sai.secondFlow);
      realTime = distribution(PrimaryFlowDistribution::generator);
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
      if (sai.verbose)
	{
	  DumpAllCustomers();
	}

      stationaryMeanTime_first.UpdateMean(departFirstQueue, stationaryMode);
      stationaryMeanTime_second.UpdateMean(departSecondQueue, stationaryMode);
      firstPrimary.UpdateMean(firstPrimarySizes);
      secondHigh.UpdateMean(secondHighPrioritySizes);
      secondLow.UpdateMean(secondLowPrioritySizes);
      middle.UpdateMean(middleSizes);
      // if (sai.verbose)
      // 	cout <<"UntilServ time = "<<stationaryMeanTime_first.mean_untilService<<" "<<stationaryMeanTime_second.mean_untilService<<endl;

    }
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

void Statistics::AddFirstCustomer(Customer cust)
{
  firstTimeUntilServ.values.push_back(cust.serviceTime - cust.arrivalTime);
  firstTimeServ.values.push_back(cust.departureTime - cust.serviceTime);
}
void Statistics::AddSecondCustomer(Customer cust)
{
  secondTimeUntilServ.values.push_back(cust.serviceTime - cust.arrivalTime);
  secondTimeServ.values.push_back(cust.departureTime - cust.serviceTime);
}


void MovingMean::UpdateMean(deque<Customer>& newNumbers, bool stationary)
{
  double sum_until = mean_untilService * numUntil,
    sum_until_sq = mean_untilService_sq * numUntil,
    sum_service = mean_Service * numServ,
    sum_service_sq = mean_Service_sq * numServ;
  for (auto& a: newNumbers)
    {
      sum_until += (a.serviceTime - a.arrivalTime);
      sum_until_sq += (a.serviceTime - a.arrivalTime)*(a.serviceTime - a.arrivalTime);
      
      sum_service += (a.departureTime - a.arrivalTime);
      sum_service_sq += (a.departureTime - a.arrivalTime) * (a.departureTime - a.arrivalTime);
    }
  numUntil += newNumbers.size();
  mean_untilService = sum_until/numUntil;
  mean_untilService_sq = sum_until_sq/numUntil;

  numServ += newNumbers.size();
  mean_Service = sum_service/numServ;
  mean_Service_sq = sum_service_sq/numServ;
  newNumbers.clear();
}

void MyMean::UpdateMean()
{
  double sum = mean * num,
    sum_sq = mean_sq * num;

  for (auto& a: values)
    {
      sum += a;
      sum_sq += a*a;
    }
  num += values.size();
  mean = sum/num;
  mean_sq = sum_sq/num;
  values.clear();
}
