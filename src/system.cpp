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
}

void System::MakeIteration(int iteration)
{
    int prevTotalTime = sQueue.stats.timeTotal;
    sQueue.stats.timeTotal += server.MakeIteration(sQueue.secondLightLowPriorityQueue.size(), iteration);
    sQueue.MakeIteration(server.allStates[server.lastState], server.allStates[server.state], prevTotalTime, iteration);
}

void System::Print()
{
    server.Print();
    sQueue.PrintState();
}

bool System::CheckStationaryMode(System& sys, int iteration)
{
    float diff1 = std::abs(sys.sQueue.stats.firstTimeUntilServ.mean - this->sQueue.stats.firstTimeUntilServ.mean)/float(this->sQueue.stats.firstTimeUntilServ.mean),
        diff2 = std::abs(sys.sQueue.stats.secondTimeUntilServ.mean - this->sQueue.stats.secondTimeUntilServ.mean)/float(this->sQueue.stats.secondTimeUntilServ.mean);
    
    if (!this->sQueue.stats.stationaryMode && 
        diff1 < this->sQueue.stats.RATIO_CHANGE &&
        diff2 < this->sQueue.stats.RATIO_CHANGE
        )
    {
        this->sQueue.stats.stationaryMode = true;
        this->sQueue.stats.ClearStatistics();
        cout <<"stationary mode! "<<iteration<<endl;
        // cout <<"("<<diff1<<","<<diff2<<")"<<endl;
    }
    // if (iteration % this->sQueue.stats.GRAN == 0)
    //     cout <<"("<<diff1<<","<<diff2<<")"<<endl;
    
    // cout <<std::max(diff1,diff2)<<endl;
    // if ((iteration+1) % sQueue.stats.GRAN == 0)
    //   cout <<"diff1 = "<<diff1<<"("<<this->sQueue.stats.secondTimeUntilServ.mean <<")"<< "   |  diff2 = " <<diff2<<endl;
    return this->sQueue.stats.stationaryMode;
}

bool System::StopCriteria()
{
    if (!this->sQueue.stats.stationaryMode)
        return false;
}
