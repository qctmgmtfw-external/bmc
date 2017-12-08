/*****************************************************************
******************************************************************
***                                                            ***
***        (C)Copyright 2010, American Megatrends Inc.         ***
***                                                            ***
***                    All Rights Reserved                     ***
***                                                            ***
***       5555 Oakbrook Parkway, Norcross, GA 30093, USA       ***
***                                                            ***
***                     Phone 770.246.8600                     ***
***                                                            ***
******************************************************************
******************************************************************
******************************************************************
*
* send_arp.h

This header file corresponds to send_arp.c
******************************************************************/

/**
 * @fn SendGratuitousARPPacket
 * @brief Function to send a Gratuitous ARP packet in the network
 * @param[in] pEthDevice - Ethernet Device name.
 * @param[in] pSrcIP - source IP in form of u_char [4].
 * @param[in] pSrcHwAddr - source Harware address (MAC) in form of u_char [6].
 * @retval return 0 on Success,
 *              -1 on Failure.
 */
extern int SendGratuitousARPPacket(char *pEthDevice, unsigned char *pSrcIP, unsigned char *pSrcHwAddr);

