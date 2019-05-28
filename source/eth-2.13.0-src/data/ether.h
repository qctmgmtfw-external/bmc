/****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross,             **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
****************************************************************/

#ifndef ETHER_H
#define ETHER_H

#define ETH_DEV_MTU   1500

void EthRxHandler(uint8 DevNo,uint8 ifnum, uint8 epnum);
void EthTxHandler(uint8 DevNo,uint8 ifnum, uint8 epnum);
int  EthReqHandler(uint8 DevNo,uint8 ifnum, DEVICE_REQUEST *DevRequest);
void EthRemHandler(uint8 DevNo,uint8 ifnum);

int  EthCreateDevice(void);
void EthRemoveDevice(void);

void RndisSetVendor(uint32 VID, const char *Desc);
void SetRndisPendingResponse(void);
void ClearRndisPendingResponse (void);
void EthSetIntf(uint8 Control, uint8 Data);
uint8 *EthGetHostMac(void);
uint8 *EthGetDevMac(void);
void  EthSetHostMac(uint8 *Mac);
void  EthSetDevMac(uint8 *Mac);

#endif
