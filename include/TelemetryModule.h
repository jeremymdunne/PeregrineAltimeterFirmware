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
#include <JFAT.h> 
#include <HardwareDefs.h>

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

    /**
     * send an error message 
     * 
     * @param buffer message buffer
     * @param length length of the message
     * @param msg_type type of message to send 
     * @param medium medium to send the message through 
     */ 
    TelemetryModuleStatus_t send_error_string(char * buffer, int length, TelemetryMessageMedium_t medium = TELEMETRY_SERIAL); 

    /**
     * open a file to write 
     * 
     * @return status code 
     */ 
    TelemetryModuleStatus_t open_file_write(); 

    /**
     * get a string of the available files on the device 
     * 
     * @param buffer buffer to write the file list to 
     * @return length of the buffer 
     */ 
    int get_file_list_buff(byte * buffer); 

    /**
     * send the file list content through the serial 
     * 
     * @return status code
     */ 
    TelemetryModuleStatus_t send_file_list(); 

    /**
     * turn on the error led 
     */ 
    void light_error_led(); 

    /**
     * delete all files on the device 
     */ 
    void delete_all_files(); 


private: 
    TelemetryModuleStatus_t _status;            ///< Most recent status 
    RocketState *_rocket_state;                 ///< Pointer to the state of the Rocket 
    SerialHandler _serial_handler;              ///< SerialHandler object to handle serial communication 
    JFAT _flash_storage;                        ///< JFAT object to handle flash storage interaction 
    UpdateTimer _general_flight_data_timer;     ///< Timer for the general flight data updates 

    /**
     * get the most recent status of the TelemetryModule
     * 
     * @return most recent status 
     */ 
    TelemetryModuleStatus_t get_status(){
        return _status; 
    } 

    /**
     * initalize the flash storage 
     * create the required file format if not found 
     * 
     * @return status code 
     */ 
    TelemetryModuleStatus_t initialize_flash_storage();     



}; 

#endif 