#ifndef IO_H
#define IO_H

#include <string>

#include "spec.h"
#include "queue.h"

#define EPS 0.00001
 
SystemAprioriInfo ReadSpecs(string filename);
void ReadStates(string filename, ServerState& serverState, QueueState& queueState);

#endif
