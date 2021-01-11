/**
 * Library for emulating a file system on a Flash Storage Device 
 * 
 * A simple file allocation table is created on the first sector (4kb) of the device 
 * 
 * File entries are as follows: 
 * 
 *  JFAT spelled out as ASCII characters 
 *      This is used as an identifier to see if the system already has a JFAT table 
 *  File entires: 
 *      Start Page (2 Bytes)
 *      Number of Pages (2 Bytes) (inclusive)
 *      End Page Offset (1 Byte) (inclusive)
 *      
 * 
 * To accompany the Flash device, a write buffer is utilized 
 * Once the buffer is filled, it is then written to the flash storage 
 * This buffer must be a multiple of 256 bytes (a page) 
 * 
 * Auto sector erases are used in an effort to decrease down time 
 *  In the future, this may be replaced with a secondary buffer to allow the downtime for the storage to erase 
 *  a sector. 
 * 
 * 
 * Author: Jeremy Dunne
 * Date: 7/5/2020
 */ 

#ifndef _JFAT_H_
#define _JFAT_H_

#define JFAT_WRITE_FLASH 
//#define JFAT_DEBUG 


#include <Arduino.h> 
#include <W25Q64FV.h> 


#define JFAT_BUFFER_SIZE 256 // single page for now 

#define STANDARD_SECTOR_ERASE_TIMEOUT       100


class JFAT{
public: 
    struct FileList{
        uint16_t start_pages[32]; 
        uint16_t page_length[32]; 
        uint8_t page_offset[32]; 
        uint8_t num_files = 0; 
    }; 

    /**
     * initialize the file system 
     * check if the device already contains a JFAT table, create one if not 
     * @param cs_pin cs pin for the storage device 
     * @return error or success code 
     */ 
    int begin(int cs_pin); 

    /**
     * open a file to write
     * currently only new files are allowed 
     * @return error or success code 
     */ 
    int open_file_write(); 

    /**
     * open a file to read 
     * @param fd file index 
     * @return error or success code 
     */ 
    int open_file_read(unsigned int fd); 

    /**
     * close the current file open 
     * @return error or success code 
     */
    int close(); 

    /**
     * write contents 
     * this uses a buffer to write to the flash in appropiate ammounts 
     * @param buffer buffer of data to write 
     * @param len length of data to write 
     * @return error or success code
     */ 
    int write(uint8_t *buffer, unsigned int len); 

    /**
     * read contents 
     * @param buffer buffer to read into 
     * @param len length of data to read 
     * @return number of bytes written -- this can be less than the desired amount if eof reached 
     */ 
    int read(uint8_t *buffer, unsigned int len); 

    /**
     * read the file allocation table 
     * @param files struct to read the data into 
     * @return error or success code
     */ 
    int read_fat(FileList *list); 

    /**
     * get the remaining file length 
     * @return number of bytes remaining to be read 
     */ 
    uint32_t peek(); 

    /**
     * 'delete' the last file 
     * @return error or success code
     */ 
    int delete_last_file(); 

    /**
     * 'delete' all files 
     * @return error or success code 
     */ 
    int delete_all_files(); 




private: 
    W25Q64FV _storage; 
    uint8_t _storage_buffer[JFAT_BUFFER_SIZE]; 
    unsigned int _storage_buffer_index = 0; // 'points' to the next available spot to write 

    enum FILE_STATE{
        READ, WRITE, NONE
    };

    FILE_STATE _cur_state = NONE; 
    // information needed for reading 
    uint32_t _file_last_address; // last available address inclusive 
    uint32_t _cur_address; // address to read from 
    // information needed for writing 
    uint32_t _erased_address; // last page erased 

    /**
     * check if any more space needs to be erased 
     * performs blocking erase cycles 
     * @param len desired length of data to write 
     * @return error or success code 
     */ 
    int erase_space(unsigned int len); 

    /**
     * write the file allocation table 
     * @param files struct to write 
     * @return error or success code 
     */ 
    int write_fat(FileList *list); 


    /**
     * write to the storage 
     * cannot write more than 256 bytes at a time 
     * @param address address to write to (divisible by 256)
     * @param buffer buffer to write 
     * @param len length of data to write 
     * @return error or success code 
     */ 
    int write_storage(uint32_t address, uint8_t *buffer, unsigned int len); 

    /**
     * read from the storage 
     * cannot read more than 256 bytes at a time 
     * @param address address to read from (divisible by 256)
     * @param buffer buffer to read into 
     * @param len length of data to read 
     * @return error or success code
     */ 
    int read_storage(uint32_t address, uint8_t *buffer, unsigned int len); 


    /**
     * wait until the storage frees up 
     * Use a timeout to prevent a hold 
     * @param timeout maximum amount of time (in milliseconds) 
     * @return error or success code
     */ 
    int wait_until_free(unsigned long timeout); 

}; 


#endif 