/**
 * @file SerialHandler.h 
 * \brief Handles serial communication 
 * 
 * This class handles serial communication with the altimeter and any connected serial devices (groundstations)
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _SERIAL_HANDLER_H_
#define _SERIAL_HANDLER_H_

#include <Arduino.h> 

#define SERIAL_HANDLER_SOURCE                   Serial 
#define SERIAL_HANDLER_BAUD_RATE                115200 
#define SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH    256 

typedef enum{
    SERIAL_HANDLER_OK = 0,                  //< Status is ok 
    SERIAL_HANDLER_MESSAGE_AVAILABLE,       //< Message available 
    SERIAL_HANDLER_BUFFER_TOO_SMALL,        //< Buffer provided to store the message in was too small 
    SERIAL_HANDLER_NOT_CONNECTED,           //< No device connected 
    SERIAL_HANDLER_UNKNOWN_ERROR            //< Unkown error occurred 
} SerialHandlerStatus_t; 



class SerialHandler{
public: 
    /**
     * initialize serial communications 
     *  
     * @return status code 
     */ 
    SerialHandlerStatus_t begin(); 

    /**
     * update the serial and check for messages 
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t update(); 

    /**
     * close the serial communication (if opened)
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t close(); 

    /**
     * get the last status 
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t get_status(){ return _status; } 

    /**
     * send a message over serial
     * 
     * Handles communication handshake (length in first two bytes, checksum at end)
     * 
     * @param buffer message to send 
     * @param length length of message to send 
     * 
     * @return status code
     */ 
    SerialHandlerStatus_t send_message(byte * buffer, int length); 

    /** 
     * get the most recent serial message 
     * 
     * @param buffer to copy the contents into 
     * @param max_length maximum length of the buffer 
     * 
     * @return length of the message or -1 if the buffer was too small 
     */ 
    int get_message(byte * buffer, int max_length); 

    /**
     * attempt a connection
     * 
     * does not wait until a connection is confirmed 
     */ 
    void send_connection_request(); 


private: 
    SerialHandlerStatus_t _status;                                       //< Last status 
    bool _connected = false;                                            //< current connection status 
    byte _receive_buffer[SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH];         //< receive buffer to store data into   
    unsigned int _receive_buffer_index = 0;  
    bool _message_avail = false;                                

}; 


#endif 