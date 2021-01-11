#include <JFAT.h> 

int JFAT::begin(int cs_pin){
    // initialize the storage 
    int storage_status = _storage.begin(cs_pin); 
    if(storage_status < 0) return storage_status; 
    wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT);

    // check for the existance of a fat table 
    FileList list; 
    storage_status = read_fat(&list);
    if(storage_status < 0){
        // no fat table 
        // write one 
        FileList list; 
        list.num_files = 0;
        write_fat(&list); 
    }  
    return 0; 
}

int JFAT::delete_last_file(){
    // delete the last file 
    // decrease the number of files in the table 
    FileList list; 
    read_fat(&list); 
    if(list.num_files > 0) list.num_files --; 
    else return -1; // no files to erase 
    write_fat(&list); 
    return 0; 
}

int JFAT::delete_all_files(){
    // write a fat table with no info 
    FileList list; 
    list.num_files = 0; 
    write_fat(&list); 
    return 0; 
}

int JFAT::open_file_read(unsigned int fd){
    // make sure the fd is valid 
    if(_cur_state != NONE) return -1; 
    FileList list; 
    read_fat(&list); 
    // check the num files 
    if(list.num_files <= fd) return -1; 
    // otherwise, read in the file data 
    _cur_address = list.start_pages[fd] * 256; 
    _file_last_address = list.start_pages[fd] * 256 + list.page_length[fd] * 256 + list.page_offset[fd] + 1; // address to read up to 
    _cur_state = READ; 
    #ifdef JFAT_DEBUG 
        Serial.println("JFAT OPENING FILE " + String(fd) + " AT " + String(_cur_address) + " UP TO " + String(_file_last_address)); 
    #endif
    return 0; 
}

int JFAT::read(uint8_t *buffer, unsigned int len){
    if(_cur_state != READ) return -1; 
    // check that the len doesn't violate the maximum size 
    if(_file_last_address <= _cur_address + len) len = _file_last_address - _cur_address; 
    // read 
    read_storage(_cur_address, buffer, len); 
    _cur_address += len; 
    return 0; 
}

uint32_t JFAT::peek(){
    // return the number of bytes left in the file 
    //Serial.println("Last Address")
    if(_cur_state != READ) return 0; // prevent errors 
    return _file_last_address - _cur_address; 
}


int JFAT::write(uint8_t *buffer, unsigned int len){
    // fill an active buffer before transferring to the device 
    // first, check if the buffer can be filled 
    erase_space(len); 
    if(len + _storage_buffer_index >= JFAT_BUFFER_SIZE && _storage_buffer_index != 0){
        // some checky things can be done here... 
        // fill up the storage buffer to the full amount 
        // then send over the remaining buffer 
        memcpy(&_storage_buffer[_storage_buffer_index], buffer, JFAT_BUFFER_SIZE - _storage_buffer_index); 
        // send this buffer 
        write_storage(_cur_address, _storage_buffer, JFAT_BUFFER_SIZE); 
        _cur_address += JFAT_BUFFER_SIZE; 
        // increment the buffer 
        buffer += JFAT_BUFFER_SIZE - _storage_buffer_index; 
        len -= JFAT_BUFFER_SIZE - _storage_buffer_index; 
        _storage_buffer_index = 0; 
        // check for a erase cycle 
    }
    // two options, buffer can be added directly to the storage 
    // or buffer can be written directly 
    if(len > 256 && _storage_buffer_index == 0){
        // find the maximum 256 multiple of it 
        unsigned int write_size = (len >> 8) << 8;
        // write it 
        #ifdef JFAT_DEBUG
            Serial.println("JFAT PERFORMING FULL 256 BYTE WRITE TO " + String(_cur_address));
        #endif
        write_storage(_cur_address, buffer, write_size); 
        _cur_address += write_size; 
        buffer += write_size; 
        len -= write_size; 
    }
    // add the rest to the storage buffer 
    if(len > 0){
        memcpy(&_storage_buffer[_storage_buffer_index], buffer, len); 
        #ifdef JFAT_DEBUG
            Serial.println("JFAT COPYING " + String(len) + " BYTES INTO BUFFER");
            Serial.println("FIRST BYTE: " + String(buffer[0])); 
        #endif
        _storage_buffer_index += len; 

    }
    return 0; 
}

