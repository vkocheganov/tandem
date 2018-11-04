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
        int count = 1;
        for (int i = 0; i < RANGE_INDEXES_LAST; i++)
        {
            currValues[i] = ranges[i].first;
            int count_1 = 0;
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

void RangeArray::PrintArr(ostream& outStream, SystemAprioriInfo sai)
{
    int idxs[RANGE_INDEXES_LAST]{};
    int idx = RANGE_INDEXES_LAST - 1;
    bool cont = true;
    bool newLine = false;
    int aIdx = 0;

    double bestTarget = arr[0].target;
    int bestIdx[RANGE_INDEXES_LAST] = {};
    
    ofstream stationarReachingTheoreticalFile (sai.outFiles.stationarReachingT, ofstream::out | ofstream::app),
        stationarReachingFactFile (sai.outFiles.stationarReachingF, ofstream::out | ofstream::app),
        targetFile (sai.outFiles.stationarTarget, ofstream::out | ofstream::app),
        loadLowFile (sai.outFiles.loadLow, ofstream::out | ofstream::app),
        loadHighFile (sai.outFiles.loadHigh, ofstream::out | ofstream::app),
        prolTimeFile (sai.outFiles.prolTime, ofstream::out | ofstream::app),
        timeUntilStatFile (sai.outFiles.timeUntilStat, ofstream::out | ofstream::app),
        itersUntilStatFile (sai.outFiles.itersUntilStat, ofstream::out | ofstream::app)
        ;

    while (1){
        {
            stationarReachingTheoreticalFile << arr[aIdx].theoreticalStationar<<" ";
            stationarReachingFactFile << arr[aIdx].stationar<<" ";
            targetFile << arr[aIdx].target<<" ";
            loadLowFile << arr[aIdx].loadLow<<" ";
            loadHighFile << arr[aIdx].loadHigh<<" ";
            prolTimeFile << arr[aIdx].avgProl<<" ";
            timeUntilStatFile <<arr[aIdx].timeUntilStationar<<" ";
            itersUntilStatFile <<arr[aIdx].itersUntilStationar<<" ";
    }
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
            {
                stationarReachingTheoreticalFile << endl;
                stationarReachingFactFile << endl;
                targetFile << endl;
                loadLowFile << endl;
                loadHighFile << endl;
                prolTimeFile << endl;
                timeUntilStatFile <<endl;
                itersUntilStatFile <<endl;
            }
            outStream<<endl;
            
            outStream<<"(";
            for (int i = 0; i < RANGE_INDEXES_LAST; i++)
            {
                outStream<<(ranges[i].first + idxs[i]*ranges[i].step)<<" ";
            }
            outStream<<")"<<endl;
        }
        
        if (bestTarget > arr[aIdx].target)
        {
            bestTarget = arr[aIdx].target;
            for (int i = 0; i < RANGE_INDEXES_LAST; i++)
                bestIdx[i] = idxs[i];
        }
        // PrintCurrParams(outStream);
    }
    outStream<<endl;
    outStream<<"best time="<<bestTarget<<"; (";
    for (int i = 0; i < RANGE_INDEXES_LAST; i++)
    {
        outStream<<(ranges[i].first + bestIdx[i] * ranges[i].step)<<" ";
    }
    outStream<<")"<<endl;
    
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
    this->rangeArray.PrintArr(saiFile, baseSai);
//    this->rangeArray.PrintArr(cout,baseSai);

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

    int ii = 0;
    for (ii; ii < sai.numMaxIteration; ii++)
    {
        refSystem.MakeIteration(ii);
        system.MakeIteration(ii);
        if (system.CheckStationaryMode(refSystem,ii) && ii >= sai.numMaxIteration * 0.1)
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


    rangeArray.arr[rangeArray.arrIdx].timeUntilStationar = system.sQueue.stats.timeTotal;
    rangeArray.arr[rangeArray.arrIdx].itersUntilStationar = ii;
    if (statSucc)
    {
        for (int i = 0; i < ii; i++)
        {
            system.MakeIteration(i);
        }
        system.sQueue.stats.UpdateStatistics(0);
    }

    double firstUntilServiceAvg = system.sQueue.stats.firstTimeUntilServ.mean,
        secondUntilServiceAvg = system.sQueue.stats.secondTimeUntilServ.mean,
        firstServiceAvg = system.sQueue.stats.firstTimeServ.mean,
        secondServiceAvg = system.sQueue.stats.secondTimeServ.mean;
    
    rangeArray.arr[rangeArray.arrIdx].stationar = statSucc;
    rangeArray.arr[rangeArray.arrIdx].theoreticalStationar = system.IsStationar();
    rangeArray.arr[rangeArray.arrIdx].timeUntilServiceFirst = firstUntilServiceAvg;
    rangeArray.arr[rangeArray.arrIdx].timeUntilServiceSecond = secondUntilServiceAvg;
    rangeArray.arr[rangeArray.arrIdx].timeServiceFirst = firstServiceAvg;
    rangeArray.arr[rangeArray.arrIdx].timeServiceSecond = secondServiceAvg;
    
    rangeArray.arr[rangeArray.arrIdx].loadLow = system.sQueue.stats.loadStatistics.inputNumLow/system.sQueue.stats.loadStatistics.theoreticalNumLow;
    rangeArray.arr[rangeArray.arrIdx].loadHigh = system.sQueue.stats.loadStatistics.inputNumHigh/system.sQueue.stats.loadStatistics.theoreticalNumHigh;
    
    rangeArray.arr[rangeArray.arrIdx].target = UpdateTarget(firstUntilServiceAvg + firstServiceAvg, secondServiceAvg + secondUntilServiceAvg, sai, currFile);

    rangeArray.arr[rangeArray.arrIdx].avgProl = double (system.sQueue.stats.loadStatistics.prolTime) / system.sQueue.stats.loadStatistics.prolNum;
    
    
    saiFile.open(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    int statesTotalDurations = 0;
    for (auto as:system.server.allStates)
    {
        statesTotalDurations += as.realResideCount * as.timeDuration;
    }

    saiFile<<"states durations:"<<endl;
    for (auto as:system.server.allStates)
    {
        as.Print(saiFile);
        saiFile << double(as.realResideCount * as.timeDuration) / statesTotalDurations<<endl;
    }
    saiFile.close();
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
    double allLambdas = sai.firstFlow.totalLambda + sai.secondFlow.totalLambda;
    double target = (firstTime *sai.firstFlow.totalLambda  + secondTime *sai.secondFlow.totalLambda)/ allLambdas;
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
