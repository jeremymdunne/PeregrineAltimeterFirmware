#include <SerialHandler.h> 


SerialHandlerStatus_t SerialHandler::begin(){
    // open up the serial at the specified baud 
    SERIAL_HANDLER_SOURCE.begin(SERIAL_HANDLER_BAUD_RATE); 
    // pre-empt with a connection attempt 

}

SerialHandlerStatus_t SerialHandler::update(){
    // check if a message is already available 
    if(_message_avail){
        _status = SERIAL_HANDLER_MESSAGE_AVAILABLE; 
        return _status; 
    }
    // check the incoming buffer 
    while(SERIAL_HANDLER_SOURCE.available()){
        // check if the length has been determined 
        SERIAL_HANDLER_SOURCE.readBytes(_receive_buffer,2); 
        // get the length 
        unsigned int length = _receive_buffer[0] << 8 | _receive_buffer[1]; 
        // read the rest of the message 
        SERIAL_HANDLER_SOURCE.readBytes(&_receive_buffer[2], length);       // overwrites the length bytes 
        // set the index 
        _receive_buffer_index = length + 2; 
        // check the checksum 
        unsigned int checksum = 0; 
        for(int i = 0; i < )
    }
    
}

SerialHandlerStatus_t SerialHandler::send_message(byte * buffer, int length){
    // encode the length in the first two bytes 
    // add 1 to length for the checksum 
    length ++; 
    byte length1 = (byte)(length >> 8); 
    byte length2 = (byte)length; 
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
    // return a success 
    _status = SERIAL_HANDLER_OK;
    return _status; 
}

