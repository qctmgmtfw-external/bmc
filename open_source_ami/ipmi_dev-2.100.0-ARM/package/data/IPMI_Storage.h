/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2005-2006, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        6145-F, Northbelt Parkway, Norcross,                **
 **                                                            **
 **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************
 ****************************************************************
 *
 * Storage.h
 * Storage Command numbers
 *
 *****************************************************************/
#ifndef IPMI_STORAGE_H
#define IPMI_STORAGE_H

/*** FRU Information commands ***/
#define CMD_FRU_INVENTORY_AREA_INFO                 0x10
#define CMD_READ_FRU_DATA                           0x11
#define CMD_WRITE_FRU_DATA                          0x12

/*** SDR Repository commands ***/
#define CMD_GET_SDR_REPOSITORY_INFO                 0x20
#define CMD_GET_SDR_REPOSITORY_ALLOCATION_INFO      0x21
#define CMD_RESERVE_SDR_REPOSITORY                  0x22
#define CMD_GET_SDR                                 0x23
#define CMD_ADD_SDR                                 0x24
#define CMD_PARTIAL_ADD_SDR                         0x25
#define CMD_DELETE_SDR                              0x26
#define CMD_CLEAR_SDR_REPOSITORY                    0x27
#define CMD_GET_SDR_REPOSITORY_TIME                 0x28
#define CMD_SET_SDR_REPOSITORY_TIME                 0x29
#define CMD_ENTER_SDR_REPOSITORY_UPDATE_MODE        0x2A
#define CMD_EXIT_SDR_REPOSITORY_UPDATE_MODE         0x2B
#define CMD_RUN_INITIALIZATION_AGENT                0x2C

/*** SEL commands ***/
#define CMD_GET_SEL_INFO                            0x40
#define CMD_GET_SEL_ALLOCATION_INFO                 0x41
#define CMD_RESERVE_SEL                             0x42
#define CMD_GET_SEL_ENTRY                           0x43
#define CMD_ADD_SEL_ENTRY                           0x44
#define CMD_PARTIAL_ADD_SEL_ENTRY                   0x45
#define CMD_DELETE_SEL_ENTRY                        0x46
#define CMD_CLEAR_SEL                               0x47
#define CMD_GET_SEL_TIME                            0x48
#define CMD_SET_SEL_TIME                            0x49
#define CMD_GET_AUXILIARY_LOG_STATUS                0x5A
#define CMD_SET_AUXILIARY_LOG_STATUS                0x5B
#define CMD_GET_SEL_TIME_UTC_OFFSET                 0x5C
#define CMD_SET_SEL_TIME_UTC_OFFSET                 0x5D

#endif  /* IPMI_STORAGE_H */
