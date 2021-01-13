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


#define STORAGE_DATA_HEADER_LENGTH 4        ///< Length of the entire header 
#define STORAGE_DATA_FLAG_LENGTH 1          ///< length of just the data flag in the header 
#define STORAGE_DATA_TIMESTAMP_LENGTH 2     ///< length of the timestamp in the header 
#define STORAGE_DATA_LENGTH_LENGTH 1        ///< length of the data length in the header 
/*
All data shall be stored in the following format for a header: 
    Data Flag (1 byte): Data flag corresponding to the data being stored 
    Time Stamp (2 bytes): Time stamp, looped using a flag described below 
    length (1 byte): Length of the data (excluding the header)

Following the header, the data will be written
    data (n bytes) 

*/ 



/*


General storage location pointers
*/ 
#define STORAGE_DEFINITIONS_LOCATION 1      ///< File location reserved for any program definitions 

/* 
the following are groups of data flags, the data associated with them 


*/ 

 

//*******************************Data flags******************************* 

// system flags 
#define STORAGE_TIME_LOOP_FLAG  5                       ///< Data loop flag  
#define STORAGE_TIME_LOOP_LENGTH 0  
/*
    No data associated with the time loop flag 
    Represents the time has looped back over (increment following timestamps by 2^16 milliseconds)
*/



#define STORAGE_GENERAL_FLIGHT_STORAGE_FLAG 20          ///< General flight data flag 
#define STORAGE_GENERAL_FLIGHT_STORAGE_LENGTH 17        
/* 
data contents as follows: 
    ACCEL_X: (1 byte) scaled -5 - 5 G 
    ACCEL_Y: (1 byte) scaled 0 - 10 G
    ACCEL_Z: (1 byte) scaled 0 - 10 G
    GYRO_X: (1 byte) scaled -720 - 720 deg/s 
    GYRO_Y: (1 byte) scaled -720 - 720 deg/s 
    GYRO_Z: (1 byte) scaled -720 - 720 deg/s
    MAG_X: (1 byte) 
    MAG_Y: (1 byte) 
    MAG_Z: (1 byte) 
    BARO_PRESSURE: (3 byte) scaled 80000 - 110000  
    BARO_TEMP: (1 byte) scaled -50 - 50 C
    ALT: (2 byte) scaled -50 - 10000 
    VEL: (2 byte) scaled 0 - 1000 
*/ 

#define STORAGE_GENERAL_FLIGHT_ACCEL_BYTE_SIZE 1 
#define STORAGE_GENERAL_FLIGHT_ACCEL_MIN_VALUE -5 
#define STORAGE_GENERAL_FLIGHT_ACCEL_MAX_VALUE 5 
#define STORAGE_GENERAL_FLIGHT_ACCEL_RESOLUTION 0.0390625

#define STORAGE_GENERAL_FLIGHT_GYRO_BYTE_SIZE 1 
#define STORAGE_GENERAL_FLIGHT_GYRO_MIN_VALUE -720 
#define STORAGE_GENERAL_FLIGHT_GYRO_MAX_VALUE 720 
#define STORAGE_GENERAL_FLIGHT_GYRO_RESOLUTION 5.625

#define STORAGE_GENERAL_FLIGHT_MAG_BYTE_SIZE 1 
#define STORAGE_GENERAL_FLIGHT_MAG_MIN_VALUE -60 
#define STORAGE_GENERAL_FLIGHT_MAG_MAX_VALUE 60 
#define STORAGE_GENERAL_FLIGHT_MAG_RESOLUTION 0.46875

#define STORAGE_GENERAL_FLIGHT_PRESSURE_BYTE_SIZE 1 
#define STORAGE_GENERAL_FLIGHT_PRESSURE_MIN_VALUE 80000 
#define STORAGE_GENERAL_FLIGHT_PRESSURE_MAX_VALUE 110000 
#define STORAGE_GENERAL_FLIGHT_PRESSURE_RESOLUTION 0.00178813934 

#define STORAGE_GENERAL_FLIGHT_TEMPERATURE_BYTE_SIZE 1 
#define STORAGE_GENERAL_FLIGHT_TEMPERATURE_MIN_VALUE -50 
#define STORAGE_GENERAL_FLIGHT_TEMPERATURE_MAX_VALUE 50 
#define STORAGE_GENERAL_FLIGHT_TEMPERATURE_RESOLUTION 0.390625 

#define STORAGE_GENERAL_FLIGHT_ALT_BYTE_SIZE 2 
#define STORAGE_GENERAL_FLIGHT_ALT_MIN_VALUE -50 
#define STORAGE_GENERAL_FLIGHT_ALT_MAX_VALUE 10000 
#define STORAGE_GENERAL_FLIGHT_ALT_RESOLUTION 0.1533508 

#define STORAGE_GENERAL_FLIGHT_VEL_BYTE_SIZE 2 
#define STORAGE_GENERAL_FLIGHT_VEL_MIN_VALUE -0 
#define STORAGE_GENERAL_FLIGHT_VEL_MAX_VALUE 1000 
#define STORAGE_GENERAL_FLIGHT_VEL_RESOLUTION 0.015258789 




#endif 