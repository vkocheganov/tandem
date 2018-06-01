#include "system.h"

System::System (QueueState initQueueState, ServerState initServerState, SystemAprioriInfo _sai):
    sQueue(initQueueState, _sai), server(initServerState, _sai), sai(_sai)
{
    cycles = FindCycles(server.allStates, sai);

    ofstream saiFile(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    saiFile <<"Cycles num = "<<cycles.size()<<endl;
    for (auto a:cycles) a.Print(saiFile);
}

void System::MakeIteration(int iteration)
{
    int prevTotalTime = sQueue.stats.timeTotal;
    server.allStates[server.state].realResideCount++;
    sQueue.stats.timeTotal += server.MakeIteration(sQueue.secondLightLowPriorityQueue.size(), iteration);
    sQueue.MakeIteration(server.allStates[server.lastState], server.allStates[server.state], prevTotalTime, iteration);
}

void System::Print(ostream& outStream)
{
    server.Print(outStream);
    sQueue.PrintState(outStream);
}

bool System::CheckStationaryMode(System& sys, int iteration)
{
    float diff1 = std::abs(sys.sQueue.stats.firstTimeUntilServ.mean - this->sQueue.stats.firstTimeUntilServ.mean) /this->sQueue.stats.firstTimeUntilServ.mean ,
        diff2 = std::abs(sys.sQueue.stats.secondTimeUntilServ.mean - this->sQueue.stats.secondTimeUntilServ.mean) / this->sQueue.stats.secondTimeUntilServ.mean;

    double inputFirstFlow = double(sQueue.stats.inputFirstCust)/sQueue.stats.timeTotal,
        outputFirstFlow = double(sQueue.stats.outputFirstCust)/sQueue.stats.timeTotal,
        inputThirdFlow = double(sQueue.stats.inputThirdCust)/sQueue.stats.timeTotal,
        outputThirdFlow = double(sQueue.stats.outputThirdCust)/sQueue.stats.timeTotal;

    

    if (!this->sQueue.stats.stationaryMode && 
        diff1 < this->sQueue.stats.RATIO_CHANGE &&
        diff2 < this->sQueue.stats.RATIO_CHANGE &&
        inputFirstFlow < 1.1 * outputFirstFlow &&
        inputThirdFlow < 1.01 * outputThirdFlow
        &&
        sQueue.stats.firstPrimary.CheckErr(0.1) &&
        sQueue.stats.secondHigh.CheckErr(0.1) &&
        sQueue.stats.secondLow.CheckErr(0.1) &&
        sQueue.stats.middle.CheckErr(0.1)
        )
    {
        this->sQueue.stats.stationaryMode = true;
        // this->sQueue.stats.Print();
        this->sQueue.stats.ClearStatistics();
        // cout <<"stationary mode! "<<iteration<<endl;
        // cout <<"("<<diff1<<","<<diff2<<")"<<endl;
    }
    // if (iteration % this->sQueue.stats.GRAN == 0)
    //     cout <<"("<<diff1<<","<<diff2<<")"<<endl;
    
    // cout <<std::max(diff1,diff2)<<endl;
    // if ((iteration+1) % sQueue.stats.GRAN == 0)
    //   cout <<"diff1 = "<<diff1<<"("<<this->sQueue.stats.secondTimeUntilServ.mean <<")"<< "   |  diff2 = " <<diff2<<endl;

    ofstream oFile (this->sQueue.stats.sai.outFiles.stationaryReaching, ofstream::out | ofstream::app);
    oFile << diff1 <<" "<<diff2<<" "
          <<inputFirstFlow<<"/"<<1.1*outputFirstFlow<<" "
          <<inputThirdFlow<<"/"<<1.01*outputThirdFlow<<" ";
    sQueue.stats.firstPrimary.PrintErr(oFile);
    sQueue.stats.secondHigh.PrintErr(oFile);
    sQueue.stats.secondLow.PrintErr(oFile);
    sQueue.stats.middle.PrintErr(oFile);
    oFile<<endl;
    
    
    return this->sQueue.stats.stationaryMode;
}

bool System::StopCriteria()
{
    if (!this->sQueue.stats.stationaryMode)
        return false;
}

bool System::IsStationar()
{
    bool ret = true;
    for (auto& a:cycles)
    {
        ret *= a.IsStationar();
    }
    return ret;
}
