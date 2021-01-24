


#ifndef _SERIAL_MESSAGE_H_
#define _SERIAL_MESSAGE_H_

#include <Arduino.h> 


struct SerialMessage{
    int message_flag; 
    int data_length; 
    byte * data; 
}; 

/**
 * create a new SerialMessage struct
 * @param message_size size of the message 
 * @return pointer to the SerialMessage struct 
 */ 
SerialMessage * newSerialMessage(int message_size); 


/**
 * create a new SerialMessage struct 
 * @param message_flag flag for the message 
 * @param message_size size of the message 
 * @return pointer to the SerialMessage struct 
 */ 
SerialMessage * newSerialMessage(int message_flag, int message_size);
 

void delSerialMessage(SerialMessage *message);




#endif 