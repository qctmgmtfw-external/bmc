/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2010-2011, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy, Building 200, Norcross,         **
 **                                                            **
 **        Georgia 30093, USA. Phone-(770)-246-8600.           **
 **                                                            **
 ****************************************************************
 ****************************************************************/
/*****************************************************************
 *
 * AMIBiosCode.h
 * 
 *
 * Author: Gokulakannan. S <gokulakannans@amiindia.co.in>
 *****************************************************************/


#ifndef AMIBIOSCODE_H_
#define AMIBIOSCODE_H_

#include "Types.h"


extern int AMIGetBiosCode (_NEAR_ INT8U* pReq, INT32U ReqLen, _NEAR_ INT8U* pRes, int BMCInst);


#endif /* AMIBIOSCODE_H_ */
