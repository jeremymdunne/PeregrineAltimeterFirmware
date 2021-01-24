/**
 * @file CommunicationProtocol.h 
 * \brief Contains all the standards needed for the communication protocol 
 * 
 * This file contains all the definitions needed for the communication protocol 
 * 
 * \author Jeremy Dunne 
 */ 

#ifndef _COMMUNICATION_PROTOCOL_H_
#define _COMMUNICATION_PROTOCOL_H_

/// Standard Communication Flags 
#define COMMUNICATION_CONNECTION_REQUEST_FLAG                   11          //< Communication request ping made by connected software to the altimeter 
#define COMMUNICATION_CONNECTION_SUCCESS_FLAG                   12          //< Communication request success response made by the altimeter 
#define COMMUNICATION_SEND_OK                                   13          //< Communication message acknowledgement 
#define COMMUNICATION_RESEND_REQUEST                            14          //< Communication message request resend 
#define COMMUNICATION_RESEND_UNABLE                             15          //< Communication message resend request unable to be completed 


/// Data Specific Flags 
#define COMMUNICATION_VERBOSE_MESSAGE_FLAG                      30          //< Communication Verbose Message (interpret as string)
#define COMMUNICATION_ERROR_MESSAGE_FLAG                        31          //< Communication Error Message (interpret as string)


// data request flags 
#define COMMUNICATION_REQUEST_FILE_LIST_FLAG                    40          //< Request for the file list 
// request response as follows 
#define COMMUNICATION_FILE_LIST_FLAG                            41          //< Response for the communication file list request 
/*
format as follows: 
    NUM_FILES (1 byte): Number of files available 
    loop for each available file: 
        FILE_PAGE_LENGTH (2 bytes): Number of pages (256 bytes) available  

*/ 
 
// file request flags 
#define COMMUNICATION_REQUEST_FILE_CONTENTS                     42          //< Request for the contents of a file 
/*
format as follows: 
    FILE_INDEX (1 byte): file being requested

This request will place the device in a file recall state 
The entire contents of a flight will be output to the host line by line using the below response 
*/ 
#define COMMUNICATION_FILE_REQUEST_ENTRY_CONTENT                43          //< Request response including one entry from storage 
/* 
format as follows: 
    DATA_FLAG (1 byte): flag of the data (entry)
    TIME_STAMP (2 bytes): time stamp of the data (a flag will represent a time loop)
    LENGTH (1 byte): length of the data in the message (this is redundant)
    DATA (n bytes): data
*/ 
#define COMMUNCIATION_FILE_REQUEST_COMPLETE                     44          //< File recall complete, ASCII ','  

#define COMMUNICATION_FILE_DELETE_ALL_FILES                     45          //< Delete all files on the board 
#define COMMUNICATION_FILE_DELETE_LAST_FILE                     46          //< Delete the last file on the device 


#define COMMUNICATION_START_LIVE_SENSOR_FEED                    50          //< Start reporting a live sensor feed at a predetermined rate  
#define COMMUNICATION_LIVE_SENSOR_FLAG                          51          //< Flag for a live sensor feed 
/* 
format as follows: 
    Same as the GENERAL_FLIGHT_STORAGE data flag 
*/ 






#endif 
