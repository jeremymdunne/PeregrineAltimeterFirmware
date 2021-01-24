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
                    break; } 
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
    int send_buffer_length = message->data_length + 4; // 2 bytes for length, 1 data flag, 1 checksum 
    byte send_buffer[send_buffer_length];
    // encode the length 
    send_buffer[0] = (byte)((message->data_length + 2) >> 8); 
    send_buffer[1] = (byte)(message->data_length + 2); 
    send_buffer[2] = (byte)message->message_flag; 
    // calculate the checksum and copy buffer contents 
    unsigned long crc = send_buffer[0] + send_buffer[1] + send_buffer[2]; 
    for(int i = 0; i < message->data_length; i ++){
        crc += message->data[i]; 
        send_buffer[i + 3] = message->data[i]; 
    }
    send_buffer[send_buffer_length - 1] = (byte)crc; 
    // write the buffer 
    SerialHandlerStatus_t write_status = write_buffer(send_buffer, send_buffer_length); 
    return write_status; 
}


SerialHandlerStatus_t SerialHandler::write_buffer(byte * buffer, int length){
    // write to the serial 
    int send_attempt = 0; 
    for(; send_attempt < SERIAL_HANDLER_RESEND_ATTEMPTS; send_attempt ++){
        SERIAL_HANDLER_SOURCE.write(buffer, length); 
        SerialMessage *message = read_message(); 
        if(message != NULL){
            switch(message->message_flag){
                case(COMMUNICATION_SEND_OK):
                    // message send ok 
                    delSerialMessage(message); 
                    return SERIAL_HANDLER_OK; 
                    break; 
                case(COMMUNICATION_RESEND_REQUEST):
                    delSerialMessage(message); 
                    // loop to resend 
                    break; 
            }
        }
    }
    // send failed 
    return SERIAL_HANDLER_SEND_FAILED; 
}



SerialMessage * SerialHandler::read_message(){
    bool done = false; 
    long start_time = millis(); 
    int send_attempt = 0; 
    while(!done && millis() - start_time < SERIAL_HANDLER_TIMEOUT && send_attempt < SERIAL_HANDLER_RESEND_ATTEMPTS){
        // wait for a message or timeout 
        while(SERIAL_HANDLER_SOURCE.available() == 0 && millis() - start_time < SERIAL_HANDLER_TIMEOUT); 
        if(SERIAL_HANDLER_SOURCE.available() > 0){
            // read the message 
            // read in the length bytes 
            byte header_bytes[3]; 
            SERIAL_HANDLER_SOURCE.read(header_bytes, 3); 
            int length = header_bytes[0] << 8 | header_bytes[1] - 2; // remove the flag and the checksum  
            int flag = header_bytes[2]; 
            SerialMessage *message = newSerialMessage(flag, length); 
            // copy in the contents 
            SERIAL_HANDLER_SOURCE.read(message->data, message->data_length); 
            // read the crc 
            byte crc; 
            SERIAL_HANDLER_SOURCE.read(&crc, 1); 
            // check if the checksums match 
            int check = header_bytes[2] + header_bytes[1] + header_bytes[0]; 
            for(int i = 0; i < message->data_length; i ++){
                check += message->data[i]; 
            }
            if(crc != (byte)check){
                // checksum mismatch 
                // send a resend request 
                if(send_attempt + 1 < SERIAL_HANDLER_RESEND_ATTEMPTS){
                    send_resend_request(); 
                }
                send_attempt ++; 
            }
            else{
                // successfull message response 
                // return the pointer 
                _status = SERIAL_HANDLER_OK;
                return message;  
            }
        }    
    }
    // set a flag 
    if(send_attempt >= SERIAL_HANDLER_RESEND_ATTEMPTS){
        // requested a resend, something happend 
        _status = SERIAL_HANDLER_RESEND_LIMIT_REACHED; 
    }
    else _status = SERIAL_HANDLER_READ_TIMEOUT; 
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
