//
// Created by Yichao.Qin on 10/10/2023.
//

#ifndef MYLABJACK_DAQ_H
#define MYLABJACK_DAQ_H

class DAQ {
public:
    /**
     * Connect and open the data acquisition device.
     * @return error code after opening the device, 0 if success.
     */
    virtual int OpenDevice() = 0;

    /**
     * Initialize the DAQ device. Implementations varies between different devices.
     * @return error code after initialing the device, 0 if success.
     */
    virtual int Init() = 0;

    /**
     * Reads raw data and calls data post processing function.
     * If no implementations found in PostProcess funtion then the result array contains only raw data
     * @return Ptr to data array after processing, null if error.
     */
    virtual double *ReadData() = 0;

    /**
     * Gets the averages from stored data array after processing.
     * The last element is always the average of all data readings.
     * @return Ptr to data array .
     */
    virtual double *GetAverages() = 0;
    /**
     * Test
     * The last element is always the average of all data readings.
     * @return 1 if failed, 0 on success
     */
    virtual int GetPassOrFail() = 0;
};

#endif //MYLABJACK_DAQ_H
