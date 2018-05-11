#ifndef STATISTICS_H
#define STATISTICS_H

#include <queue>
#include <deque>
#include <iostream>
#include <list>
#include "spec.h"
#include "customer.h"

#include <fstream>
#include <cmath>
#include <random>
#include "flow.h"
using namespace std;


struct MyMean
{
    vector<int> values;
    double mean = 0;
    double mean_sq = 0;
    double diff = 0;
    double diff_std = 0;

    vector<double> ests;
    double est_err = 0;
    double est_err_sq = 0;
    int est_num = 0;
  
    int num = 0;
    void UpdateMean();
    void AddMeans(MyMean&);
    void Print(ostream& outStream){ outStream<<"mean="<<mean<<", sigma="<<std::sqrt(double(mean_sq - mean*mean))<<", n_samples="<<num<<"; err="<<std::sqrt(double(est_err_sq - est_err*est_err))<<"("<<est_num<<")"<<endl;}
    void DumpMeans(ofstream& _stream){ _stream<<mean<<" "<<diff<<"  ";}
    void DumpMeansDiffs(ofstream& _stream){ _stream<<diff<<" ";}
    void DumpStd(ofstream& _stream){ _stream<<std::sqrt(double(mean_sq - mean*mean))<<" ";}
    void DumpStdDiffs(ofstream& _stream){ _stream<<diff_std<<" ";}
    void Clear() {values.clear(); mean = mean_sq = num = 0; est_err = est_err_sq = est_num = 0;}

    bool CheckErr(double ratio = 0.2) {
        if (mean < 0.01)
            return true;
        else
            return (std::sqrt(double(est_err_sq - est_err*est_err)) < mean*ratio + 0.00005 ? true : false) ;  }
    void PrintErr(ostream& outStream) {outStream<<std::sqrt(double(est_err_sq - est_err*est_err))<<"/"<<mean<<" ";}
};

struct Statistics
{
Statistics(SystemAprioriInfo _sai): sai(_sai){};
    deque<Customer> departFirstQueue;
    deque<Customer> departSecondQueue;

    long long unsigned timeTotal = 0;

    long long unsigned inputFirstCust = 0;
    long long unsigned outputFirstCust = 0;
    long long unsigned inputThirdCust = 0;
    long long unsigned outputThirdCust = 0;
  
    bool stationaryMode = false;
    const int GRAN = 100;
    const float RATIO_CHANGE = 0.1;
    SystemAprioriInfo sai;

    int timesLocate[3] = {};
    int timesLocateTimes[3] = {};

    MyMean firstTimeUntilServ;
    MyMean firstTimeServ;
    MyMean secondTimeUntilServ;
    MyMean secondTimeServ;

    
  
    MyMean firstPrimary;
    MyMean secondHigh;
    MyMean secondLow;
    MyMean middle;

    void Print(ostream&);
    void DumpAllCustomers();
    void DumpStatsMean(ofstream&);
    
    void ClearStatistics();
    void UpdateStatistics(int);
    void AddFirstCustomer(Customer);
    void AddSecondCustomer(Customer);
    void AddStatistics(Statistics&);
};
  

#endif
