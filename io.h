#ifndef IO_H
#define IO_H

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include "servers.h"
#include <numeric>

#define EPS 0.00001
 
SystemAprioriInfo ReadSpecs(string filename)
{
  SystemAprioriInfo sai;
  
  ifstream file(filename);
  if (!file.is_open())
    {
      cout <<"Error openning file: "<<filename<<endl;
      exit(1);
    }

  float temp;
  string line;


  ///// First light information

  //First light primary flow probabilities
  getline(file,line);
  istringstream iss(line);
  while (!iss.eof())
    {
      iss >> temp;
      sai.firstLight.probabilities.push_back(temp);
    }
  float check = accumulate(sai.firstLight.probabilities.begin(), sai.firstLight.probabilities.end(), 0);
  if (check - EPS > check || check + EPS < check)
    {
      cout <<"Error summing probs for first light."<<endl;
      exit(1);
    }

  //First light primary flow intensity
  getline(file,line);
  iss.clear();
  iss.str(line);
  iss >> temp;
  sai.firstLight.lambda = temp;

  //First light server spec
  getline(file,line);
  iss.clear();
  iss.str(line);
  
  iss >> temp; sai.fls.primaryTime = temp;
  iss >> temp; sai.fls.primaryIntensity = temp;
  iss >> temp; sai.fls.secondaryTime = temp;
  iss >> temp; sai.fls.secondaryIntensity = temp;


  ///// Second light information
    
  //Second light primary flow probabilities
  getline(file,line);
  iss.clear();
  iss.str(line);
  while (!iss.eof())
    {
      iss >> temp;
      sai.secondLight.probabilities.push_back(temp);
    }
  check = accumulate(sai.secondLight.probabilities.begin(), sai.secondLight.probabilities.end(), 0);
  if (check - EPS > check || check + EPS < check)
    {
      cout <<"Error summing probs for second light."<<endl;
      exit(1);
    }
  
  //Second light primary flow intensity
  getline(file,line);
  iss.clear();
  iss.str(line);
  iss >> temp;
  sai.secondLight.lambda = temp;
  
  //Second light server spec
  getline(file,line);
  iss.clear();
  iss.str(line);
  
  iss >> temp; sai.sls.lowPriorityTime = temp;
  iss >> temp; sai.sls.lowPriorityIntensity = temp;
  iss >> temp; sai.sls.highPriorityTime = temp;
  iss >> temp; sai.sls.highPriorityIntensity = temp;
  iss >> temp; sai.sls.prolongationTime = temp;
  iss >> temp; sai.sls.prolongationIntensity = temp;
  
  //Midle queue info
  getline(file,line);
  iss.clear();
  iss.str(line);

  iss >> temp; sai.midleQueueSuccProb = temp;
  
  return sai;
}

#endif
