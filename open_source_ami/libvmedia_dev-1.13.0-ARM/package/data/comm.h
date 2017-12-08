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


#ifndef COMM_H
#define COMM_H

/* For UNIX (non-Windows) Platforms */
#ifndef WIN32
typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
#define closesocket close
#define INVALID_SOCKET (-1)
#endif

#define BOTH_INTERFACES "both"

/* Function Prototypes of COMM Module */
SOCKET	InitializeServer(u_short PortNo, char *InterfaceName);
SOCKET	WaitForClientConnection(SOCKET serv_sock);
void	ShutDownServer(SOCKET sock);

#endif /* COMM_H */