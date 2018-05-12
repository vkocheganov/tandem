#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "spec.h"
#include <limits>
#include <string>
#include <vector>
#include <iostream>

template <typename T>
struct Range
{
    Range (T f, T s, T l): first(f), step(s), last(l){}
    T first = -1;
    T step;
    T last;
    void Print(ostream&);
};

enum RangeIndexes
{
    FIRST_LIGHT_TIME_PRIMARY,
    FIRST_LIGHT_TIME_SECONDARY,
    SECOND_LIGHT_TIME_LOW,
    SECOND_LIGHT_TIME_HIGH,
    SECOND_LIGHT_TIME_PROLONG,
    THRESHOLD,
    RANGE_INDEXES_LAST
};


struct OptStats
{
    bool stationar;
    bool theoreticalStationar;
    double timeUntilServiceFirst;
    double timeUntilServiceSecond;
    double timeServiceFirst;
    double timeServiceSecond;
    double target;
    friend ostream& operator<< (ostream& stream, const OptStats& optStats)
        {
            stream<<"("<<optStats.theoreticalStationar<<","<<optStats.stationar<<"|"
                  <<optStats.timeUntilServiceFirst<<","<<optStats.timeUntilServiceSecond<<"|"<<optStats.timeServiceFirst<<","<<
                  optStats.timeServiceSecond<<"|"<<optStats.target<<") ";
            return stream;
        }
};


struct RangeArray
{
    RangeArray(SystemAprioriInfo _baseSai);
    Range<int> ranges[RANGE_INDEXES_LAST];
    vector<OptStats> arr;
    int arrIdx;

    int maxIdx[RANGE_INDEXES_LAST];
    int currIdxs[RANGE_INDEXES_LAST];
    int currValues[RANGE_INDEXES_LAST];
    void Start();
    bool Iterate();

    void PrintArr(ostream&);
    void PrintCurrParams(ostream&);
    void PrintAllParams(ostream&);

    void SetSai(SystemAprioriInfo& sai);
};

struct Optimization
{
    Optimization(QueueState, ServerState, SystemAprioriInfo);
    QueueState initialQueueState;
    ServerState initialServerState;
    SystemAprioriInfo baseSai;

    RangeArray rangeArray;
    
    double UpdateTarget(double firstTime, double secondTime, SystemAprioriInfo, string filename="");
    double bestTarget = std::numeric_limits<double>::max();
    SystemAprioriInfo bestTargetSpec;

    void MakeOptimization();
    void Iterate(SystemAprioriInfo sai);
    void DumpTarget(double target, SystemAprioriInfo sai, string filename="");
    void DumpParams();

    string currFile;
};
  
  
#endif
