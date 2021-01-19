#include <SerialHandler.h> 


SerialHandlerStatus_t SerialHandler::begin(){
    // open up the serial at the specified baud 
    SERIAL_HANDLER_SOURCE.begin(SERIAL_HANDLER_BAUD_RATE); 
    // pre-empt with a connection attempt 
    _status = SERIAL_HANDLER_OK; 
    return _status; 
}

int SerialHandler::get_message(byte * buffer){
    if(!_message_avail) return 0; 
    // copy over the message contents 
    memcpy(buffer, &_receive_buffer[2], _receive_buffer_index - 2); 
    int len = _receive_buffer_index - 2; 
    _receive_buffer_index = 0; 
    // flag as no message available 
    _message_avail = false; 
    // set the status to ok 
    _status = SERIAL_HANDLER_OK; 
    return len; 
}

int SerialHandler::get_message_length(){
    if(_message_avail){
        return _receive_buffer_index - 2; 
    }
    return 0; 
}

SerialHandlerStatus_t SerialHandler::update(){
    // check if a message is already available 
    if(_message_avail){
        _status = SERIAL_HANDLER_MESSAGE_AVAILABLE; 
        return _status; 
    }
    // check the incoming buffer 
    while(SERIAL_HANDLER_SOURCE.available()){
        // get a message 
        int msg_length = read_message(_receive_buffer, SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH); 
        // decode it 
        SerialHandlerStatus_t resp = decode_message(_receive_buffer, msg_length); 
        _receive_buffer_index = msg_length - 1; // remove the checksum 
        if(resp == SERIAL_HANDLER_CHECKSUM_MISMATCH){
            // ask for a resend 
            request_resend(); 
            _status = SERIAL_HANDLER_OK; 
            return _status; 
        }
        else if(resp == SERIAL_HANDLER_OK){
            // check the code for a connection request 
            if(_receive_buffer[2] == COMMUNICATION_CONNECTION_REQUEST_FLAG){
                // update the state 
                _connected = true; 
                // send a connected message 
                byte connection_success = COMMUNICATION_CONNECTION_SUCCESS_FLAG; 
                send_message(&connection_success, 1); 
                _status = SERIAL_HANDLER_OK; 
                return _status; 
            }
            
            _message_avail = true;
            _status = SERIAL_HANDLER_MESSAGE_AVAILABLE; 
            return _status; 
        }
    }
    if(!_connected) send_connection_request(); // attempt to connect with the host 
    return _status; 
}

SerialHandlerStatus_t SerialHandler::send_message(byte * buffer, int length){
    // encode the length in the first two bytes 
    int msg_length = length + 1; // one extra for the checksum 
    byte length1 = (byte)(msg_length >> 8); 
    byte length2 = (byte)msg_length; 
    // write the header  
    SERIAL_HANDLER_SOURCE.write(length1); 
    SERIAL_HANDLER_SOURCE.write(length2);
    // place holder for checksum 
    // checksum includes the length bytes 
    unsigned long sum = length1 + length2; // for larger messages, long instead of int 
    for(int i = 0; i < length; i++){
        sum += buffer[i]; // add for the checksum 
        SERIAL_HANDLER_SOURCE.write(buffer[i]); // write the buffer element 
    }
    // write the checksum 
    SERIAL_HANDLER_SOURCE.write((byte)sum); 

    // wait for response and resend if neccessary 
    int resend_count = 0; 
    for(; resend_count < SERIAL_HANDLER_RESEND_ATTEMPTS; resend_count ++){
        // wait for a response 
        SerialHandlerStatus_t resp = wait_for_message_response(); 
        switch(resp){
            case(SERIAL_HANDLER_OK):
                // send good, escape 
                _status = SERIAL_HANDLER_OK; 
                return _status; 
                break; 
            case(SERIAL_HANDLER_RESEND_REQUEST):
                // resend request 
                SERIAL_HANDLER_SOURCE.write(length1); // length byte 1 
                SERIAL_HANDLER_SOURCE.write(length2); // legnth byte 2 
                SERIAL_HANDLER_SOURCE.write(buffer, length); // message 
                SERIAL_HANDLER_SOURCE.write((byte)sum); // checksum 
                break; 
            default: 
                // error state 
                _status = resp; 
                return resp; 
                break; 
        }

    }
    
    return _status; 
}


