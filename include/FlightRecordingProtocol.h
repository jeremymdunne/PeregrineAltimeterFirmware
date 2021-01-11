/**
 * @file FlightRecordingProtocol.h 
 * \brief standards needed for recording data 
 * 
 * Data standards, scaling factors, and formats for data being stored on the altimeter 
 * \author Jeremy Dunne 
 */ 

#ifndef _FLIGHT_RECORDING_PROTOCOL_H_
#define _FLIGHT_RECORDING_PROTOCOL_H_

#include <Arduino.h> 

struct StorableData{                        ///< Struct used to store data in a readable format 
    unsigned int data_flag;                 ///< Data flag for the data (see below)
    unsigned long time_stamp;               ///< Absolute time stamp (in milliseconds) for the data
    unsigned int length;                    ///< Length of data in the buffer 
    byte * data;                            ///< Data to be stored 
}; 

/*
General storage location pointers
*/ 
#define STORAGE_DEFINITIONS_LOCATION 1      ///< File location reserved for any program definitions 

/* 
the following are groups of data flags, the data associated with them 


*/ 

#define STORAGE_GENERAL_FLIGHT_STORAGE_FLAG 20          ///< General flight data flag 
#define STORAGE_GENERAL_FLIGHT_STORAGE_LENGTH 
/* 
data contents as follows: 
    ACCEL_X: (1 byte) scaled 0 - 10 G 
    ACCEL_Y: (1 byte) scaled 0 - 10 G
    ACCEL_Z: (1 byte) scaled 0 - 10 G
    GYRO_X: (1 byte) scaled -720 - 720 deg/s 
    GYRO_Y: (1 byte) scaled -720 - 720 deg/s 
    GYRO_Z: (1 byte) scaled -720 - 720 deg/s
    MAG_X: (1 byte) 
    MAG_Y: (1 byte) 
    MAG_Z: (1 byte) 
    BARO_PRESSURE: (3 byte) scaled 80000 - 100000  
    BARO_TEMP: (2 byte) scaled -50 - 50 C
    ALT: (2 byte) scaled -50 - 10000 
    VEL: (2 byte) scaled 0 - 1000 
*/ 


#endif 