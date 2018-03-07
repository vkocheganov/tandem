#include "servers.h"
#include <iostream>
#include "spec.h"
#include "io.h"
#include "servers.h"
#include "queue.h"
#include "system.h"
#include <unistd.h>
#include <sys/stat.h>

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

  System system(initialQueueState, initialServerState, sai);

  for (int i = 0; i < sai.numIteration; i++)
    {
      system.MakeIteration(i);
    }

  system.Print();
  
  return 0;
}



SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
  bool verbose = false;
  SystemAprioriInfo sai = ReadSpecs("../sample_data/sample_spec_0");
  time_t rawtime;
  struct tm *info;
  char tmp_buf[80];
  int opt;
  sai.numIteration = 1000;
  while ((opt = getopt(argc, argv, "v:i:")) != -1) {
    switch (opt) {
    case 'v':
      verbose = (atoi(optarg) == 1);
      if (verbose) cout <<"Verbose mode"<<endl;
      break;
    case 'i':
      sai.numIteration = atoi(optarg);
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
  sai.verbose = verbose;
  cout <<"Iterations: "<<sai.numIteration<<endl;
  
  if (sai.verbose)
    sai.Print();
  return sai;
}
