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
#include <SerialHandler.h> 
#include <UpdateTimer.h> 
#include <CommunicationProtocol.h> 

/// Enum containing all possible responses made by the system 
typedef enum {
    TELEMETRY_MODULE_OK  = 0,                           ///< Telemetry Module Ok 
    TELEMETRY_MODULE_AVAILABLE_SERIAL_MESSAGE,          ///< Available message from serial 
    TELEMETRY_MODULE_AVAILABLE_RADIO_MESSAGE,           ///< Available message from radio 
    TELEMETRY_MODULE_STORAGE_COMMUNICATION_FAIL,        ///< Failed to communicate with the flash storage  
    TELEMETRY_MODULE_RADIO_COMMUNICATION_FAIL,          ///< Failed to communicate with the radio 
    TELEMETRY_MODULE_UNKOWN_FAIL,                       ///< Unkown failure 

}   TelemetryModuleStatus_t; 

/// Enum for message mediums 
typedef enum{
    TELEMETRY_SERIAL = 0,                               ///< Send data through Serial 
    TELEMETRY_RADIO,                                    ///< Send data through Radio 
    TELEMETRY_ALL                                       ///< Send data through all communication mediums 
}   TelemetryMessageMedium_t; 

typedef enum{
    TELEMETRY_VERBOSE_MESSAGE = 0,                      ///< Verbose message 
    TELEMTERY_ERROR_MESSAGE                            ///< Error Message
}   TelemetryStringMessageType_t;  

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

    /**
     * send a verbose message 
     * 
     * @param buffer message buffer
     * @param length length of the message
     * @param msg_type type of message to send 
     * @param medium medium to send the message through 
     */ 
    TelemetryModuleStatus_t send_verbose_string(char * buffer, int length, TelemetryMessageMedium_t medium = TELEMETRY_SERIAL); 

private: 
    TelemetryModuleStatus_t _status;            ///< Most recent status 
    RocketState *_rocket_state;                 ///< Pointer to the state of the Rocket 
    SerialHandler _serial_handler;              ///< SerialHandler object to handle serial communication 

    UpdateTimer _general_flight_data_timer;     ///< Timer for the general flight data updates 

    /**
     * get the most recent status of the TelemetryModule
     * 
     * @return most recent status 
     */ 
    TelemetryModuleStatus_t get_status(){
        return _status; 
    } 




}; 

#endif 