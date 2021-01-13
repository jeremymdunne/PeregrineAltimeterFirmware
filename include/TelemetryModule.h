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
#include <FlightRecordingProtocol.h> 
#include <JFAT.h> 
#include <HardwareDefs.h>
#include <Configurations.h> 
 
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
     * start recording data 
     * opens a file in the flash storage 
     * starts all timers for data storage 
     * @return status code
     */ 
    TelemetryModuleStatus_t start_recording(); 

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

    /**
     * get the most recent status of the TelemetryModule
     * 
     * @return most recent status 
     */ 
    TelemetryModuleStatus_t get_status(){
        return _status; 
    } 

    
    /**
     * write a storable data 
     * also handles looping time 
     * this function must be used for all data storage callouts 
     * 
     * @param storable data structure to store 
     * @return status code
     */
    TelemetryModuleStatus_t store_data(StorableData * data); 

    /**
     * send the last file to a host 
     * mainly used for debugging purposes 
     */ 
    void send_last_file(); 



private: 
    TelemetryModuleStatus_t _status;            ///< Most recent status 
    
    RocketState *_rocket_state;                 ///< Pointer to the state of the Rocket 
    SerialHandler _serial_handler;              ///< SerialHandler object to handle serial communication 
    JFAT _flash_storage;                        ///< JFAT object to handle flash storage interaction 
    unsigned long _last_time_update = 0;        ///< Last time an time loop was stored 

    enum STORAGE_STATE{
        STORAGE_STORE_DATA,                     ///< Data storage as normal 
        STORAGE_DATA_STORAGE_COMPLETE,          ///< Data storage has been completed and file closed 
        STORAGE_DATA_NO_STORAGE                 ///< No data storage (typcial sign of data recall or preflight checks)
    }; 


    STORAGE_STATE _storage_state = STORAGE_DATA_NO_STORAGE;  

    // data storage timers 
    UpdateTimer _general_flight_data_timer;     ///< Timer for the general flight data updates 

    /**
     * update all storage timers 
     * @return status code 
     */ 
    TelemetryModuleStatus_t update_storage_data(); 

    /**
     * store a time loop flag 
     */ 
    void store_time_loop(); 

    /**
     * encode a header sequence 
     * @param flag data flag 
     * @param time_stamp time stamp (must be < 2 ^ 16)
     * @param data_length length of the data following the message 
     * @param buff buffer to store the header data in 
     */ 
    void encode_header(int flag, unsigned long time_stamp, int data_length, byte * buff);  

    /**
     * initalize the flash storage 
     * create the required file format if not found 
     * 
     * @return status code 
     */ 
    TelemetryModuleStatus_t initialize_flash_storage();     

    /**
     * encode general flight data 
     * 
     */ 
    void encode_general_flight_data(StorableData *data); 

    /**
     * scale and store data to a byte format 
     * @param data data to encode 
     * @param buff buffer to encode to 
     * @param resoultion resolutino to encode by 
     * @param byte_size size (in bytes) to encode by 
     * @param max_value maximum value 
     * @param min_value minimum value 
     */ 
    void convert_float_to_bytes(float data, byte * buff, float resolution, int byte_size, float max_value, float min_value); 

    /**
     * convert to float 
     * @param buff buff to convert 
     * @param resolution resolution of the data 
     * @param byte_size size of the data 
     * @param min_value min value of the data 
     */ 
    float convert_to_float(byte * buff, float resolution, int byte_size, float min_value); 



    /**
     * recall a file 
     * print contents through the serial handler 
     * @param fd file index 
     * @return status code 
     */ 
    TelemetryModuleStatus_t recall_file(int fd); 


}; 

#endif 