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


  if (serverState.state2 == LowPriority)
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
      for (int i = 0; i < temp_count; i++)
  	{
	  Customer customerToRemove = secondLightLowPriorityQueue.front();
	  customerToRemove.serviceTime = std::max(currentTime,customerToRemove.arrivalTime);
	  customerToRemove.departureTime = currentTime + timeToService;
	  stats.AddSecondCustomer(customerToRemove);
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

      firstTimeUntilServ.UpdateMean();
      firstTimeServ.UpdateMean();
      secondTimeUntilServ.UpdateMean();
      secondTimeServ.UpdateMean();
      
      firstPrimary.UpdateMean();
      secondHigh.UpdateMean();
      secondLow.UpdateMean();
      middle.UpdateMean();

      if (stationaryMode)
	{
	  ofstream file_(sai.stationaryFileMeans, ofstream::out | ofstream::app );
	  DumpStatsMean(file_); file_<<endl;
	}
      
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

// void Statistics::DumpDepartQueues()
// {
//   ofstream file(sai.filename, ofstream::out | ofstream::app );
//   file << "1 Light:"<<endl;
//   for (auto& a:departFirstQueue)
//     {
//       a.Dump(file);
//     }
  
//   file << "2 Light:"<<endl;
//   for (auto& a:departSecondQueue)
//     {
//       a.Dump(file);
//     }
// }

void Statistics::AddFirstCustomer(Customer cust)
{
  firstTimeUntilServ.values.push_back(cust.serviceTime - cust.arrivalTime);
  firstTimeServ.values.push_back(cust.departureTime - cust.serviceTime);
  departFirstQueue.push_back(cust);
}
void Statistics::AddSecondCustomer(Customer cust)
{
  secondTimeUntilServ.values.push_back(cust.serviceTime - cust.arrivalTime);
  secondTimeServ.values.push_back(cust.departureTime - cust.serviceTime);
  departSecondQueue.push_back(cust);
}

void Statistics::AddStatistics(Statistics& s)
{
  firstTimeUntilServ.AddMeans(s.firstTimeUntilServ);
  firstTimeServ.AddMeans(s.firstTimeServ);
  secondTimeUntilServ.AddMeans(s.secondTimeUntilServ);
  secondTimeServ.AddMeans(s.secondTimeServ);

  firstPrimary.AddMeans(s.firstPrimary);
  secondHigh.AddMeans(s.secondHigh);
  secondLow.AddMeans(s.secondLow);
  middle.AddMeans(s.middle);
}

void Statistics::Print()
{
  cout << "FirstTimeUntilServ:";
  firstTimeUntilServ.Print();

  cout << "FirstTimeServ:";
  firstTimeServ.Print();

  cout << "SecondTimeUntilServ:";
  secondTimeUntilServ.Print();

  cout << "SecondTimeServ:";
  secondTimeServ.Print();

  cout << "FirstPrimary queue:";
  firstPrimary.Print();

  cout << "SecondHigh queue:";
  secondHigh.Print();

  cout << "SecondLow queue:";
  secondLow.Print();

  cout << "Middle queue:";
  middle.Print();
}

void Statistics::ClearStatistics()
{
  firstTimeUntilServ.Clear();
  firstTimeServ.Clear();
  secondTimeUntilServ.Clear();
  secondTimeServ.Clear();
  firstPrimary.Clear();
  secondHigh.Clear();
  secondLow.Clear();
  middle.Clear();
}

void Statistics::DumpStatsMean(ofstream& _stream)
{
  firstTimeUntilServ.DumpMeans(_stream);
  firstTimeServ.DumpMeans(_stream);
  secondTimeUntilServ.DumpMeans(_stream);
  secondTimeServ.DumpMeans(_stream);
  
  firstPrimary.DumpMeans(_stream);
  secondHigh.DumpMeans(_stream);
  secondLow.DumpMeans(_stream);
  middle.DumpMeans(_stream);
}

void Statistics::DumpStatsMeanDiffs(ofstream& _stream)
{
  firstTimeUntilServ.DumpMeansDiffs(_stream);
  firstTimeServ.DumpMeansDiffs(_stream);
  secondTimeUntilServ.DumpMeansDiffs(_stream);
  secondTimeServ.DumpMeansDiffs(_stream);
  
  firstPrimary.DumpMeansDiffs(_stream);
  secondHigh.DumpMeansDiffs(_stream);
  secondLow.DumpMeansDiffs(_stream);
  middle.DumpMeansDiffs(_stream);
}

void Statistics::DumpStatsStd(ofstream& _stream)
{
  firstTimeUntilServ.DumpStd(_stream);
  firstTimeServ.DumpStd(_stream);
  secondTimeUntilServ.DumpStd(_stream);
  secondTimeServ.DumpStd(_stream);
  
  firstPrimary.DumpStd(_stream);
  secondHigh.DumpStd(_stream);
  secondLow.DumpStd(_stream);
  middle.DumpStd(_stream);
}


void Statistics::DumpStatsStdDiffs(ofstream& _stream)
{
  firstTimeUntilServ.DumpStdDiffs(_stream);
  firstTimeServ.DumpStdDiffs(_stream);
  secondTimeUntilServ.DumpStdDiffs(_stream);
  secondTimeServ.DumpStdDiffs(_stream);
  
  firstPrimary.DumpStdDiffs(_stream);
  secondHigh.DumpStdDiffs(_stream);
  secondLow.DumpStdDiffs(_stream);
  middle.DumpStdDiffs(_stream);
}


void MyMean::UpdateMean()
{
  double old_sum = mean,
    old_sum_sq = mean_sq;
  
  double sum = mean * num,
    sum_sq = mean_sq * num;

  for (auto& a: values)
    {
      sum += a;
      sum_sq += a*a;
    }
  num += values.size();
  if (num > 0)
    {
      mean = sum/num;
      mean_sq = sum_sq/num;
      values.clear();
    }

  if (old_sum > 0)
    diff = std::abs(old_sum - mean)/old_sum;
  else
    diff = 1.;
    
  double old_std = std::sqrt(double(old_sum_sq - old_sum * old_sum)),
    new_std = std::sqrt(double(mean_sq - mean*mean));

  if (old_std > 0)
    diff_std = std::abs(old_std - new_std)/double(old_std);
  else
    diff_std = 1.;
}

void MyMean::AddMeans(MyMean& m)
{
  double old_sum = mean,
    old_sum_sq = mean_sq;
  
  this->UpdateMean();
  m.UpdateMean();
  
  mean = mean * num + m.mean * m.num;
  mean_sq = mean_sq * num + m.mean_sq * m.num;
  num += m.num;
  if (num > 0)
    {
      mean /= num;
      mean_sq /= num;
    }
  
  if (old_sum > 0)
    diff = std::abs(old_sum - mean)/old_sum;
  else
    diff = 1.;
  
  double old_std = std::sqrt(double(old_sum_sq - old_sum * old_sum)),
    new_std = std::sqrt(double(mean_sq - mean*mean));

  if (old_std > 0)
    diff_std = std::abs(old_std - new_std)/double(old_std);
  else
    diff_std = 1.;
}


