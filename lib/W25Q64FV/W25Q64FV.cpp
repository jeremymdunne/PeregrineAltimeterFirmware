#include <W25Q64FV.h> 

int W25Q64FV::begin(int cs_pin){
    _cs = cs_pin; 
    pinMode(_cs, OUTPUT); 
    digitalWrite(_cs, HIGH); 
    // set the settings 
    SPI.begin(); 
    _spi_settings = SPISettings(W25Q64FV_SPI_FREQUENCY,MSBFIRST,SPI_MODE_0); 
    // check the id 
    int status = check_id(); 
    if(status < 0) return status; 
    return 0; 
}

int W25Q64FV::read(uint32_t address, uint8_t *buffer, unsigned int length){
    // check if busy 
    if(is_busy()) return -1; 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    // send a page program 
    SPI.transfer(W25Q64FV_READ_DATA_REG);
    // send over the address 
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer((address) & 0xFF); // last 8 bits must be 0 
    // send the data
    while(length > 0){
        *buffer = SPI.transfer(0); 
        length --; 
        *buffer ++; 
    }
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::write(uint32_t address, uint8_t *buffer, unsigned int length){
    // check if busy 
    if(is_busy()) return -1; 
    write_enable(); 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    // send a page program 
    SPI.transfer(W25Q64FV_PAGE_PROGRAM_REG);
    // send over the address 
    SPI.transfer((address >> 16) & 0xFF);
    SPI.transfer((address >> 8) & 0xFF);
    SPI.transfer(0); // last 8 bits must be 0 
    // send the data
    while(length > 0){
        SPI.transfer(*buffer); 
        length --; 
        *buffer ++; 
    }
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::erase_sector(uint32_t sector_address){
    if(is_busy()) return -1; 
    write_enable(); 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(W25Q64FV_SECTOR_ERASE_REG); 
    SPI.transfer((sector_address >> 16) & 0xFF);
    SPI.transfer((sector_address >> 8) & 0xFF);
    SPI.transfer((sector_address >> 0) & 0xFF);
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::erase_block_32(uint32_t block_address){
    if(is_busy()) return -1; 
    write_enable(); 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(W25Q64FV_BLOCK_ERASE_32_REG); 
    SPI.transfer((block_address >> 16) & 0xFF);
    SPI.transfer((block_address >> 8) & 0xFF);
    SPI.transfer((block_address >> 0) & 0xFF);
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::erase_block_64(uint32_t block_address){
    if(is_busy()) return -1; 
    write_enable(); 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(W25Q64FV_BLOCK_ERASE_64_REG); 
    SPI.transfer((block_address >> 16) & 0xFF);
    SPI.transfer((block_address >> 8) & 0xFF);
    SPI.transfer((block_address >> 0) & 0xFF);
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}


int W25Q64FV::check_id(){
    // check the id 
    uint8_t buf[3] = {0}; 
    read_dev(W25Q64FV_JEDEC_ID_REG, buf, 3); 
    if(buf[0] != 255 & buf[1] != 0 & buf[2] != 0){
        return 0; 
    }
    return -1; 
}

bool W25Q64FV::is_busy(){
    // check the BUSY reg 
    uint8_t busy; 
    // read the status reg 1 
    read_dev(W25Q64FV_READ_STATUS_REG_1_REG, &busy, 1); 
    // S0 represents the busy 
    if(busy & 0b00000001){
        return true; 
    }
    return false; 
}

int W25Q64FV::write_enable(){
    // just write the instruction 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(W25Q64FV_WRITE_ENABLE_REG); 
    SPI.endTransaction();
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::write_disable(){
    // just write the instruction 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(W25Q64FV_WRITE_DISABLE_REG); 
    SPI.endTransaction();
    digitalWrite(_cs, HIGH); 
    return 0; 
}

int W25Q64FV::write_dev(uint8_t write_reg, uint8_t *buffer, int len){
    // select 
    SPI.beginTransaction(_spi_settings); 
    digitalWrite(_cs, LOW); 
    SPI.transfer(write_reg); 
    while(len --){
        SPI.transfer(*buffer); 
        len --; 
        *buffer ++; 
    }
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}


int W25Q64FV::read_dev(uint8_t read_reg, uint8_t *buffer, int len){
    // select the device 
    SPI.beginTransaction(_spi_settings);
    digitalWrite(_cs, LOW); 
    SPI.transfer(read_reg); 
    while(len > 0){
        *buffer = SPI.transfer(0); 
        //Serial.print(*buffer, HEX); 
        //Serial.print("\t"); 
        len --; 
        *buffer ++; 
    }
    //Serial.println(); 
    SPI.endTransaction(); 
    digitalWrite(_cs, HIGH); 
    return 0; 
}

