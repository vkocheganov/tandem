#include "queue.h"


void Queue::Init(QueueState initialState, float succProb)
{
  midleQueueSuccProb = succProb;
  Customer dummy = {0,0};
  for (int i = 0; i < initialState.firstLightPrimary; i++)
    {
      firstLightPrimaryQueue.push(dummy);
    }
  for (int i = 0; i < initialState.secondLightSecondary; i++)
    {
      secondLightHighPriorityQueue.push(dummy);
    }
  for (int i = 0; i < initialState.secondLightPrimary; i++)
    {
      secondLightLowPriorityQueue.push(dummy);
    }
  for (int i = 0; i < initialState.midleQueue; i++)
    {
      midleQueue.push_back(dummy);
    }
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
	  // 	     secondLightHighPriorityQueue.push(*(midleQueue.erase(a)));
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


void Queue::MakeIteration(SystemAprioriInfo sai, ServerState serverState)
{
  UpdateQueues(sai.firstFlow, sai.secondFlow, serverState);

  int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
  int firstLightCustomersToServe = timeToService;
  int secondLightCustomersToServe = timeToService;
  int midleQueueToServe;

  firstLightCustomersToServe *= (serverState.state1 == Primary ? sai.fls.primaryIntensity : 0);
  secondLightCustomersToServe *= (serverState.state2 == LowPriority ? sai.sls.lowPriorityIntensity : (serverState.state2 == HighPriority ? sai.sls.highPriorityIntensity : sai.sls.prolongationIntensity) );

  if (serverState.state2 == LowPriority)
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightLowPriorityQueue.size() );
      //	cout<<"Second: LowPriority serving: "<<temp_count<<endl;
      for (int i = 0; i < temp_count; i++)
	{
	  secondLightLowPriorityQueue.pop();
	  //temp
	}
    }
  else
    {
      int temp_count = std::min(secondLightCustomersToServe,(int)secondLightHighPriorityQueue.size() );
      //	cout<<"Second: High Priority serving: "<<temp_count<<endl;
      for (int i = 0; i < temp_count; i++)
	{
	  secondLightHighPriorityQueue.pop();
	  //temp
	}
    }
  ServiceMidleQueue();
  if (serverState.state1 == Primary)
    {
      //	cout<<"First: Primary serving: "<<firstLightCustomersToServe<<endl;
      int temp_count = std::min(firstLightCustomersToServe,(int)firstLightPrimaryQueue.size() );
      for (int i =0; i < temp_count; i++)
	{
	  midleQueue.push_back(firstLightPrimaryQueue.front());
	  firstLightPrimaryQueue.pop();
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

void Queue::UpdateQueues(PrimaryFlowDistribution firstFlow, PrimaryFlowDistribution secondFlow, ServerState serverState)
{
  int timeToService = (serverState.time1 < serverState.time2 ? serverState.time1 : serverState.time2);
  int firstLightBatches = (firstFlow.lambda * timeToService);
  int secondLightBatches = (secondFlow.lambda * timeToService);

  Customer dummy = {0,0};
    
  for (int i = 0; i < firstLightBatches; i++)
    {
      int custInBatch = GenerateCustomersInBatch(firstFlow);
      for (int j = 0; j < custInBatch; j++)
	{
	  firstLightPrimaryQueue.push(dummy);
	}
    }

  for (int i = 0; i < secondLightBatches; i++)
    {
      int custInBatch = GenerateCustomersInBatch(secondFlow);
      for (int j = 0; j < custInBatch; j++)
	{
	  secondLightLowPriorityQueue.push(dummy);
	}
    }
}
