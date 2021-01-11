/**
 * Library for interfacing with the W25Q64FV flash chip 
 * It is highly recommended that a file system is included to use this chip 
 * 
 * Author: Jeremy Dunne 
 * Date: 7/5/2020 
 */ 


#ifndef _W25Q64FV_
#define _W25Q64FV_

#include <Arduino.h> 
#include <SPI.h> 
#include <W25Q64FV_defs.h> 


class W25Q64FV{
public: 
    /**
     * initialize the device 
     * @param cs_pin cs pin for SPI 
     * @return error or success code 
     */ 
    int begin(int cs_pin); 

    /**
     * write to page 
     * @param address page address to write (i.e. last 8 bits are 0) 
     * @param buffer buffer of data to write to 
     * @param length of data to write 
     * @return number of bytes written
     */ 
    int write(uint32_t address, uint8_t *buffer, unsigned int length); 


    /**
     * read data from a page 
     * @param address page address to read from (i.e. last 8 bits are 0)
     * @param buffer buffer to read into 
     * @param len length of data to read 
     * @return number of bytes read 
     */ 
    int read(uint32_t address, uint8_t *buffer, unsigned int length);


    /**
     * perform a non-blocking sector (4kb) erase 
     * @param sector_address address, must be divisible by 4kb 
     * @return error or success code
     */ 
    int erase_sector(uint32_t sector_address); 


    /**
     * perform a non-blocking block (32 kb) erase 
     * @param block_address address, must be divisible by 32 kb 
     * @return error or success code 
     */ 
    int erase_block_32(uint32_t block_address); 


    /**
     * perform a non-blocking block (64 kb) erase 
     * @param block_address address, must be divisible by 64 kb 
     * @return error or success code 
     */ 
    int erase_block_64(uint32_t block_address); 

    /**
     * check if the device is busy 
     * @return true if busy, false if not 
     */ 
    bool is_busy(); 

    /**
     * enable writing to the device 
     * @return error or success code 
     */ 
    int write_enable(); 

    /**
     * disable writing to the device 
     * @return error or success code 
     */ 
    int write_disable(); 


private: 
    // private global variables 
    SPISettings _spi_settings; // spi settings 
    int _cs; //cs pin 

    // private functions 
    /**
     * check the device id 
     * @return 0 for correct id, -1 otherwise 
     */ 
    int check_id(); 


    /**
     * write to the device 
     * @param write_reg or command reg 
     * @param buffer buffer to write 
     * @param len length of data to write 
     * @return error or success code
     */ 
    int write_dev(uint8_t write_reg, uint8_t *buffer, int len); 

    /**
     * read from the device 
     * @param read_reg or command reg 
     * @param buffer buffer to read into 
     * @param len length of data to read 
     * @return error or success code 
     */ 
    int read_dev(uint8_t read_reg, uint8_t *buffer, int len); 

}; 



#endif 