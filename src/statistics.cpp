#include "statistics.h"

void Statistics::UpdateStatistics(int iteration)
{
    {
        if (sai.verbose)
        {
            DumpAllCustomers();
        }

        firstTimeUntilServ.UpdateMean();
        firstTimeServ.UpdateMean();
        secondTimeUntilServ.UpdateMean();
        secondTimeServ.UpdateMean();
      
        firstPrimary.UpdateMean();
        secondHigh.UpdateMean();
        secondLow.UpdateMean();
        middle.UpdateMean();

        if (stationaryMode)
	{
            ofstream file_(sai.outFiles.stationaryFileMeans, ofstream::out | ofstream::app );
            DumpStatsMean(file_); file_<<endl;
            // this->firstTimeUntilServ.Print_errs();
            // cout<<endl;
	}
    }
}

void Statistics::DumpAllCustomers()
{
    ofstream file1(sai.outFiles.firstCustomersFile, ofstream::out | ofstream::app );
    ofstream file2(sai.outFiles.secondCustomersFile, ofstream::out | ofstream::app );

    for (auto& a : departFirstQueue)
        a.Dump(file1);
    for (auto& a : departSecondQueue)
        a.Dump(file2);
}

void Statistics::AddFirstCustomer(Customer cust)
{
    long untilServ = cust.serviceTime - cust.arrivalTime,
        serv = cust.departureTime - cust.serviceTime;
    if (untilServ < 0 || serv <= 0)
    {
        cout <<" "<<cust.arrivalTime<<" "<<cust.serviceTime<<" "<<cust.departureTime<<";";
        cout <<untilServ << " "<<serv<<" < 0"<<endl;
        exit(1);
    }
    else
    {
        firstTimeUntilServ.values.push_back(untilServ);
        firstTimeServ.values.push_back(serv);
        departFirstQueue.push_back(cust);
    }
}
void Statistics::AddSecondCustomer(Customer cust)
{
    secondTimeUntilServ.values.push_back(cust.serviceTime - cust.arrivalTime);
    secondTimeServ.values.push_back(cust.departureTime - cust.serviceTime);
    departSecondQueue.push_back(cust);
}

void Statistics::AddStatistics(Statistics& s)
{
    firstTimeUntilServ.AddMeans(s.firstTimeUntilServ);
    firstTimeServ.AddMeans(s.firstTimeServ);
    secondTimeUntilServ.AddMeans(s.secondTimeUntilServ);
    secondTimeServ.AddMeans(s.secondTimeServ);

    firstPrimary.AddMeans(s.firstPrimary);
    secondHigh.AddMeans(s.secondHigh);
    secondLow.AddMeans(s.secondLow);
    middle.AddMeans(s.middle);
}

void Statistics::Print(ostream& outStream)
{
    outStream << "FirstTimeUntilServ:";
    firstTimeUntilServ.Print(outStream);

    outStream << "FirstTimeServ:";
    firstTimeServ.Print(outStream);

    outStream << "SecondTimeUntilServ:";
    secondTimeUntilServ.Print(outStream);

    outStream << "SecondTimeServ:";
    secondTimeServ.Print(outStream);
    outStream<<endl;

    outStream << "FirstPrimary queue:";
    firstPrimary.Print(outStream);

    outStream << "SecondHigh queue:";
    secondHigh.Print(outStream);

    outStream << "SecondLow queue:";
    secondLow.Print(outStream);

    outStream << "Middle queue:";
    middle.Print(outStream);
}

void Statistics::ClearStatistics()
{
    // cout<<"Clearing stats"<<endl;
    firstTimeUntilServ.Clear();
    firstTimeServ.Clear();
    secondTimeUntilServ.Clear();
    secondTimeServ.Clear();
    
    firstPrimary.Clear();
    secondHigh.Clear();
    secondLow.Clear();
    middle.Clear();
}

void Statistics::DumpStatsMean(ofstream& _stream)
{
    firstTimeUntilServ.DumpMeans(_stream);
    firstTimeServ.DumpMeans(_stream);
    secondTimeUntilServ.DumpMeans(_stream);
    secondTimeServ.DumpMeans(_stream);
  
    firstPrimary.DumpMeans(_stream);
    secondHigh.DumpMeans(_stream);
    secondLow.DumpMeans(_stream);
    middle.DumpMeans(_stream);
}


void MyMean::UpdateMean()
{
    double old_sum = mean,
        old_sum_sq = mean_sq;
  
    double sum = mean * num,
        sum_sq = mean_sq * num;

    for (auto& a: values)
    {
        sum += a;
        sum_sq += a*a;
        if (a < 0)
            cout <<a<<endl;
    }
    num += values.size();
    if (num > 0)
    {
        mean = sum/num;
        mean_sq = sum_sq/num;
        values.clear();
    }

    if (old_sum > 0)
        diff = std::abs(old_sum - mean)/old_sum;
    else
        diff = 1.;
    
    double old_std = std::sqrt(double(old_sum_sq - old_sum * old_sum)),
        new_std = std::sqrt(double(mean_sq - mean*mean));

    if (old_std > 0)
        diff_std = std::abs(old_std - new_std)/double(old_std);
    else
        diff_std = 1.;


    //Error est
    double tmp_est = est_err * est_num,
        tmp_est_sq = est_err_sq * est_num;
    tmp_est += mean;
    tmp_est_sq += mean*mean;
    est_num++;
    est_err = tmp_est/est_num;
    est_err_sq = tmp_est_sq/est_num;


}

void MyMean::AddMeans(MyMean& m)
{
    double old_sum = mean,
        old_sum_sq = mean_sq;
  
    this->UpdateMean();
    m.UpdateMean();
  
    mean = mean * num + m.mean * m.num;
    mean_sq = mean_sq * num + m.mean_sq * m.num;
    num += m.num;
    if (num > 0)
    {
        mean /= num;
        mean_sq /= num;
    }
  
    if (old_sum > 0)
        diff = std::abs(old_sum - mean)/old_sum;
    else
        diff = 1.;
  
    double old_std = std::sqrt(double(old_sum_sq - old_sum * old_sum)),
        new_std = std::sqrt(double(mean_sq - mean*mean));

    if (old_std > 0)
        diff_std = std::abs(old_std - new_std)/double(old_std);
    else
        diff_std = 1.;
}


