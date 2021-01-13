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
#include <CommunicationProtocol.h> 

#define SERIAL_HANDLER_SOURCE                   Serial 
#define SERIAL_HANDLER_BAUD_RATE                115200 
#define SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH    256 
#define SERIAL_HANDLER_TIMEOUT                  1000
#define SERIAL_HANDLER_RESEND_ATTEMPTS          3 

typedef enum{
    SERIAL_HANDLER_OK = 0,                  //< Status is ok 
    SERIAL_HANDLER_MESSAGE_AVAILABLE,       //< Message available 
    SERIAL_HANDLER_BUFFER_TOO_SMALL,        //< Buffer provided to store the message in was too small 
    SERIAL_HANDLER_NOT_CONNECTED,           //< No device connected 
    SERIAL_HANDLER_RECEIVE_TIMEOUT,         //< Timeout on attempting to recieve a message 
    SERIAL_HANDLER_CHECKSUM_MISMATCH,       //< Mismatch on the message checksum, need to request a resend 
    SERIAL_HANDLER_RESEND_REQUEST,          //< Request to resend the last message 
    SERIAL_HANDLER_NO_RESPONSE_FROM_HOST,   //< No response received from host
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
     * if serial is not connected, will attempt a connection 
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
    SerialHandlerStatus_t _status;                                      //< Last status 
    bool _connected = false;                                            //< current connection status 
    byte _receive_buffer[SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH];         //< receive buffer to store data into   
    unsigned int _receive_buffer_index = 0;  
    bool _message_avail = false;                                


    /**
     * wait for a message recieved response 
     * responses should either be an 'ok' or a resend request
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t wait_for_message_response(); 

    /**
     * get a message from the host 
     * will timeout after default time 
     * any messages too long for the buffer will be truncated and discarded 
     * check _status after calling this function for any errors 
     *  
     * @param buffer buffer to store data in (minimum size of 2, much larger expected)
     * @param max_length maximum length allowed 
     * 
     * @return number of bytes recieved 
     */ 
    int read_message(byte * buffer, int max_length); 

    /**
     * decode a message using the standard protocol 
     * for now, this will only check the message checksum and report any errors 
     *  
     * @param buffer buffer the message is located in 
     * @param length length of the message 
     *  
     * @return status code
     */ 
    SerialHandlerStatus_t decode_message(byte * buffer, int length); 

    /**
     * request a message resend for host device 
     * 
     */ 
    void request_resend(); 

    /**
     * get the required size for a message
     * this is done for future expansion reasons 
     * 
     * @param message_size number of bytes in the message 
     * @return number of bytes needed for the protocol 
     */ 
    int determine_message_size(int message_size){return message_size + 3;} // 2 for the length, 1 for the checksum 


}; 


#endif 