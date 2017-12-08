--- .pristine/goahead-1.62.0-src/data/web_session_ifc_ipmi.c Thu Aug 30 13:27:45 2012
+++ source/goahead-1.62.0-src/data/web_session_ifc_ipmi.c Thu Aug 30 17:35:35 2012
@@ -556,7 +556,6 @@
 #ifdef CONFIG_SPX_FEATURE_GLOBAL_WEB_AUTHORIZATION
 	int configpriv;
 #endif	
-	struct sockaddr_in   name;
 	socket_t *m_socketList;
 	char EffectiveUserName[64];
 	char EffectivePass[64];
@@ -565,6 +564,17 @@
 	int UserPriv = 0;
 	SERVICE_CONF_STRUCT webCfg;
 
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPV6
+	struct sockaddr_in6 namev6; 
+	INT8U IPAddr[IP6_ADDR_LEN]; 
+#else 
+	struct sockaddr_in namev4; 
+	INT8U IPAddr[IP_ADDR_LEN];
+#endif
+	void *name;
+	char BMCIPAddrStr[INET6_ADDRSTRLEN];
+
+
 	//the web ipmi session structure is a wrapper over a regular IPMISession used by libipmi
 	//just in case we want to add stuff specific to the web there.
 	WEBIPMI_SESSION_ENTRY_T* pWEBIPMISession;
@@ -577,6 +587,7 @@
 	WEBPAGE_DECLARE_JSON_RECORD( WEB_SESSION )
 	{
 		JSONFIELD(SESSION_COOKIE,JSONFIELD_TYPE_STR),
+		JSONFIELD(BMC_IP_ADDR,JSONFIELD_TYPE_STR),
 	};
 
 	//get the username and password
@@ -586,17 +597,47 @@
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN( WEB_SESSION );
 
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPV6
+	name = &namev6;
+	sock_addr_len = sizeof(struct sockaddr_in6); 
+#else 
+	name = &namev4;
+	sock_addr_len = sizeof(struct sockaddr_in); 
+#endif
 	m_socketList=socketList[((webs_t)wp)->sid];
-	sock_addr_len = sizeof(struct sockaddr_in);
-	retval = getsockname(m_socketList->sock,(struct sockaddr *) &name , (unsigned *)&sock_addr_len);
+	retval = getsockname(m_socketList->sock,(struct sockaddr *) name , (unsigned *)&sock_addr_len);
 	if (retval == -1)
 		printf("\n Error in Reading the socket :%x\t%s \n",m_socketList->sock , strerror( errno ));
 
-	EthIndex=GetHostEthbyIPAddr((char*)&name.sin_addr);
+//	EthIndex=GetHostEthbyIPAddr((char*)&name.sin_addr);
+
+#ifdef CONFIG_SPX_FEATURE_GLOBAL_IPV6
+	memset(&IPAddr[0],0,IP6_ADDR_LEN);
+	if(IN6_IS_ADDR_V4MAPPED(&((struct sockaddr_in6 *)name)->sin6_addr))
+	{ 
+		EthIndex = GetHostEthbyIPAddr((char*)&((struct sockaddr_in6 *)name)->sin6_addr.s6_addr[IP6_ADDR_LEN-IP_ADDR_LEN]);
+		memcpy(&IPAddr[0],(INT8U *)&((struct sockaddr_in6 *)name)->sin6_addr.s6_addr[IP6_ADDR_LEN-IP_ADDR_LEN],IP_ADDR_LEN); 
+		inet_ntop(AF_INET,IPAddr,BMCIPAddrStr,INET_ADDRSTRLEN); 
+	} 
+	else 
+	{ 
+		EthIndex = GetHostEthByIPv6Addr((char *)&((struct sockaddr_in6 *)name)->sin6_addr);
+		memcpy(&IPAddr[0],(INT8U *)&((struct sockaddr_in6 *)name)->sin6_addr.s6_addr[0],IP6_ADDR_LEN); 
+		inet_ntop(AF_INET6,IPAddr,BMCIPAddrStr,INET6_ADDRSTRLEN); 
+	} 
+#else 
+	memset(&IPAddr[0],0,IP_ADDR_LEN); 
+	EthIndex=GetHostEthbyIPAddr((char*)&((struct sockaddr_in *)name)->sin_addr);
+	memcpy(&IPAddr[0],(INT8U *)&((struct sockaddr_in *)name)->sin_addr.s_addr,IP_ADDR_LEN); 
+	inet_ntop(AF_INET,IPAddr,BMCIPAddrStr,INET_ADDRSTRLEN);
+#endif
+
 	if(EthIndex >=0)
 		pWEBIPMISession->IPMISession.EthIndex=EthIndex;
 	else
 		pWEBIPMISession->IPMISession.EthIndex=0;
+
 
 	strncpy(UserPasswd,WEBVAR_PASSWORD,63);
 	m_passwd = UserPasswd;
@@ -608,7 +649,7 @@
 		if(RADIUS_Login(WEBVAR_USERNAME,WEBVAR_PASSWORD) != 0)
 		{
 			TCRIT("Radius login failed..  \n");
-			WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_IPMI_Then_LDAP_then_Active_Directory_Radius");
+			WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_IPMI_Then_LDAP_then_Active_Directory_Radius", BMCIPAddrStr);
 			if (nopriv == CC_INSUFFIENT_PRIVILEGE)
 			{
 				WEBPAGE_WRITE_JSON_END(WEB_SESSION, CC_INSUFFIENT_PRIVILEGE);
@@ -627,7 +668,7 @@
 		if(retval != 0)
 		{
 			TCRIT("Error establishing proxy ipmi session for RADIUS user\n");
-			WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_Proxy");
+			WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_Proxy", BMCIPAddrStr);
 			if (nopriv == CC_INSUFFIENT_PRIVILEGE)
 			{
 				WEBPAGE_WRITE_JSON_END(WEB_SESSION, CC_INSUFFIENT_PRIVILEGE);
@@ -679,7 +720,7 @@
 			if(retval != 0)
 			{
 				TCRIT("Error establishing proxy ipmi session for ldap user\n");
-				WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_Proxy");
+				WEBPAGE_WRITE_JSON_RECORD( WEB_SESSION, "Failure_Login_Proxy", BMCIPAddrStr);
 				if (nopriv == CC_INSUFFIENT_PRIVILEGE)
 				{
 					WEBPAGE_WRITE_JSON_END(WEB_SESSION, CC_INSUFFIENT_PRIVILEGE);
@@ -727,14 +768,14 @@
 	if(retval != 0)
 	{
 		TCRIT("Error in WebSession_Create\n");
-		WEBPAGE_WRITE_JSON_RECORD(WEB_SESSION, "Failure_Session_Creation");
+		WEBPAGE_WRITE_JSON_RECORD(WEB_SESSION, "Failure_Session_Creation", BMCIPAddrStr);
 		WEBPAGE_WRITE_JSON_END(WEB_SESSION, retval );
 		free(pWEBIPMISession);
 		return;
 	}
 	else
 	{
-		WEBPAGE_WRITE_JSON_RECORD(WEB_SESSION, SessionCookie);
+		WEBPAGE_WRITE_JSON_RECORD(WEB_SESSION, SessionCookie, BMCIPAddrStr);
 		WEBPAGE_WRITE_JSON_END(WEB_SESSION, retval );
 	}
 	WEBPAGE_WRITE_END();
