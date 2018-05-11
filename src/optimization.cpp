#include "optimization.h"
#include "system.h"
#include <numeric>
#include <sstream>
#include <time.h>

string RangeIndexesNames[]=
{
    "FIRST_LIGHT_TIME_PRIMARY",
    "FIRST_LIGHT_TIME_SECONDARY",
    "SECOND_LIGHT_TIME_LOW",
    "SECOND_LIGHT_TIME_HIGH",
    "SECOND_LIGHT_TIME_PROLONG",
    "THRESHOLD"
};

template <typename T>
void Range<T>::Print(ostream& outStream)
{
    for (T f = first; f <= last; f += step)
        outStream<<f<<" ";
}

RangeArray::RangeArray(SystemAprioriInfo _baseSai):
    ranges{
    {_baseSai.fls.primaryTime, 1, _baseSai.fls.primaryTime},
    {_baseSai.fls.secondaryTime, 1, _baseSai.fls.secondaryTime},
    {_baseSai.sls.lowPriorityTime, 1, _baseSai.sls.lowPriorityTime},
    {_baseSai.sls.highPriorityTime, 1, _baseSai.sls.highPriorityTime},
    {_baseSai.sls.prolongationTime, 1, _baseSai.sls.prolongationTime},
    {_baseSai.prolongThres, 1, _baseSai.prolongThres}
}, maxIdx{}, currIdxs{},
    currValues{
        _baseSai.fls.primaryTime,
            _baseSai.fls.secondaryTime,
            _baseSai.sls.lowPriorityTime,
            _baseSai.sls.highPriorityTime,
            _baseSai.sls.prolongationTime,
            _baseSai.prolongThres
            },
    arrIdx{}
{
}
    
    void RangeArray::Start()
    {
        unsigned count = 1;
        for (int i = 0; i < RANGE_INDEXES_LAST; i++)
        {
            currValues[i] = ranges[i].first;
            unsigned count_1 = 0;
            for (int i_range = ranges[i].first; i_range <= ranges[i].last; i_range += ranges[i].step)
            {
                count_1++;
            }
            maxIdx[i] = count_1 - 1;
            cout<<"max idx = "<<maxIdx[i]<<endl;
            count *= count_1;
        }
        arr.resize(count);
    }


bool RangeArray::Iterate()
{
    int currIdx = RANGE_INDEXES_LAST - 1;
    while (currIdx >= 0 && currIdxs[currIdx] == maxIdx[currIdx])
    {
        currValues[currIdx] = ranges[currIdx].first;
        currIdxs[currIdx] = 0;
        currIdx--;
    }
    if (currIdx < 0)
        return false;
    arrIdx++;
    currIdxs[currIdx]++;
    currValues[currIdx] += ranges[currIdx].step;

    return true;
}

void RangeArray::PrintArr(ostream& outStream)
{
    unsigned idxs[RANGE_INDEXES_LAST]{};
    int idx = RANGE_INDEXES_LAST - 1;
    bool cont = true;
    bool newLine = false;
    unsigned aIdx = 0;
    

    while (1){
        outStream<< arr[aIdx++]<<" ";
        idx = RANGE_INDEXES_LAST - 1;
        newLine = false;
        while (idx >= 0 && idxs[idx] == maxIdx[idx])
        {
            if (maxIdx[idx] > 0)
                newLine = true;
            idxs[idx] = 0;
            idx--;
        }
        if (idx < 0)
            break;
        idxs[idx]++;
        if (newLine)
        {
            outStream<<endl;
            
            outStream<<"(";
            for (int i = 0; i < RANGE_INDEXES_LAST; i++)
            {
                outStream<<(ranges[i].first + idxs[i]*ranges[i].step)<<" ";
            }
            outStream<<")"<<endl;
        }
        // PrintCurrParams(outStream);
    }
    outStream<<endl;
}





void RangeArray::SetSai(SystemAprioriInfo& sai)
{
    sai.fls.primaryTime = currValues[FIRST_LIGHT_TIME_PRIMARY];
    sai.fls.secondaryTime = currValues[FIRST_LIGHT_TIME_SECONDARY];
    sai.sls.lowPriorityTime = currValues[SECOND_LIGHT_TIME_LOW];
    sai.sls.highPriorityTime = currValues[SECOND_LIGHT_TIME_HIGH];
    sai.sls.prolongationTime = currValues[SECOND_LIGHT_TIME_PROLONG];
    sai.prolongThres = currValues[THRESHOLD];
}

void RangeArray::PrintCurrParams(ostream& outStream)
{
    for (int i = 0; i < RANGE_INDEXES_LAST; i++)
        outStream<<currValues[i]<<" ";
    outStream<<endl;
}

void RangeArray::PrintAllParams(ostream& outStream)
{
    for (int i = 0; i < RANGE_INDEXES_LAST; i++)
    {
        outStream<<RangeIndexesNames[i]<<": ";
        ranges[i].Print(outStream);
        outStream<<endl;
    }
}

Optimization::Optimization(QueueState initQueue, ServerState initServer, SystemAprioriInfo _baseSai):
    initialQueueState(initQueue), initialServerState(initServer), baseSai(_baseSai),
    rangeArray(_baseSai)
{
}


