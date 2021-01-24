#include <SerialHandler.h> 


SerialHandlerStatus_t SerialHandler::begin(){
    // initalize the serial 
    SERIAL_HANDLER_SOURCE.begin(SERIAL_HANDLER_BAUD_RATE); 
    // return ok 
    return SERIAL_HANDLER_OK; 
}


SerialHandlerStatus_t SerialHandler::update(){
    // check if a message is available 
    if(_message_available) return SERIAL_HANDLER_MESSAGE_AVAILABLE; 

    if(SERIAL_HANDLER_SOURCE.available() > 0){
        // snag the message 
        delSerialMessage(_received_message); // del the serial message held here 
        _received_message = read_message(); 
        if(_received_message != NULL){
            // check for a low-level 
            switch(_received_message->message_flag){
                case(COMMUNICATION_CONNECTION_REQUEST_FLAG): {
                    // set connection status to true 
                    _connected = true; 
                    // send a communication success 
                    SerialMessage * resp = newSerialMessage(COMMUNICATION_CONNECTION_SUCCESS_FLAG,0); 
                    send_message(resp); 
                    delSerialMessage(resp); 
                } break;
                default: 
                    // raise that a message is available 
                    _message_available = true; 
                    // return message avialble flag 
                    return SERIAL_HANDLER_MESSAGE_AVAILABLE; 
                    break; 
            }
        }
    }
    return SERIAL_HANDLER_OK; 
}

SerialMessage * SerialHandler::get_message(){
    // return the message available 
    // make a copy of the message 
   
    if(_message_available){
         SerialMessage * msg = newSerialMessage(_received_message->message_flag, _received_message->data_length); 
        // cpy the data 
        memcpy(msg->data, _received_message->data, msg->data_length); 
        _message_available = false; 
        return msg; 
    }
    return NULL; 
}


SerialHandlerStatus_t SerialHandler::send_message(SerialMessage *message){
    // convert into a buffer and send the buffer 
    int send_buffer_length = message->data_length + 3; // 2 bytes for length, 1 data flag
    byte send_buffer[send_buffer_length];
    // encode the length 
    send_buffer[0] = (byte)((message->data_length + 1) >> 8); 
    send_buffer[1] = (byte)(message->data_length + 1); 
    send_buffer[2] = (byte)message->message_flag; 
    memcpy(&send_buffer[3],message->data, message->data_length); 
    // write the buffer 
    SerialHandlerStatus_t write_status = write_buffer(send_buffer, send_buffer_length); 
    return write_status; 
}


SerialHandlerStatus_t SerialHandler::write_buffer(byte * buffer, int length){
    // write to the serial
    SERIAL_HANDLER_SOURCE.write(buffer, length);  
    // send failed 
    return SERIAL_HANDLER_OK; 
}



SerialMessage * SerialHandler::read_message(){
    bool done = false; 
    long start_time = millis(); 
    while(!done && millis() - start_time < SERIAL_HANDLER_TIMEOUT){
        // wait for a message or timeout 
        while(SERIAL_HANDLER_SOURCE.available() == 0 && millis() - start_time < SERIAL_HANDLER_TIMEOUT); 
        if(SERIAL_HANDLER_SOURCE.available() > 0){
            // read the message 
            // read in the length bytes 
            byte header_bytes[3]; 
            SERIAL_HANDLER_SOURCE.read(header_bytes, 3); 
            int length = header_bytes[0] << 8 | header_bytes[1] - 1; // remove the flag
            int flag = header_bytes[2]; 
            SerialMessage *message = newSerialMessage(flag, length); 
            // copy in the contents 
            SERIAL_HANDLER_SOURCE.read(message->data, message->data_length); 
            // read the crc 
            _status = SERIAL_HANDLER_OK;
            return message;  
        }
    }    
    // set a flag 
    _status = SERIAL_HANDLER_READ_TIMEOUT; 
    return NULL; 
}

void SerialHandler::send_message_ok_reciept(){
    // construct the message 
    SerialMessage * message = newSerialMessage(COMMUNICATION_SEND_OK, 0); 
    // write the message 
    send_message(message); 
    // del the message 
    delSerialMessage(message); 
}

void SerialHandler::send_resend_request(){
    // construct the message 
    SerialMessage * message = newSerialMessage(COMMUNICATION_RESEND_REQUEST, 0); 
    // write the message 
    send_message(message); 
    // del the message 
    delSerialMessage(message); 
}
