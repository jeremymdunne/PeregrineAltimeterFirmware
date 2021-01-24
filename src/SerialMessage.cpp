#include <SerialMessage.h> 



SerialMessage * newSerialMessage(int message_size){
    SerialMessage *message = (SerialMessage *) malloc(sizeof(SerialMessage)); 
    message->data = (byte *) malloc(message_size * sizeof(byte)); 
    message->data_length = message_size; 
    return message; 
}

SerialMessage * newSerialMessage(int message_flag, int message_size){
    SerialMessage *message = newSerialMessage(message_size);  
    message->message_flag = message_flag; 
    return message; 
}

void delSerialMessage(SerialMessage *message){
    if(message != NULL){
        free(message->data); 
        free(message); 
    }
}