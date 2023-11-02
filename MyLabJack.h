#ifndef MYLABJACK_MYLABJACK_H
#define MYLABJACK_MYLABJACK_H

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <combaseapi.h>
#include "LabJackUD.h"
#include "DAQ.h"
#include "MyLabJack.h"


//LabJack related global macros
const long deviceType = LJ_dtU6; //LabJack U6
LJ_ERROR lngErrorcode;
LJ_HANDLE lngHandle = 0;
long lngGetNextIteration;
long lngIOType = 0, lngChannel = 0;
double dblValue = 0;
const long resolution = 10; //Configure resolution of the analog inputs (pass a non-zero value for quick sampling).
//See section 2.6 / 3.1 for more information.
const long settlingTime = 4;  //0=5us, 1=10us, 2=100us, 3=1ms, 4=10ms


//Application related macros
double gVoltsopen = 0;
const long numChannels = 14;  //Number of AIN channels, 0-13.
double ValueDIPort = 0;
double ValueAIN[14] = {9999};
double Vref = 4.09;
double Rref = 4020;
const int leftChannelCounts = 7;
long STARTBUTTON;
long STOPBUTTON;

class LJU6 : DAQ {
public:
    /**
     * Open the first found labjack
     * @return 0 if success, err code other wise
     */
    int OpenDevice() override;
    int Init() override;
    double* ReadData() override;
    double* GetAverages() override;
    int GetPassOrFail() override;
    void PostProcess();
    double GetgVoltsOpen();
    int ReadButton();

    /**
     * Computes and gets the statistics of voltage readings.
     * First element is the
     * @return Ptr to data array. data array contains 3 stats: max, min, and diff.
     */
    double* GetVoltStats();
    char* GetErrorMessage(LJ_ERROR lngErrorcode);

private:
    double readings[14];
    double averages[3];
    double stats[3];
    char errmsg[255];
};

#endif //MYLABJACK_MYLABJACK_H