int JFAT::close(){
    // close out the current file 
    if(_cur_state == READ){
        // overwrite the state and cur address 
        _cur_state = NONE;
        _cur_address = 0; 
        return 0; 
    }
    if(_cur_state == NONE) return -1; 

    // first write the remaining buffer to memory 
    write_storage(_cur_address, _storage_buffer, _storage_buffer_index); 
    _cur_address += _storage_buffer_index; 
    // grab a copy of the current file list 
    FileList list; 
    read_fat(&list); 
    // set the ending file 
    // Serial.println("CLOSE END ADDRESS: " + String(_cur_address)); 
    unsigned int index = list.num_files-1; 
    //Serial.println("JFAT INDEX " + String(index));
    if(_cur_address == list.start_pages[index] * 256){
        // nothing was written to the file 
        // close out the file with no information 
        list.num_files --; 
        #ifdef JFAT_DEBUG
            Serial.println("JFAT CLOSE NO DATA WRITTEN TO FILE, OVERWRITTING FILE"); 
        #endif 
    }
    else{
        list.page_length[index] = (_cur_address - 1 - list.start_pages[index] * 256) >> 8;
        if(_cur_address%256 == 0){
            list.page_offset[index] = 255;
        }
        else list.page_offset[index] = (_cur_address%256)-1; // last address you could write to 
    }
    // write the data 
    write_fat(&list);  
    _cur_state = NONE; 
}

int JFAT::open_file_write(){
    // open a file to write 
    // make sure we are not in a write mode already 
    if(wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT) < 0) return -1;  
    if(_cur_state == WRITE) return -1; 
    // first get an accurate fat table 
    FileList list; 
    read_fat(&list); 
    // find the next available address for writing 
    // find the next 4 kb sector available after the last write 
    uint32_t last_address_used; 
    if(list.num_files == 0) last_address_used = 0; 
    else last_address_used = list.start_pages[list.num_files-1] * 256 + list.page_length[list.num_files-1] * 256 + list.page_offset[list.num_files-1];
    // find the next 4 kb address after this 
    uint32_t new_address = ((last_address_used >> 12) + 1) << 12; 
    // start an erase operation 
    _storage.erase_sector(new_address); 
    _erased_address = new_address + 4096; 
    // add an entry 
    list.num_files += 1; 
    list.start_pages[list.num_files - 1] = new_address >> 8; // convert to a page 
    list.page_length[list.num_files - 1] = 0; 
    list.page_offset[list.num_files - 1] = 0; 
    _storage_buffer_index = 0; // reset this, edge case 
    // set current information 
    _cur_state = WRITE; 
    _cur_address = new_address; 
    //_erased_address = new_address + 1 << 12; 
    #ifdef JFAT_DEBUG
        // print out the info 
        Serial.println("JFAT CREATING NEW FILE AT " + String(new_address)); 
    #endif 
    // write the information 
    return write_fat(&list); 
}

int JFAT::erase_space(unsigned int len){
    // check if an erase is neccessary 
    // keep one sector ahead 
    while(len + _cur_address > _erased_address - 4096){
        // erase another sector 
        wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT); 
        _storage.erase_sector(_erased_address); 
        _erased_address += 4096; 
    }
    return 0; 
}


int JFAT::write_fat(JFAT::FileList *list){
    // create the buffer to write the table 
    // use the buffer-based write system available 
    // schedule a sector erase 
    if(wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT) < 0) return -1; 
    _storage.erase_sector(0); 
    // start writing the buffer 
    unsigned int size = list->num_files* 5 + 5;
    uint8_t buffer[size]; 
    // set the start 
    buffer[0] = (char)'J'; 
    buffer[1] = (char)'F'; 
    buffer[2] = (char)'A'; 
    buffer[3] = (char)'T'; 
    // start adding the file information 
    buffer[4] = list->num_files; 
    unsigned int pntr = 5; 
    for(int i = 0; i < list->num_files; i ++){
        buffer[pntr] = (uint8_t)(list->start_pages[i] >> 8); 
        buffer[pntr + 1] = (uint8_t)list->start_pages[i];
        buffer[pntr + 2] = (uint8_t)(list->page_length[i] >> 8); 
        buffer[pntr + 3] = (uint8_t)list->page_length[i];
        buffer[pntr + 4] = list->page_offset[i]; 
        pntr += 5; 
    }
    // wait for the write to complete 
    int status = wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT); 
    // write to the first page 
    // break up into appropriate buffer sizes 
    unsigned int index = 0; 
    while(size - index > 0){
        // check if a full 256 byte write is available 
        if(size - index >= 256){
            // write the next 256 bytes
            write_storage(index, &buffer[index], 256); 
            index += 256; 
        }
        else{
            write_storage(index, &buffer[index], size - index); 
            index = size; 
        }
    }
    return 0; 
}

