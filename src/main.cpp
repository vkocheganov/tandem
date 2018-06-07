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
#include <stdlib.h>

using namespace std;

SystemAprioriInfo CreateSai(int argc, char * const argv[]);

int main(int argc, char * const argv[])
{
    SystemAprioriInfo sai = CreateSai(argc, argv);

    ServerState initialServerState;
    QueueState initialQueueState;
    ReadStates("../sample_data/sample_states", initialServerState, initialQueueState);
    if (sai.verbose)
    {
        initialServerState.Print(cout);
        initialQueueState.Print();
    }

    Statistics aggStats(sai);
    Optimization optimize(initialQueueState, initialServerState, sai);
    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_PRIMARY] = {10, 5, 20};
    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_SECONDARY] = {10, 5, 20};
    // // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_LOW] = {10, 10, 60};
    // // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_HIGH] = {10, 10, 60};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_PROLONG] = {5, 5, 15};
    // optimize.rangeArray.ranges[THRESHOLD] = {0, 5, 10};
    
    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_PRIMARY] = {8, 5, 8};
    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_SECONDARY] = {8, 15, 8};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_LOW] = {8, 5, 8};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_HIGH] = {8, 8, 8};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_PROLONG] = {4, 1, 4};
    // optimize.rangeArray.ranges[THRESHOLD] = {-1, 6, 5};
    // optimize.rangeArray.ranges[THRESHOLD] = {-1, 6, 5};
    // optimize.rangeArray.ranges[THRESHOLD] = {5, 6, 5};
    // optimize.rangeArray.ranges[THRESHOLD] = {-1, 3, -1};

    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_PRIMARY] = {20, 10, 20};
    // optimize.rangeArray.ranges[FIRST_LIGHT_TIME_SECONDARY] = {10, 10, 10};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_LOW] = {1, 2, 100};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_HIGH] = {1, 2, 100};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_PROLONG] = {5, 1, 5};
    // optimize.rangeArray.ranges[THRESHOLD] = {10, 1, 10};
    
    optimize.rangeArray.ranges[FIRST_LIGHT_TIME_PRIMARY] = {20, 10, 20};
    optimize.rangeArray.ranges[FIRST_LIGHT_TIME_SECONDARY] = {10, 10, 10};
    optimize.rangeArray.ranges[SECOND_LIGHT_TIME_LOW] = {5, 5, 5};
    optimize.rangeArray.ranges[SECOND_LIGHT_TIME_HIGH] = {10, 1, 10};
    optimize.rangeArray.ranges[SECOND_LIGHT_TIME_PROLONG] = {5, 10, 5};
    optimize.rangeArray.ranges[THRESHOLD] = {-1, 6, -1};
    
    // do
    // {
    //     optimize.rangeArray.PrintCurrParams(cout);
    // } while (optimize.rangeArray.Iterate());

    // return 0;
    cout <<"Make optimization"<<endl;
    if (1)
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
            System system(initialQueueState, initialServerState, sai),
                refSystem(refInitialQueueState, initialServerState, refSai);
            int i = 0;
            for (i = 0; i < sai.numMaxIteration; i++)	  
	    {
                refSystem.MakeIteration(i);
                system.MakeIteration(i);
                if (system.CheckStationaryMode(refSystem,i) && i >= sai.numMaxIteration * 0.1)
                    break;
	    }
            
            for (i = 0; i < sai.numIterationStationary; i++)
	    {
                system.MakeIteration(i);
	    }
            system.sQueue.stats.UpdateStatistics(i);

            ofstream saiFile(sai.outFiles.saiFile, ofstream::out | ofstream::app);
            saiFile<<"intensity input/output 1 = "<<double(system.sQueue.stats.inputFirstCust)/system.sQueue.stats.timeTotal
                   <<"/"<<double(system.sQueue.stats.outputFirstCust)/system.sQueue.stats.timeTotal<<endl
                   <<"intensity input/output 3 = "<<double(system.sQueue.stats.inputThirdCust)/system.sQueue.stats.timeTotal
                   <<"/"<<double(system.sQueue.stats.outputThirdCust)/system.sQueue.stats.timeTotal<<endl;
            saiFile.close();
	  
            if (system.sQueue.stats.stationaryMode)
                aggStats.AddStatistics(system.sQueue.stats);

            
            system.sQueue.stats.Print(saiFile);
	}
        cout << endl;
//        aggStats.Print();
    }
  
    system("rm -rf LOGS/las_log");
    system(("cp -R "+sai.outFiles.foldName+" LOGS/las_log").c_str());

    time_t rawtime;
    struct tm *info;
    char tmp_buf[80];
    time( &rawtime );
    info = localtime( &rawtime );
    ofstream saiFile(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    strftime(tmp_buf,80,"%Y_%m_%d__%H_%M_%S", info);
    saiFile<<"Finish time "<<tmp_buf<<endl;
    return 0;
}

SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     PrimaryFlowDistribution::generator.seed(seed);
    // PrimaryFlowDistribution::generator.seed(2);  
    bool verbose = false;
    SystemAprioriInfo sai = ReadSpecs("../sample_data/sample_spec");
    int opt;
    sai.numMaxIteration = 100000;
    sai.numIterationStationary = 100000;
    sai.numSamples = 1;
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

    time_t rawtime;
    struct tm *info;
    char tmp_buf[80];
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(tmp_buf,80,"LOGS/LOG_%Y_%m_%d__%H_%M_%S", info);
  
    sai.outFiles.foldName = tmp_buf;
    mkdir(sai.outFiles.foldName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sai.outFiles.stationaryFileMeans = sai.outFiles.foldName + "/stationaryMeans";
    sai.outFiles.firstCustomersFile = sai.outFiles.foldName + "/output_customers_first";
    sai.outFiles.secondCustomersFile = sai.outFiles.foldName + "/output_customers_second";
    sai.outFiles.saiFile = sai.outFiles.foldName + "/sai";
    sai.outFiles.optFile = sai.outFiles.foldName + "/optimization";
    sai.outFiles.stationaryReaching = sai.outFiles.foldName + "/stationaryReaching";

    
    sai.outFiles.stationarReachingT = sai.outFiles.foldName + "/stationaryReachingT";
    sai.outFiles.stationarReachingF = sai.outFiles.foldName + "/stationaryReachingF";
    sai.outFiles.stationarTarget = sai.outFiles.foldName + "/target";
    sai.outFiles.outputSpec = sai.outFiles.foldName + "/spec";

    sai.outFiles.loadLow = sai.outFiles.foldName + "/loadLow";
    sai.outFiles.loadHigh = sai.outFiles.foldName + "/loadHigh";

    sai.verbose = verbose;
    
    ofstream saiFile(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    strftime(tmp_buf,80,"%Y_%m_%d__%H_%M_%S", info);
    saiFile << "Begin Time = "<<tmp_buf<<endl;
    saiFile <<"Iterations stationary: "<<sai.numIterationStationary<<endl;
    saiFile <<"MaxIterations: "<<sai.numMaxIteration<<endl;
    saiFile <<"Samples: "<<sai.numSamples<<endl;

    sai.Print(saiFile);
    sai.PrintOut(sai.outFiles.outputSpec);
    return sai;
}
