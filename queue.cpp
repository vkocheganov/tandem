#include "queue.h"
#include <fstream>
using namespace std;

long long Customer::count = 0;



Queue::Queue(QueueState initialState, SystemAprioriInfo _sai): sai(_sai)
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
      //	 cout<<"generated = "<<generated<<" vs "<<midleQueueSuccProb<<endl;
      if (generated <= midleQueueSuccProb)
	{
	  secondLightHighPriorityQueue.push(*a);
	  //	     cout <<"erasing ";
	  //	     a->Print();
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
  UpdateQueues(sai.firstFlow, sai.secondFlow, serverState, currentTime);

  int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
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
	  customerToRemove.serviceTime = currentTime;
	  customerToRemove.departureTime = currentTime + timeToService;
	  departSecondQueue.push(customerToRemove);
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
	  departFirstQueue.push(customerToRemove);
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
	  customerToMove.serviceTime = currentTime;
  	  midleQueue.push_back(customerToMove);
  	}
    }
  //  if (departFirstQueue.size() + departSecondQueue.size() > 200)
  const int GRAN=10;
  if ((iteration > 0) &&  (iteration % GRAN == 0))
    {
      int oldUntilServiceFirst = untilServiceTimeFirst;
      int oldUntilServiceSecond = untilServiceTimeSecond;
      UpdateMeanTimes();
      if (!(stationaryModeFirst && stationaryModeSecond))
	{
	  if (std::abs(oldUntilServiceFirst - untilServiceTimeFirst) <= 2 && untilServiceTimeFirst > 0)
	    stationaryModeFirst = true;
	  else
	    stationaryModeFirst = false;
	  if (std::abs(oldUntilServiceSecond - untilServiceTimeSecond) <= 2 && untilServiceTimeSecond)
	    stationaryModeSecond = true;
	  else
	    stationaryModeSecond = false;
	  if (stationaryModeFirst && stationaryModeSecond)
	    {
	    cout <<"stationary reached at "<<iteration<<" iteration"<<endl;
	    cout << oldUntilServiceSecond<< " vs "<< untilServiceTimeSecond<< " and "<< oldUntilServiceFirst << " vs "<< untilServiceTimeFirst<<endl;
	    }
	}
    }
}

int Queue::GenerateCustomersInBatch(PrimaryFlowDistribution flow)
{
  float generated = float(rand()) / RAND_MAX;
  int idx = 0;
  float sum = flow.probabilities[idx++];
  while (sum < 1 - 0.00001)
    {
      if (generated <= sum)
	{
	  return idx;
	}
      sum += flow.probabilities[idx++];
    }
  return idx;
}

void Queue::UpdateQueues(PrimaryFlowDistribution firstFlow, PrimaryFlowDistribution secondFlow, ServerState serverState, int currentTime)
{
  int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
  int firstLightBatches = (firstFlow.lambda * timeToService);
  int secondLightBatches = (secondFlow.lambda * timeToService);
  
    
  for (int i = 0; i < firstLightBatches; i++)
    {
      int custInBatch = GenerateCustomersInBatch(firstFlow);
      for (int j = 0; j < custInBatch; j++)
  	{
	  firstLightPrimaryQueue.push(Customer(currentTime));
  	}
    }

  for (int i = 0; i < secondLightBatches; i++)
    {
      int custInBatch = GenerateCustomersInBatch(secondFlow);
      for (int j = 0; j < custInBatch; j++)
  	{
  	  secondLightLowPriorityQueue.push(Customer(currentTime));
  	}
    }
}

void Queue::DumpDepartQueues()
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


void Queue::UpdateMeanTimes()
{
  ofstream file(sai.filename, ofstream::out | ofstream::app );
  long long sum = 0;
  int firstSize = departFirstQueue.size(),
    secondSize = departSecondQueue.size();
  
  while (!departFirstQueue.empty())
    {
      Customer cust = departFirstQueue.front();
      sum += (cust.serviceTime-cust.arrivalTime);
      departFirstQueue.pop();
    }
  if (firstSize > 0)
    {
      untilServiceTimeFirst = sum/firstSize;
      file << "1 Light:"<<untilServiceTimeFirst<<endl;
    }
  
  sum = 0;
  while (!departSecondQueue.empty())
    {
      Customer cust = departSecondQueue.front();
      sum += (cust.serviceTime-cust.arrivalTime);
      departSecondQueue.pop();
    }
  if (secondSize > 0)
    {
      untilServiceTimeSecond = sum/secondSize;
      file << "2 Light:"<<(sum/secondSize)<<endl;
    }
}