int JFAT::read_fat(FileList *list){
    // try to read the first 5 bytes and check for a 'JFAT' listing 
    if(wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT) < 0) return -1; 
    uint8_t header[5]; 
    int status = read_storage(0, header, 5); 
    if(header[0] != 'J' || header[1] != 'F' || header[2] != 'A' || header[3] != 'T'){
        #ifdef JFAT_DEBUG
            Serial.println("JFAT NO FAT FOUND"); 
        #endif 
        return -1; 
    }
    // otherwise, parse 
    list->num_files = header[4]; 
    uint8_t buffer[5*header[4]]; 
    read_storage(5, buffer, 5*header[4]); 
    #ifdef JFAT_DEBUG
        Serial.println("JFAT FAT TABLE FOUND: " + String(header[4]) + " FILES: ");  
    #endif 
    // parse through 
    unsigned int pntr = 0; 
    for(int i = 0; i < header[4]; i ++){
        list->start_pages[i] = buffer[pntr] << 8 | buffer[pntr + 1]; 
        list->page_length[i] = buffer[pntr + 2] << 8 | buffer[pntr + 3]; 
        list->page_offset[i] = buffer[pntr + 4];
        pntr += 5; 
        #ifdef JFAT_DEBUG
            Serial.println("\t" + String(list->start_pages[i]) + " " + String(list->page_length[i]) + " " + String(list->page_offset[i])); 
        #endif 
    }
    return 0; 

}

int JFAT::read_storage(uint32_t address, uint8_t *buffer, unsigned int len){
    // do a pass through for now, reading should not be prevented 
    wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT);
    int status = _storage.read(address, buffer, len);
    #ifdef JFAT_DEBUG 
        Serial.println("JFAT READING " + String(len) + " BYTES FROM " + String(address)); 
        for(int i = 0; i < len; i ++){
            Serial.print("\t"); 
            Serial.print(buffer[i]);
            if((i + 1) % 24 == 0) Serial.println(); 
        }
        Serial.println(); 
    #endif 
    return status; 
}

int JFAT::write_storage(uint32_t address, uint8_t *buffer, unsigned int len){
    if(wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT) < 0) return -1; 
    // check that writing is okay 
    #ifdef JFAT_DEBUG 
        // print out the data 
        Serial.println("JFAT WRITING " + String(len) + " BYTES TO: " + String(address)); 
        for(int i = 0; i < len; i ++){
            Serial.print("\t"); 
            Serial.print(buffer[i]);
            if((i + 1) % 24 == 0) Serial.println(); 
        }
        Serial.println(); 
    #endif 
    #ifdef JFAT_WRITE_FLASH 
        // handle amounts greater than 256 bytes appropriately 
        if(len >= 256){
            while(len > 0){
                wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT);
                _storage.write(address, buffer, 256);
                address += 256; 
                buffer += 256;  
                len -= 256; 
            }
        }
        else{
            wait_until_free(STANDARD_SECTOR_ERASE_TIMEOUT);
            _storage.write(address, buffer, len);
        }
    #endif 
    return 0; 
}   


int JFAT::wait_until_free(unsigned long timeout){
    // wait until free 
    unsigned long start = millis(); 
    while((millis() - start < timeout) && (_storage.is_busy())){
        delay(1); 
    }
    if(_storage.is_busy()){
        #ifdef JFAT_DEBUG
            Serial.println("JFAT WAIT TIMEOUT ERROR"); 
        #endif 
        return -1; 
    }
    #ifdef JFAT_DEBUG 
        Serial.println("JFAT WAIT TOOK " + String(millis() - start) + " ms"); 
    #endif
    return 0; 
}