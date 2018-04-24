#include "optimization.h"
#include "system.h"
#include <numeric>
#include <sstream>
#include <time.h>


RangeArray::RangeArray(SystemAprioriInfo _baseSai):
    ranges{
    {_baseSai.fls.primaryTime, 1, _baseSai.fls.primaryTime},
    {_baseSai.fls.secondaryTime, 1, _baseSai.fls.secondaryTime},
    {_baseSai.sls.lowPriorityTime, 1, _baseSai.sls.lowPriorityTime},
    {_baseSai.sls.highPriorityTime, 1, _baseSai.sls.highPriorityTime},
    {_baseSai.sls.prolongationTime, 1, _baseSai.sls.prolongationTime},
    {_baseSai.prolongThres, 1, _baseSai.prolongThres}
}, currIdx{}, maxIdx{}, currIdxs{},
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
    
    void RangeArray::Resize()
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

    // for (int i = 0; i < RANGE_INDEXES_LAST; i++)
    //     cout << currValues[i]<<" ";
    // cout<<endl;
    return true;
}

void RangeArray::Print(ostream& outStream)
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
            if (maxIdx[idx] > 1)
                newLine = true;
            idxs[idx] = 0;
            idx--;
        }
        if (idx < 0)
            break;
        if (newLine)
            outStream<<endl;
        idxs[idx]++;
    }
    outStream<<endl;
}


Optimization::Optimization(QueueState initQueue, ServerState initServer, SystemAprioriInfo _baseSai):
    initialQueueState(initQueue), initialServerState(initServer), baseSai(_baseSai),
    rangeArray(_baseSai)
{
}


void Optimization::MakeOptimization()
{
    SystemAprioriInfo sai(baseSai);

    // for (int fltp = firstLightTimePrimary.first; fltp <= firstLightTimePrimary.last; fltp += firstLightTimePrimary.step)
    // {
    //     sai.fls.primaryTime = fltp;
    //     for (int flts = firstLightTimeSecondary.first; flts <= firstLightTimeSecondary.last; flts += firstLightTimeSecondary.step)
    //     {
    //         stringstream ios;
    //         ios << sai.outFiles.optFile <<"_"<<fltp;
    //         ios << "_"<<flts;
    //         currFile = ios.str();
	  
    //         sai.fls.secondaryTime = flts;
    //         for (int sltl = secondLightTimeLow.first; sltl <= secondLightTimeLow.last; sltl += secondLightTimeLow.step)
    //         {
    //             time_t rawtime;
    //             struct tm *info;
    //             char tmp_buf[80];
    //             time( &rawtime );
    //             info = localtime( &rawtime );
    //             strftime(tmp_buf,80,"_%Y_%m_%d__%H_%M_%S", info);
    //             cout <<tmp_buf<<endl;
  
    //             sai.sls.lowPriorityTime = sltl;
    //             for (int slth = secondLightTimeHigh.first; slth <= secondLightTimeHigh.last; slth += secondLightTimeHigh.step)
    //     	{
    //                 sai.sls.highPriorityTime = slth;
    //                 for (int sltp = secondLightTimeProlong.first; sltp <= secondLightTimeProlong.last; sltp += secondLightTimeProlong.step)
    //     	    {
    //                     sai.sls.prolongationTime = sltp;
    //                     for (int t = threshold.first; t <= threshold.last; t += threshold.step)
    //     		{
    //                         sai.prolongThres = t;
    //                         Iterate(sai);
    //     		}

    //     	    }

    //     	}

    //         }

    //     }

    // }
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

    for (int i = 0; i < sai.numMaxIteration; i++)
    {
        refSystem.MakeIteration(i);
        system.MakeIteration(i);
        system.CheckStationaryMode(refSystem,i);
    }
    aggStats.AddStatistics(system.sQueue.stats);
    if (sai.verbose)
    {
        system.Print(cout);
        cout << endl;
    }

    double firstServiceAvg = accumulate(firstService.begin(), firstService.end(), 0.)/firstService.size(),
        secondServiceAvg = accumulate(secondService.begin(), secondService.end(), 0.)/secondService.size();
  
    UpdateTarget(firstServiceAvg, secondServiceAvg, sai, currFile);
  
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

void Optimization::UpdateTarget(double firstTime, double secondTime, SystemAprioriInfo sai, string filename)
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
}




