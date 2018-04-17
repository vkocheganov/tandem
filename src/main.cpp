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
#include <chrono>

using namespace std;

SystemAprioriInfo CreateSai(int argc, char * const argv[]);

int main(int argc, char * const argv[])
{
    SystemAprioriInfo sai = CreateSai(argc, argv);

    ServerState initialServerState;
    QueueState initialQueueState;
    ReadStates("../sample_data/sample_states_0", initialServerState, initialQueueState);
    if (sai.verbose)
    {
        initialServerState.Print();
        initialQueueState.Print();
    }

    Statistics aggStats(sai);  
    Optimization optimize(initialQueueState, initialServerState, sai);
    optimize.firstLightTimePrimary = {10, 10, 100};
    optimize.firstLightTimeSecondary = {10, 10, 60};
    optimize.secondLightTimeLow = {10, 10, 60};
    optimize.secondLightTimeHigh = {10, 10, 60};
    optimize.secondLightTimeProlong = {5, 5, 20};
    optimize.threshold = {0, 5, 30};

    if (0)
    {
        optimize.MakeOptimization();
    }
    else
    {
        for (int j = 0; j < sai.numSamples; j++)
	{
            SystemAprioriInfo refSai(sai);
            refSai.verbose=false;
            QueueState refInitialQueueState(true);
            // QueueState refInitialQueueState(initialQueueState);	  
            System system(initialQueueState, initialServerState, sai),
                refSystem(refInitialQueueState, initialServerState, refSai);
            int i = 0;
            //	  for (i = 0; i < sai.numIteration && !system.sQueue.stats.stationaryMode ; i++)
            for (i = 0; i < sai.numMaxIteration; i++)	  
	    {
                refSystem.MakeIteration(i);
                system.MakeIteration(i);
                // if (i % 1000 == 0)
                //     cout <<"i = "<<i<<endl;
                if (system.CheckStationaryMode(refSystem,i))
                    break;
	    }
            
            for (i = 0; i < sai.numIterationStationary; i++)	  
	    {
                system.MakeIteration(i);
                // if (i % 1000 == 0)
                //     cout <<"i = "<<i<<endl;
	    }
            system.sQueue.stats.UpdateStatistics(i);

            cout <<"intensity input/output 1 = "<<double(system.sQueue.stats.inputFirstCust)/system.sQueue.stats.timeTotal
                 <<"/"<<double(system.sQueue.stats.outputFirstCust)/system.sQueue.stats.timeTotal<<endl
                 <<"intensity input/output 3 = "<<double(system.sQueue.stats.inputThirdCust)/system.sQueue.stats.timeTotal
                 <<"/"<<double(system.sQueue.stats.outputThirdCust)/system.sQueue.stats.timeTotal<<endl;
	  
            if (system.sQueue.stats.stationaryMode)
                aggStats.AddStatistics(system.sQueue.stats);
            if (sai.verbose)
	    {
                system.Print();
                cout << endl;
	    }
            system.sQueue.stats.Print();
	}
        cout << endl;
//        aggStats.Print();
    }
  
    return 0;
}

SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     PrimaryFlowDistribution::generator.seed(seed);
    // PrimaryFlowDistribution::generator.seed(2);  
    bool verbose = false;
    SystemAprioriInfo sai = ReadSpecs("../sample_data/sample_spec_1");
    time_t rawtime;
    struct tm *info;
    char tmp_buf[80];
    int opt;
    sai.numMaxIteration = 20000;
    sai.numIterationStationary = 5000;
    sai.numSamples = 10;
    while ((opt = getopt(argc, argv, "v:i:m:s:")) != -1) {
        switch (opt) {
        case 'v':
            verbose = (atoi(optarg) == 1);
            if (verbose) cout <<"Verbose mode"<<endl;
            break;
        case 'm':
            sai.numMaxIteration = atoi(optarg);
            break;
        case 'i':
            sai.numIterationStationary = atoi(optarg);
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
  
    sai.outFiles.foldName = tmp_buf;
    mkdir(sai.outFiles.foldName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sai.outFiles.stationaryFileMeans = sai.outFiles.foldName + "/stationaryMeans";
    sai.outFiles.firstCustomersFile = sai.outFiles.foldName + "/output_customers_first";
    sai.outFiles.secondCustomersFile = sai.outFiles.foldName + "/output_customers_second";
    sai.outFiles.optFile = sai.outFiles.foldName + "/optimization";
    sai.verbose = verbose;
    cout <<"Iterations stationary: "<<sai.numIterationStationary<<endl;
    cout <<"MaxIterations: "<<sai.numMaxIteration<<endl;
    cout <<"Samples: "<<sai.numSamples<<endl;
  
    if (sai.verbose)
        sai.Print();
    return sai;
}
