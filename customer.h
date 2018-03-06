#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
using namespace std;
struct Customer
{
  int arrivalTime;
  int departureTime;
  void Print()
  {
    cout<<"Customer: arrival="<<arrivalTime<<", departure="<<departureTime<<endl;
  }
};

#endif
