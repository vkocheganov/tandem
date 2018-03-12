#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "spec.h"
#include <limits>
#include <string>

template <typename T>
struct Range
{
  Range (T f, T s, T l): first(f), step(s), last(l){}
  T first = -1;
  T step;
  T last;
};


						      
struct Optimization
{
  Optimization(QueueState, ServerState, SystemAprioriInfo);
  QueueState initialQueueState;
  ServerState initialServerState;
  SystemAprioriInfo baseSai;

  Range<int> firstLightTimePrimary;
  Range<int> firstLightTimeSecondary;

  Range<int> secondLightTimeLow;
  Range<int> secondLightTimeHigh;
  Range<int> secondLightTimeProlong;

  Range<int> threshold;

  void UpdateTarget(double firstTime, double secondTime, SystemAprioriInfo, string filename="");
  double bestTarget = std::numeric_limits<double>::max();
  SystemAprioriInfo bestTargetSpec;

  void MakeOptimization();
  void Iterate(SystemAprioriInfo sai);
  void DumpTarget(double target, SystemAprioriInfo sai, string filename="");

  string currFile;
};
  
  
#endif