SerialHandlerStatus_t SerialHandler::wait_for_message_response(){
    // get a message 
    read_message(_receive_buffer, SERIAL_HANDLER_RECIEVE_BUFFER_LENGTH); 
    // handle a connection request here 
    if(_receive_buffer[2] == COMMUNICATION_CONNECTION_SUCCESS_FLAG){
        _connected = true;
        return wait_for_message_response(); 
    }
    // check the status 
    switch(_status){
        case(SERIAL_HANDLER_BUFFER_TOO_SMALL):
            // this should not happen but should be accomadated in the future 
            // TODO
            return _status; 
            break; 
        default: 
            break; 
    }
    
    // parse the message for the ok or a resend request 
    byte data = _receive_buffer[3];         // 3rd byte holds the actual data 
    switch(data){
        case(COMMUNICATION_SEND_OK):
            return SERIAL_HANDLER_OK; 
        case(COMMUNICATION_RESEND_REQUEST):
            return SERIAL_HANDLER_RESEND_REQUEST; 
        default: 
            return SERIAL_HANDLER_UNKNOWN_ERROR; 
    }
    return SERIAL_HANDLER_UNKNOWN_ERROR; 
}


SerialHandlerStatus_t SerialHandler::decode_message(byte * buffer, int length){
    // compute the checksum 
    // 8 bit checksum, ignore the last byte of the message (where the checksum already is)
    unsigned int checksum = 0; 
    for(int i = 0; i < length - 1; i ++){
        checksum += buffer[i]; 
    }
    // compare the checksume 
    if(buffer[length-1] != (byte)checksum){
        // checksum mismatch 
        _status = SERIAL_HANDLER_CHECKSUM_MISMATCH; 
        return _status; 
    }
    else{
        _status = SERIAL_HANDLER_OK; 
        return _status; 
    }
}

int SerialHandler::read_message(byte *buffer, int max_length){
    // start a timer for timeout purposes 
    long start_time = millis(); 
    while(!SERIAL_HANDLER_SOURCE.available() && millis() - start_time < SERIAL_HANDLER_TIMEOUT); 
    if(!SERIAL_HANDLER_SOURCE.available()){
        // set the status 
        _status = SERIAL_HANDLER_RECEIVE_TIMEOUT; 
        return -1; 
    }
    // otherwise, read in the message 
    // read the length 
    SERIAL_HANDLER_SOURCE.readBytes((char *)buffer,2); 
    // get the length 
    int length = buffer[0] << 8 | buffer[1]; 
    // sanity check length here 
    if(length > (max_length - 2)){    // account for the length bytes in the message 
        // override the length to read in the entire message 
        SERIAL_HANDLER_SOURCE.readBytes((char *)&buffer[2], max_length - 2); 
        // read in the rest of the message into a null buffer 
        byte temp_buffer[length - (max_length - 2)]; 
        SERIAL_HANDLER_SOURCE.readBytes((char *)temp_buffer, length - (max_length - 2)); 
        // set the status 
        _status = SERIAL_HANDLER_BUFFER_TOO_SMALL; 
        return max_length; 
    }
    else{   // buffer is large enough 
        SERIAL_HANDLER_SOURCE.readBytes((char *)&buffer[2], length);
        _status = SERIAL_HANDLER_OK; 
        return length + 2; 
    }
    return -1;
}

void SerialHandler::request_resend(){
    // construct the simple message 
    byte resend_command = COMMUNICATION_RESEND_REQUEST; 
    send_message(&resend_command, 1); 
}

void SerialHandler::send_connection_request(){
    // write a connection attempt 
    byte request = COMMUNICATION_CONNECTION_REQUEST_FLAG; 
    send_message(&request, 1);  
}