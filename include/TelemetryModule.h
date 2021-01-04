/**
 * @file TelemetryModule.h 
 * \brief Module that handles telemetry of the altimeter 
 * 
 * This module handles storing, transmitting, and recalling data form the altimeter. 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _TELEMETRY_MODULE_H_
#define _TELEMETRY_MODULE_H_

#include <Arduino.h> 
#include <RocketState.h> 

/// Enum containing all possible responses made by the system 
typedef enum {
    TELEMETRY_MODULE_OK  = 0,                           ///< Telemetry Module Ok 
    TELEMETRY_MODULE_AVAILABLE_SERIAL_MESSAGE           ///< Available message from serial 
    TELEMETRY_MODULE_AVAILABLE_RADIO_MESSAGE            ///< Available message from radio 
    TELEMETRY_MODULE_STORAGE_COMMUNICATION_FAIL         ///< Failed to communicate with the flash storage  
    TELEMETRY_MODULE_RADIO_COMMUNICATION_FAIL           ///< Failed to communicate with the radio 
    TELEMETRY_MODULE_UNKOWN_FAIL                        ///< Unkown failure 

}   TelemetryModuleStatus_t; 

class TelemetryModule{
public: 
    /**
     * initialize all telemetry and hardware
     * 
     * @param state RocketState pointer to access data from 
     * @return error or success code 
     */ 
    TelemetryModuleStatus_t begin(RocketState *state);

    /**
     * update the telemetry module 
     * 
     * @return error or status update 
     */ 
    TelemetryModuleStatus_t update(); 

    /**
     * stop the telemetry module 
     * 
     * @return error or success code 
     */ 
    TelemetryModuleStatus_t stop(); 

private: 
    TelemetryModuleStatus_t status;         ///< Most recent status 

    /**
     * get the most recent status of the TelemetryModule
     * 
     * @return most recent status 
     */ 
    TelemetryModuleStatus_t get_status(){
        return status; 
    } 




}; 

#endif 