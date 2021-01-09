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
the following are groups of data flags, the data associated with them 


*/ 






#endif 