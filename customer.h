#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
using namespace std;

struct Customer
{
Customer(int arrTime): arrivalTime(arrTime){}
  int arrivalTime;
  int departureTime = -1;
  void Print()
  {
    cout<<"Customer: arrival="<<arrivalTime<<", departure="<<departureTime<<endl;
  }
};

#endif
