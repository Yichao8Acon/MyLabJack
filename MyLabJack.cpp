#include "MyLabJack.h"

//This is our simple error handling function that is called after every UD
//function call.  This function displays the errorcode and string description
//of the error.  It also has a line number input that can be used with the
//macro __LINE__ to display the line number in source code that called the
//error handler.  It also has an iteration input is useful when processing
//results in a loop (getfirst/getnext).
int ErrorHandler(LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration) {
    char err[255];
    if (lngErrorcode != LJE_NOERROR) {
        ErrorToString(lngErrorcode, err);
//        std::cout << "Exception in MyLabJack.dll: \n";
//        std::cout << "Error number = " << lngErrorcode;
//        std::cout << "  " << "Error string = " << err;
//        std::cout << "  " << "Source line number = " << lngLineNumber << std::endl;
        if (lngErrorcode > LJE_MIN_GROUP_ERROR) {

            return 1;
        }
    }
    return 0;
}

char* LJU6::GetErrorMessage(LJ_ERROR lngErrorcode){

    char err[255];
    ErrorToString(lngErrorcode, err);
    std::string head = "Exception in MyLabJack.dll: \r\nError number = " + std::to_string(lngErrorcode) + " Error string = ";
    head += err;
    char msg[1024];
    strcpy(msg, head.c_str());
    ULONG ulSize = strlen(msg) + sizeof(char);
    char* pszReturn = NULL;

    pszReturn = (char*)::CoTaskMemAlloc(ulSize);
    // Copy the contents of err
    // to the memory pointed to by pszReturn.
    strcpy(pszReturn, msg);
    // Return pszReturn.
    return pszReturn;
}

int LJU6::OpenDevice() {
    lngErrorcode = OpenLabJack(deviceType, LJ_ctUSB, "1", 1, &lngHandle); //Open the first found LabJack.
    return ErrorHandler(lngErrorcode, __LINE__, 0);
}

int LJU6::Init() {
    lngErrorcode = ePut(lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, resolution, 0); //Configure resolution.
    lngErrorcode = ePut(lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_SETTLING_TIME, settlingTime, 0); //Configure settling time
    lngErrorcode = GoOne(lngHandle);
    if (ErrorHandler(lngErrorcode, __LINE__, 0)) return lngErrorcode;

    for (int i = 0; i < numChannels; i++) {
        lngErrorcode = ePut(lngHandle, LJ_ioPUT_AIN_RANGE, i, LJ_rgBIP1V, 0);
        if (ErrorHandler(lngErrorcode, __LINE__, 0)) return lngErrorcode;
    }
    lngErrorcode = GoOne(lngHandle);
    if (ErrorHandler(lngErrorcode, __LINE__, 0)) return lngErrorcode;

    for (int i = 0; i < numChannels; i++) {
        lngErrorcode = AddRequest(lngHandle, LJ_ioGET_AIN, i, 0, 0, 0);
        if (ErrorHandler(lngErrorcode, __LINE__, 0)) return lngErrorcode;
    }
    lngErrorcode = GoOne(lngHandle);
    if (ErrorHandler(lngErrorcode, __LINE__, 0)) return lngErrorcode;

    return 0;
}

double *LJU6::ReadData() {
    lngErrorcode = GoOne(lngHandle);

    lngErrorcode = GetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
    if (ErrorHandler(lngErrorcode, __LINE__, 0)) return nullptr;

    gVoltsopen = dblValue;
    while (lngErrorcode < LJE_MIN_GROUP_ERROR) {
        switch (lngIOType) {
            case 10 : // LJ_ioGET_AIN
                ValueAIN[lngChannel] = dblValue;
                break;

            case 35 : // LJ_ioGET_DIGITAL_PORT
                ValueDIPort = dblValue;
                break;
        }
        lngErrorcode = GetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
        if (lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE) {
            if (ErrorHandler(lngErrorcode, __LINE__, 0)) return nullptr;
        }
    }
    PostProcess();
    return readings;
}

void LJU6::PostProcess() {
    for (int i = 0; i < numChannels; i++) {
        double reading = ValueAIN[i];
        reading *= Rref; // ASSUME 4.02K Resistor  Rref  right now
        readings[i] = abs(reading / (Vref - ValueAIN[i])); // ASSUME 5 VOLTS  Vref  right now
    }
}

double *LJU6::GetAverages() {
    double lsum = 0;
    double rsum = 0;
    for (int i = 0; i < leftChannelCounts; i++) {
        lsum += readings[i];
    }
    for (int i = leftChannelCounts; i < numChannels; i++) {
        rsum += readings[i];
    }
    averages[0] = lsum / leftChannelCounts;
    averages[1] = rsum / (numChannels - leftChannelCounts);
    averages[2] = (lsum + rsum) / numChannels;
    return averages;
}

double LJU6::GetgVoltsOpen() {
    return gVoltsopen;
}

double *LJU6::GetVoltStats() {
    std::priority_queue<double> maxheap;

    for (double e: readings) {
        maxheap.push(e);
    }

    maxheap.pop();
    double max = maxheap.top();
    while (maxheap.size() > 2) {
        maxheap.pop();
    }
    double min = maxheap.top();

    stats[0] = max;
    stats[1] = min;
    stats[2] = max - min;
    return stats;
}

int LJU6::GetPassOrFail() {
    double max = stats[0];
    double min = stats[1];
    double diff = stats[2];
    return min < 190 || max > 450 || diff > 81;
}

int LJU6::ReadButton() {
    lngErrorcode = eDI(lngHandle, 1, &STARTBUTTON); // Read LabJack FIO1
    lngErrorcode = eDI(lngHandle, 1, &STOPBUTTON); //
    return STARTBUTTON;
}


extern "C" {
__declspec(dllexport) LJU6 *DAQ_Get() { return new LJU6(); }
__declspec(dllexport) int DAQ_OpenDevice(LJU6 *pLJU6) { return pLJU6->OpenDevice(); }
__declspec(dllexport) int DAQ_InitDAQ(LJU6 *pLJU6) { return pLJU6->Init(); }
__declspec(dllexport) double *DAQ_ReadData(LJU6 *pLJU6) { return pLJU6->ReadData(); }
__declspec(dllexport) double *DAQ_GetAverages(LJU6 *pLJU6) { return pLJU6->GetAverages(); }
__declspec(dllexport) double DAQ_GetgVoltsOpen(LJU6 *pLJU6) { return pLJU6->GetgVoltsOpen(); }
__declspec(dllexport) double *DAQ_GetVoltStats(LJU6 *pLJU6) { return pLJU6->GetVoltStats(); }
__declspec(dllexport) int DAQ_GetPassOrFail(LJU6 *pLJU6) { return pLJU6->GetPassOrFail(); }
__declspec(dllexport) int DAQ_ReadButton(LJU6 *pLJU6) { return pLJU6->ReadButton(); }
__declspec(dllexport) char* DAQ_GetErrorMessage(LJU6 *pLJU6, int lngErrorcode) { return pLJU6->GetErrorMessage(lngErrorcode); }
}