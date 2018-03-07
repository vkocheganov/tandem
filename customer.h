#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include <fstream>
using namespace std;

struct Customer
{
  static long long count;
  long long idx;
Customer(int arrTime): arrivalTime(arrTime){idx = count++;}
  int arrivalTime=0;
  int departureTime = -1;
  int serviceTime=0;
  void Dump(ofstream& stream)
  {
    int timeUntilService = serviceTime - arrivalTime,
      timeService = departureTime - arrivalTime;
    stream<<""<<idx<<", "<<arrivalTime<<","<<serviceTime<<","<<departureTime<<", "<<timeUntilService<<","<<timeService<<endl;
    //    stream<<"Customer_"<<idx<<": arrival="<<arrivalTime<<", departure="<<departureTime<<endl;
    
  }
};

#endif
