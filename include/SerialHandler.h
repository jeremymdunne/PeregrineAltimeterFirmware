/**
 * @file SerialHandler.h 
 * 
 * \brief Handles all incoming and outgoing serial communication 
 * 
 */ 

#ifndef _SERIAL_HANDLER_H_
#define _SERIAL_HANDLER_H_

#include <Arduino.h> 
#include <CommunicationProtocol.h> 
#include <SerialMessage.h> 

#define SERIAL_HANDLER_SOURCE                   Serial 
#define SERIAL_HANDLER_BAUD_RATE                115200 
#define SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH    256 
#define SERIAL_HANDLER_TIMEOUT                  1000
#define SERIAL_HANDLER_RESEND_ATTEMPTS          100 




typedef enum{
    SERIAL_HANDLER_OK = 0,              ///< Serial Handler ok
    SERIAL_HANDLER_CHECKSUM_MISMATCH,   ///< Checksum mismatch
    SERIAL_HANDLER_SEND_FAILED,             ///< Failed to send the message 
    SERIAL_HANDLER_RESEND_LIMIT_REACHED,    ///< Resend limit reached 
    SERIAL_HANDLER_READ_TIMEOUT,        ///< Timeout on a read attempt  
    SERIAL_HANDLER_NO_RESPONSE,         ///< No response on a message read  
    SERIAL_HANDLER_MESSAGE_AVAILABLE    ///< Message available

} SerialHandlerStatus_t; 


class SerialHandler{
public: 
    /**
     * initialize the serial device 
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t begin(); 

    /**
     * update the serial 
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t update(); 

    /**
     * send a message over serial 
     * 
     * @param message message to send 
     * @return status code 
     */ 
    SerialHandlerStatus_t send_message(SerialMessage *message); 

    /**
     * get the connection status of the serial 
     * @return true for connected, false for not 
     */ 
    bool is_connected() { return _connected; } 

    /**
     * get the available message 
     *  
     * @return pointer to the avialable message
     */  

    SerialMessage * get_message(); 

    /**
     * stop serial communication 
     * 
     * @return status code 
     */ 
    SerialHandlerStatus_t stop(); 

    int _resend_attemps = 0; 

private: 
    bool _connected = false; 
    bool _message_available = false; 
    SerialHandlerStatus_t _status;
    SerialMessage * _received_message = NULL; 


    /**
     * write a buffer to the serial
     * wait for a message receipt and resend if necessary 
     * 
     * @param buffer pointer to the buffer 
     * @param length number of bytes to write 
     * @return status code 
     */ 
    SerialHandlerStatus_t write_buffer(byte * buffer, int length); 

    /**
     * read in a message from serial 
     * 
     * @param buffer buffer to place the message into 
     * @param max_length maximum length of the buffer 
     * @return number of bytes written to the buffer 
     */ 
    int read_serial(byte * buffer, int max_length); 

    /**
     * read in a message from serial 
     *  
     * @return pointer to the recieved message 
     */ 
    SerialMessage * read_message(); 


    /**
     * send a message ok reciept 
     */ 
    void send_message_ok_reciept(); 

    /**
     * send a message rescend reciept 
     */ 
    void send_resend_request(); 



}; 


#endif 