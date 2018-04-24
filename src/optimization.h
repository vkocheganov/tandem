#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "spec.h"
#include <limits>
#include <string>
#include <vector>

template <typename T>
struct Range
{
    Range (T f, T s, T l): first(f), step(s), last(l){}
    T first = -1;
    T step;
    T last;
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

struct RangeArray
{
    RangeArray(SystemAprioriInfo _baseSai);
    Range<int> ranges[RANGE_INDEXES_LAST];
    vector<double> arr;
    int arrIdx;

    int maxIdx[RANGE_INDEXES_LAST];
    int currIdx;
    int currIdxs[RANGE_INDEXES_LAST];
    int currValues[RANGE_INDEXES_LAST];
    void Resize();
    bool Iterate();

    void Print(ostream&);
};

struct Optimization
{
    Optimization(QueueState, ServerState, SystemAprioriInfo);
    QueueState initialQueueState;
    ServerState initialServerState;
    SystemAprioriInfo baseSai;

    RangeArray rangeArray;
    
    void UpdateTarget(double firstTime, double secondTime, SystemAprioriInfo, string filename="");
    double bestTarget = std::numeric_limits<double>::max();
    SystemAprioriInfo bestTargetSpec;

    void MakeOptimization();
    void Iterate(SystemAprioriInfo sai);
    void DumpTarget(double target, SystemAprioriInfo sai, string filename="");

    string currFile;
};
  
  
#endif
