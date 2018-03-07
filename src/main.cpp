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

SystemAprioriInfo CreateSai(int argc, char * const argv[])
{
  SystemAprioriInfo sai;
  time_t rawtime;
  struct tm *info;
  char tmp_buf[80];
  int opt;
  while ((opt = getopt(argc, argv, "v:")) != -1) {
    switch (opt) {
    case 'v':
      verbose = (atoi(optarg) == 1);
      if (verbose) cout <<"Verbose mode"<<endl;
      break;
    default: /* '?' */
      fprintf(stderr, "Usage: %s [-t nsecs] [-n] name\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  time( &rawtime );
  info = localtime( &rawtime );
  strftime(tmp_buf,80,"LOG_%Y_%m_%d__%H_%M_%S", info);
  cout <<tmp_buf<<endl;
  
  SystemAprioriInfo sai = ReadSpecs("sample_spec_0");
  sai.foldName = tmp_buf;
  mkdir(sai.foldName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  sai.filename = sai.foldName + "/output";
  sai.verbose = verbose;
  
}
int main(int argc, char * const argv[])
{
  SystemAprioriInfo sai = CreateSai(argc, argv);
  if (sai.verbose)
    sai.Print();
  ServerState initialServerState;
  QueueState initialQueueState;
  ReadStates("sample_states_0", initialServerState, initialQueueState);
  if (sai.verbose)
    {
      initialServerState.Print();
      initialQueueState.Print();
    }

  System system(initialQueueState, initialServerState, sai);

  vector<Cycle> cycles = FindCycles(system.server.allStates, sai);

  const int ITERS = 10000;
  for (int i = 0; i < ITERS; i++)
    {
      system.MakeIteration(i);
    }

  system.Print();
  
  return 0;
}