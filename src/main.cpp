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
    ReadStates("../sample_data/sample_states", initialServerState, initialQueueState);
    if (sai.verbose)
    {
        initialServerState.Print(cout);
        initialQueueState.Print();
    }

    Statistics aggStats(sai);
    Optimization optimize(initialQueueState, initialServerState, sai);
    optimize.rangeArray.ranges[FIRST_LIGHT_TIME_PRIMARY] = {10, 1, 15};
    optimize.rangeArray.ranges[FIRST_LIGHT_TIME_SECONDARY] = {10, 1, 15};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_LOW] = {10, 10, 60};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_HIGH] = {10, 10, 60};
    // optimize.rangeArray.ranges[SECOND_LIGHT_TIME_PROLONG] = {5, 5, 20};
    // optimize.rangeArray.ranges[THRESHOLD] = {0, 5, 30};
    

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
  
    return 0;
}

SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
     PrimaryFlowDistribution::generator.seed(seed);
    // PrimaryFlowDistribution::generator.seed(2);  
    bool verbose = false;
    SystemAprioriInfo sai = ReadSpecs("../sample_data/sample_spec");
    time_t rawtime;
    struct tm *info;
    char tmp_buf[80];
    int opt;
    sai.numMaxIteration = 20000;
    sai.numIterationStationary = 5000;
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

    time( &rawtime );
    info = localtime( &rawtime );
    strftime(tmp_buf,80,"LOGS/LOG_%Y_%m_%d__%H_%M_%S", info);
    cout <<tmp_buf<<endl;
  
    sai.outFiles.foldName = tmp_buf;
    mkdir(sai.outFiles.foldName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    sai.outFiles.stationaryFileMeans = sai.outFiles.foldName + "/stationaryMeans";
    sai.outFiles.firstCustomersFile = sai.outFiles.foldName + "/output_customers_first";
    sai.outFiles.secondCustomersFile = sai.outFiles.foldName + "/output_customers_second";
    sai.outFiles.saiFile = sai.outFiles.foldName + "/sai";
    sai.outFiles.optFile = sai.outFiles.foldName + "/optimization";
    sai.outFiles.stationaryReaching = sai.outFiles.foldName + "/stationaryReaching";
    sai.verbose = verbose;
    
    ofstream saiFile(sai.outFiles.saiFile, ofstream::out | ofstream::app);
    saiFile <<"Iterations stationary: "<<sai.numIterationStationary<<endl;
    saiFile <<"MaxIterations: "<<sai.numMaxIteration<<endl;
    saiFile <<"Samples: "<<sai.numSamples<<endl;

    // sai.Print(saiFile);
    return sai;
}
