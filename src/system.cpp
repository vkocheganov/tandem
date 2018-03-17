#include "system.h"

System::System (QueueState initQueueState, ServerState initServerState, SystemAprioriInfo _sai):
  sQueue(initQueueState, _sai), server(initServerState, _sai), sai(_sai)
{
  cycles = FindCycles(server.allStates, sai);
  if (sai.verbose)
    {
      cout <<"Cycles num = "<<cycles.size()<<endl;
      for (auto a:cycles) a.Print();
    }
  timeTotal = 0;
}

void System::MakeIteration(int iteration)
{
  int prevTotalTime = timeTotal;
  timeTotal += server.MakeIteration(sQueue.secondLightLowPriorityQueue.size(), iteration);
  sQueue.MakeIteration(server.allStates[server.state], prevTotalTime, iteration);
}

void System::Print()
{
  server.Print();
  sQueue.PrintState();
  sQueue.PrintStatistics();
}

void System::CheckStationaryMode(System& sys, int iteration)
{
  float diff1 = std::abs(sys.sQueue.stats.stationaryMeanTime_first.mean_untilService - this->sQueue.stats.stationaryMeanTime_first.mean_untilService)/float(this->sQueue.stats.stationaryMeanTime_first.mean_untilService),
    diff2 = std::abs(sys.sQueue.stats.stationaryMeanTime_second.mean_untilService - this->sQueue.stats.stationaryMeanTime_second.mean_untilService)/float(this->sQueue.stats.stationaryMeanTime_second.mean_untilService);
  if (!this->sQueue.stats.stationaryMode && 
      diff1 < this->sQueue.stats.RATIO_CHANGE &&
      diff2 < this->sQueue.stats.RATIO_CHANGE
      )
    {
      this->sQueue.stats.stationaryMode = true;
      cout <<"stationary mode! "<<iteration<<endl;
    }
  if ((iteration+1) % sQueue.stats.GRAN == 0)
    cout <<"diff1 = "<<diff1<<"("<<this->sQueue.stats.stationaryMeanTime_first.mean_untilService <<")"<< "   |  diff2 = " <<diff2<<endl;
}
