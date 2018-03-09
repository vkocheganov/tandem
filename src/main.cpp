#include "servers.h"
#include <iostream>
#include "spec.h"
#include "io.h"
#include "servers.h"
#include "queue.h"
#include "system.h"
#include <unistd.h>
#include <sys/stat.h>
#include <numeric>
#include "optimization.h"

using namespace std;

SystemAprioriInfo CreateSai(int argc, char * const argv[]);

int main(int argc, char * const argv[])
{
  srand(time(NULL));
  SystemAprioriInfo sai = CreateSai(argc, argv);

  ServerState initialServerState;
  QueueState initialQueueState;
  ReadStates("../sample_data/sample_states_0", initialServerState, initialQueueState);
  if (sai.verbose)
    {
      initialServerState.Print();
      initialQueueState.Print();
    }

  
  Optimization optimize(initialQueueState, initialServerState, sai);
  optimize.firstLightTimePrimary = {10, 10, 100};
  optimize.firstLightTimeSecondary = {10, 10, 60};
  optimize.secondLightTimeLow = {10, 10, 60};
  optimize.secondLightTimeHigh = {10, 10, 60};
  optimize.secondLightTimeProlong = {5, 5, 20};
  optimize.threshold = {0, 5, 30};

  optimize.MakeOptimization();

  // vector<double> firstUntilService,
  //   firstService,
  //   secondUntilService,
  //   secondService;
  // for (int j = 0; j < sai.numSamples; j++)
  //   {
  //     System system(initialQueueState, initialServerState, sai);

  //     for (int i = 0; i < sai.numIteration; i++)
  // 	{
  // 	  system.MakeIteration(i);
  // 	}
  //     firstUntilService.push_back(system.sQueue.stats.stationaryMeanTime_first.mean_untilService);
  //     firstService.push_back(system.sQueue.stats.stationaryMeanTime_first.mean_Service);
  //     secondUntilService.push_back(system.sQueue.stats.stationaryMeanTime_second.mean_untilService);
  //     secondService.push_back(system.sQueue.stats.stationaryMeanTime_second.mean_Service);
  //     if (sai.verbose)
  // 	{
  // 	  system.Print();
  // 	  cout << endl;
  // 	}
  //   }
  // cout << endl;

  // cout <<"First. Until service time="  <<(accumulate(firstUntilService.begin(), firstUntilService.end(), 0.))/firstUntilService.size()<<", service time="<<(accumulate(firstService.begin(), firstService.end(), 0.))/firstService.size()<<endl;
  // cout <<"Second. Until service time="  <<(accumulate(secondUntilService.begin(), secondUntilService.end(), 0.))/secondUntilService.size()<<", service time="<<(accumulate(secondService.begin(), secondService.end(), 0.))/secondService.size()<<endl;

  
  return 0;
}



SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
  bool verbose = false;
  SystemAprioriInfo sai = ReadSpecs("../sample_data/sample_spec_1");
  time_t rawtime;
  struct tm *info;
  char tmp_buf[80];
  int opt;
  sai.numIteration = 1000;
  sai.numSamples = 10;
  while ((opt = getopt(argc, argv, "v:i:s:")) != -1) {
    switch (opt) {
    case 'v':
      verbose = (atoi(optarg) == 1);
      if (verbose) cout <<"Verbose mode"<<endl;
      break;
    case 'i':
      sai.numIteration = atoi(optarg);
      break;
    case 's':
      sai.numSamples = atoi(optarg);
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  time( &rawtime );
  info = localtime( &rawtime );
  strftime(tmp_buf,80,"LOGS/LOG_%Y_%m_%d__%H_%M_%S", info);
  cout <<tmp_buf<<endl;
  
  sai.foldName = tmp_buf;
  mkdir(sai.foldName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  sai.filename = sai.foldName + "/output";
  sai.firstCustomersFile = sai.foldName + "/output_customers_first";
  sai.secondCustomersFile = sai.foldName + "/output_customers_second";
  sai.optFile = sai.foldName + "/optimization";
  sai.verbose = verbose;
  cout <<"Iterations: "<<sai.numIteration<<endl;
  cout <<"Samples: "<<sai.numSamples<<endl;
  
  if (sai.verbose)
    sai.Print();
  return sai;
}
