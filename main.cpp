//
// Created by Yichao.Qin on 10/9/2023.
//

//---------------------------------------------------------------------------
//
//  allio.cpp
//
//	Demonstrates using the add/go/get method to efficiently write and read
//	some or all analog and digital I/O on the LabJack U6.
//	Records the time for 1000 iterations and divides by 1000, to allow
//	verification of the basic command/response communication times of the
//	LabJack U6 as documented in Section 3.1 of the U6 User's Guide.
//
//  support@labjack.com
//  June 3, 2009
//	Revised December 27, 2010
//----------------------------------------------------------------------
//

#include <stdio.h>
#include <windows.h>
#include <math.h>
#include "LabJackUD.h"

extern "C"
{
__declspec(dllexport) int add(int a, int b) {
    return a + b;
}
__declspec(dllexport) int subtract(int a, int b) {
    return a - b;
}
}

extern "C"{
__declspec(dllexport) int mainprocess(){
    LJ_ERROR lngErrorcode;
    LJ_HANDLE lngHandle=0;
    long lngGetNextIteration;
    char ch;

    long lngIOType=0, lngChannel=0;
    double dblValue=0;
    double ValueDIPort=0;
    double ValueAIN[16]={9999};


    long time=0, i=0, j=0;
    long numIterations = 100;
    long numChannels = 16;  //Number of AIN channels, 0-16.
    long resolution = 0;	//Configure resolution of the analog inputs (pass a non-zero value for quick sampling).
    //See section 2.6 / 3.1 for more information.
    long settlingTime = 1;  //0=5us, 1=10us, 2=100us, 3=1ms, 4=10ms
    lngErrorcode = OpenLabJack (LJ_dtU6, LJ_ctUSB, "1", 1, &lngHandle);
    return lngErrorcode;
}
}