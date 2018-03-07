#include "servers.h"
#include <iostream>
#include "spec.h"
#include "io.h"
#include "servers.h"
#include "queue.h"
#include "system.h"
#include <unistd.h>

using namespace std;

int main(int argc, char * const argv[])
{
  string opts_string;
  bool verbose = false;
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
  
  SystemAprioriInfo sai = ReadSpecs("sample_spec_0");
  sai.filename ="output_customers";
  sai.verbose = verbose;

  if (verbose)
    sai.Print();
  ServerState initialServerState;
  QueueState initialQueueState;
  ReadStates("sample_states_0", initialServerState, initialQueueState);
  if (verbose)
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
