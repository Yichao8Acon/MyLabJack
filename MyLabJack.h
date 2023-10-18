#ifndef MYLABJACK_MYLABJACK_H
#define MYLABJACK_MYLABJACK_H

#include <iostream>
#include <vector>
#include <queue>
#include "LabJackUD.h"
#include "DAQ.h"
#include "MyLabJack.h"

LJ_ERROR lngErrorcode;
LJ_HANDLE lngHandle = 0;
long lngGetNextIteration;

long lngIOType = 0, lngChannel = 0;
double dblValue = 0;
double gVoltsopen = 0;

long numChannels = 14;  //Number of AIN channels, 0-16.
long resolution = 10;    //Configure resolution of the analog inputs (pass a non-zero value for quick sampling).
//See section 2.6 / 3.1 for more information.
long settlingTime = 4;  //0=5us, 1=10us, 2=100us, 3=1ms, 4=10ms

double ValueDIPort = 0;
double ValueAIN[14] = {9999};


double Vref = 4.09;
double Rref = 4020;

int leftChannelCounts = 7;

long STARTBUTTON;
long STOPBUTTON;

class LJU6 : DAQ {
public:
    int OpenDevice() override;
    int Init() override;
    double* ReadData() override;
    double* GetAverages() override;
    int GetPassOrFail() override;
    void PostProcess();
    int GetgVoltsOpen();
    int ReadButton();

    /**
     * Computes and gets the statistics of voltage readings.
     * First element is the
     * @return Ptr to data array. data array contains 3 stats: max, min, and diff.
     */
    double* GetVoltStats();

private:
    double readings[14];
    double averages[3];
    double stats[3];
};

#endif //MYLABJACK_MYLABJACK_H