void Optimization::MakeOptimization()
{
    this->DumpParams();
    SystemAprioriInfo sai(baseSai);
    
    this->rangeArray.Start();
    this->rangeArray.PrintArr(cout);

    ofstream saiFile;
    do
    {
        saiFile.open(baseSai.outFiles.saiFile, ofstream::out | ofstream::app);
        this->rangeArray.PrintCurrParams(saiFile);
        saiFile.close();
        this->rangeArray.SetSai(sai);
        this->Iterate(sai);
    } while (this->rangeArray.Iterate());
    
    saiFile.open(baseSai.outFiles.saiFile, ofstream::out | ofstream::app);
    this->rangeArray.PrintArr(saiFile);

    //             time_t rawtime;
    //             struct tm *info;
    //             char tmp_buf[80];
    //             time( &rawtime );
    //             info = localtime( &rawtime );
    //             strftime(tmp_buf,80,"_%Y_%m_%d__%H_%M_%S", info);
    //             cout <<tmp_buf<<endl;
  
    //             sai.sls.lowPriorityTime = sltl;
}

void Optimization::Iterate(SystemAprioriInfo sai)
{
    vector<double> firstService, secondService;
    SystemAprioriInfo refSai(sai);
    refSai.verbose=false;
    QueueState refInitialQueueState(true);
    Statistics aggStats(sai);  
  
    System system(initialQueueState, initialServerState, sai),
        refSystem(refInitialQueueState, initialServerState, refSai);
    bool statSucc = false;

    for (int i = 0; i < sai.numMaxIteration; i++)
    {
        refSystem.MakeIteration(i);
        system.MakeIteration(i);
        if (system.CheckStationaryMode(refSystem,i) && i >= sai.numMaxIteration * 0.1)
        {
            statSucc = true;
            break;
        }
    }
    ofstream saiFile;
    saiFile.open(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    system.sQueue.PrintState(cout);
    system.sQueue.PrintState(saiFile);

    int totalCount = 0, totalTime = 0;
    totalCount = system.sQueue.stats.timesLocate[0] +
        system.sQueue.stats.timesLocate[1] +
        system.sQueue.stats.timesLocate[2];
    totalTime = system.sQueue.stats.timesLocateTimes[0] +
        system.sQueue.stats.timesLocateTimes[1] +
        system.sQueue.stats.timesLocateTimes[2];
    cout <<"Locating (low,high,prolong): ("<<double(system.sQueue.stats.timesLocate[0])/totalCount<<","<<
        double(system.sQueue.stats.timesLocate[1])/totalCount<<","<<
        double(system.sQueue.stats.timesLocate[2])/totalCount<<")"<<endl;
    cout <<"Locating times (low,high,prolong): ("<<double(system.sQueue.stats.timesLocateTimes[0])/totalTime<<","<<
        double(system.sQueue.stats.timesLocateTimes[1])/totalTime<<","<<
        double(system.sQueue.stats.timesLocateTimes[2])/totalTime<<")"<<endl;
    saiFile.close();


    if (statSucc)
    {
        for (int i = 0; i < sai.numIterationStationary; i++)
        {
            system.MakeIteration(i);
        }
        system.sQueue.stats.UpdateStatistics(0);
    }

    double firstServiceAvg = system.sQueue.stats.firstTimeUntilServ.mean,
        secondServiceAvg = system.sQueue.stats.secondTimeUntilServ.mean;
    
    rangeArray.arr[rangeArray.arrIdx].stationar = statSucc;
    rangeArray.arr[rangeArray.arrIdx].theoreticalStationar = system.IsStationar();
    rangeArray.arr[rangeArray.arrIdx].time1 = firstServiceAvg;
    rangeArray.arr[rangeArray.arrIdx].time2 = secondServiceAvg;
    rangeArray.arr[rangeArray.arrIdx].target = UpdateTarget(firstServiceAvg, secondServiceAvg, sai, currFile);
    
    // aggStats.AddStatistics(system.sQueue.stats);
    // if (sai.verbose)
    // {
    //     system.Print(cout);
    //     cout << endl;
    // }

    // double firstServiceAvg = accumulate(firstService.begin(), firstService.end(), 0.)/firstService.size(),
    //     secondServiceAvg = accumulate(secondService.begin(), secondService.end(), 0.)/secondService.size();
  
    // UpdateTarget(firstServiceAvg, secondServiceAvg, sai, currFile);
  
}


void Optimization::DumpTarget(double target, SystemAprioriInfo sai, string filename)
{
    ofstream file;
    if (filename == "")
        file.open(baseSai.outFiles.optFile, ofstream::out | ofstream::app );
    else
        file.open(filename, ofstream::out | ofstream::app );

    sai.PrintOpt(file);
    file<<target<<endl;
}

double Optimization::UpdateTarget(double firstTime, double secondTime, SystemAprioriInfo sai, string filename)
{
    double target = firstTime + secondTime;
    DumpTarget(target, sai, filename);
    if (target <= bestTarget)
    {
        bestTarget = target;
        bestTargetSpec = sai;
        //      DumpTarget(target, sai, filename);
        DumpTarget(bestTarget, sai, sai.outFiles.optFile);
    }

    return target;
}

void Optimization::DumpParams()
{
    ofstream saiFile(baseSai.outFiles.saiFile, ofstream::out | ofstream::app);
    rangeArray.PrintAllParams(saiFile);
}
