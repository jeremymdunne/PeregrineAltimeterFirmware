#ifndef _W25Q64FV_DEFS_H_
#define _W25Q64FV_DEFS_H_

#define W25Q64FV_WRITE_ENABLE_REG                   0x06 
#define W25Q64FV_WRITE_DISABLE_REG                  0x04 
#define W25Q64FV_READ_STATUS_REG_1_REG              0x05
#define W25Q64FV_READ_STATUS_REG_2_REG              0x35
#define W25Q64FV_WRITE_STATUS_REG                   0x01
#define W25Q64FV_PAGE_PROGRAM_REG                   0x02
#define W25Q64FV_SECTOR_ERASE_REG                   0x20 // 4 kb 
#define W25Q64FV_BLOCK_ERASE_32_REG                 0x52
#define W25Q64FV_BLOCK_ERASE_64_REG                 0xD8
#define W25Q64FV_CHIP_ERASE_REG                     0x60
#define W25Q64FV_READ_DATA_REG                      0x03
#define W25Q64FV_JEDEC_ID_REG                       0x9F
#define W25Q64FV_READ_SFDP_REG                      0x5A
#define W25Q64FV_ERASE_SECURITY_REG                 0x44
#define W25Q64FV_PROGRAM_SECURITY_REG               0x42
#define W25Q64FV_READ_SECURITY_REG                  0x48
#define W25Q64FV_ENABLE_RESET_REG                   0x66
#define W25Q64FV_RESET_REG                          0x99 


#define W25Q64FV_SPI_FREQUENCY                      104000000

#endif 