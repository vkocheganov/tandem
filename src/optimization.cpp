#include "optimization.h"
#include "system.h"
#include <numeric>
#include <sstream>
#include <time.h>


Optimization::Optimization(QueueState initQueue, ServerState initServer, SystemAprioriInfo _baseSai):
  initialQueueState(initQueue), initialServerState(initServer), baseSai(_baseSai),
  firstLightTimePrimary(_baseSai.fls.primaryTime, 1, _baseSai.fls.primaryTime),
  firstLightTimeSecondary(_baseSai.fls.secondaryTime, 1, _baseSai.fls.secondaryTime),
  secondLightTimeLow(_baseSai.sls.lowPriorityTime, 1, _baseSai.sls.lowPriorityTime),
  secondLightTimeHigh(_baseSai.sls.highPriorityTime, 1, _baseSai.sls.highPriorityTime),
  secondLightTimeProlong(_baseSai.sls.prolongationTime, 1, _baseSai.sls.prolongationTime),
  threshold(_baseSai.prolongThres, 1, _baseSai.prolongThres)
{
}


void Optimization::MakeOptimization()
{
  SystemAprioriInfo sai(baseSai);

  for (int fltp = firstLightTimePrimary.first; fltp <= firstLightTimePrimary.last; fltp += firstLightTimePrimary.step)
    {
      stringstream ios;
      ios << sai.optFile <<"_"<<fltp;
      
      currFile = sai.optFile + "";
      sai.fls.primaryTime = fltp;
      for (int flts = firstLightTimeSecondary.first; flts <= firstLightTimeSecondary.last; flts += firstLightTimeSecondary.step)
	{
	  ios << "_"<<flts;
	  currFile = ios.str();
	  
	  sai.fls.secondaryTime = flts;
	  for (int sltl = secondLightTimeLow.first; sltl <= secondLightTimeLow.last; sltl += secondLightTimeLow.step)
	    {
	      time_t rawtime;
	      struct tm *info;
	      char tmp_buf[80];
	      time( &rawtime );
	      info = localtime( &rawtime );
	      strftime(tmp_buf,80,"_%Y_%m_%d__%H_%M_%S", info);
	      cout <<tmp_buf<<endl;
  
	      sai.sls.lowPriorityTime = sltl;
	      for (int slth = secondLightTimeHigh.first; slth <= secondLightTimeHigh.last; slth += secondLightTimeHigh.step)
		{
		  sai.sls.highPriorityTime = slth;
		  for (int sltp = secondLightTimeProlong.first; sltp <= secondLightTimeProlong.last; sltp += secondLightTimeProlong.step)
		    {
		      sai.sls.prolongationTime = sltp;
		      for (int t = threshold.first; t <= threshold.last; t += threshold.step)
			{
			  sai.prolongThres = t;
			  Iterate(sai);
			}

		    }

		}

	    }

	}

    }
}

void Optimization::Iterate(SystemAprioriInfo sai)
{
  vector<double> firstService, secondService;
  
  for (int j = 0; j < sai.numSamples; j++)
    {
      System system(initialQueueState, initialServerState, sai);

      for (int i = 0; i < sai.numIteration; i++)
	{
	  system.MakeIteration(i);
	}
      firstService.push_back(system.sQueue.stats.stationaryMeanTime_first.mean_Service);
      secondService.push_back(system.sQueue.stats.stationaryMeanTime_second.mean_Service);
      if (sai.verbose)
	{
	  system.Print();
	  cout << endl;
	}
    }

  double firstServiceAvg = accumulate(firstService.begin(), firstService.end(), 0.)/firstService.size(),
    secondServiceAvg = accumulate(secondService.begin(), secondService.end(), 0.)/secondService.size();
  
  UpdateTarget(firstServiceAvg, secondServiceAvg, sai, currFile);
  
}


void Optimization::DumpTarget(double target, SystemAprioriInfo sai, string filename)
{
  ofstream file;
  if (filename == "")
    file.open(baseSai.optFile, ofstream::out | ofstream::app );
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
      DumpTarget(bestTarget, sai, sai.optFile);
    }
}




