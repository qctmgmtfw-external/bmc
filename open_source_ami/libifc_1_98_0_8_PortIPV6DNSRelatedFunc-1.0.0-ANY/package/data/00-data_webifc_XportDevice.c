--- .pristine/libifc-1.98.0-src/data/webifc_XportDevice.c Fri Feb  3 16:19:59 2012
+++ source/libifc-1.98.0-src/data/webifc_XportDevice.c Mon Feb 13 19:16:54 2012
@@ -876,171 +876,214 @@
 {
 	int retval = RPC_HAPI_SUCCESS;
 
-	DNS_CONFIG v4dnscfg;
-	char domainsetting[MAX_IFACE_NAME] = "Manual";
-	char interfaceName[MAX_IFACE_NAME];
-
-	INT8U v4dnslanchannel = 0;
-	unsigned char strV4DNSServer1[INET_ADDRSTRLEN];
-	unsigned char strV4DNSServer2[INET_ADDRSTRLEN];
-
-	V6DNS_CONFIG v6dnscfg;
-	INT8U v6dnslanchannel = 0;
-	unsigned char strV6DNSServer1[INET6_ADDRSTRLEN];
-	unsigned char strV6DNSServer2[INET6_ADDRSTRLEN];
+	int i;
+	HOSTNAMECONF hostCfg;
+	DomainSetting domainCfg;
+	char strDomainCfg[MAX_IFACE_NAME] = "Manual";
+	char strDomainEth[MAX_IFACE_NAME];
+	unsigned char domainName[MAX_DOMAIN_NAME_STRING_SIZE];
+
+	INT8U RegBMC_FQDN[MAX_CHANNEL];
+
+	DNSSetting dnsCfg;
+	INT8U DNSLanChannel = 0;
+	INT8U Addrlen;
+	INT8U DNSIPAddr[IP6_ADDR_LEN];
+	char strDNSServer[3][INET6_ADDRSTRLEN];
 
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(GETDNSCFG);
 
 	WEBPAGE_DECLARE_JSON_RECORD(GETDNSCFG)
 	{
-		JSONFIELD(hostSetting, JSONFIELD_TYPE_INT),
-		JSONFIELD(hostName, JSONFIELD_TYPE_STR),
-		JSONFIELD(registerBMC0, JSONFIELD_TYPE_INT),
-		JSONFIELD(registerBMC1, JSONFIELD_TYPE_INT),
-		JSONFIELD(registerBMC2, JSONFIELD_TYPE_INT),
-		JSONFIELD(registerDHCP0, JSONFIELD_TYPE_INT),
-		JSONFIELD(registerDHCP1, JSONFIELD_TYPE_INT),
-		JSONFIELD(registerDHCP2, JSONFIELD_TYPE_INT),
-		JSONFIELD(domainSetting, JSONFIELD_TYPE_STR),
-		JSONFIELD(domainName, JSONFIELD_TYPE_STR),
-		JSONFIELD(v4DNSLANChannel, JSONFIELD_TYPE_INT),
-		JSONFIELD(v4DNS1, JSONFIELD_TYPE_STR),
-		JSONFIELD(v4DNS2, JSONFIELD_TYPE_STR),
-		JSONFIELD(v6DNSLANChannel, JSONFIELD_TYPE_INT),
-		JSONFIELD(v6DNS1, JSONFIELD_TYPE_STR),
-		JSONFIELD(v6DNS2, JSONFIELD_TYPE_STR)
+		JSONFIELD(HOST_CFG, JSONFIELD_TYPE_INT),
+		JSONFIELD(HOST_NAME, JSONFIELD_TYPE_STR),
+		JSONFIELD(REG_BMC0, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(REG_BMC1, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(REG_BMC2, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(REG_DHCP0, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(REG_DHCP1, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(REG_DHCP2, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(DOMAIN_CFG, JSONFIELD_TYPE_STR),
+		JSONFIELD(DOMAIN_NAME, JSONFIELD_TYPE_STR),
+		JSONFIELD(DNS_CFG, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(DNS_PRIORITY, JSONFIELD_TYPE_INT8U),
+		JSONFIELD(DNS_IP1, JSONFIELD_TYPE_STR),
+		JSONFIELD(DNS_IP2, JSONFIELD_TYPE_STR),
+		JSONFIELD(DNS_IP3, JSONFIELD_TYPE_STR)
 	};
 
-	memset (&v4dnscfg, 0, sizeof(DNS_CONFIG));
-	memset (&v6dnscfg, 0, sizeof(V6DNS_CONFIG));
-
-	/*This block will get all DNS configuration such as Host setting, 
-	Host name, Domain Name, Domain setting, Register BMC, DNS DHCP Enable 
-	or not, DNS Eth Index and v4 DNS Servers. Also it validates and 
-	converts IPv4 Address format for v4 DNS Server1 & v4 DNS Server2 
-	using inet_ntop method*/
-	retval = LIBIPMI_HL_AMIGetAllDNSConf(&((wc_get_ipmi_session(wp))->IPMISession),
-					(INT8U *)&v4dnscfg, DEFAULT_IPMITIMEOUT);
+	/* Host Configuration */
+	memset (&hostCfg, 0, sizeof(HOSTNAMECONF));
+	retval = LIBIPMI_HL_AMIGetDNSHostSetting(
+		&((wc_get_ipmi_session(wp))->IPMISession),
+		(INT8U *)&hostCfg, DEFAULT_IPMITIMEOUT);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in Getting Hostname configuration::%x\n", retval);
+		goto error_out;
+	}
+	TDBG ("Success getting Hostname configuration\n");
+
+	/* Register BMC Configuration */
+	retval = LIBIPMI_HL_AMIGetDNSRegister(
+		&((wc_get_ipmi_session(wp))->IPMISession),
+		RegBMC_FQDN, DEFAULT_IPMITIMEOUT);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in getting Register BMC configuration::%x\n", retval);
+		goto error_out;
+	}
+	TDBG ("Success getting Register BMC configuration\n");
+
+	/* Domain Name Configuration */
+	memset (&domainCfg, 0, sizeof(DomainSetting));
+	retval = LIBIPMI_HL_AMIGetDNSDomainSetting(
+		&((wc_get_ipmi_session(wp))->IPMISession),
+		(INT8U *)&domainCfg, DEFAULT_IPMITIMEOUT);
 	if (retval != RPC_HAPI_SUCCESS)
 	{
-		TCRIT("Error in getting Domain and v4 DNS configuration::%x\n",retval);
-		goto error_out;
-	}
-	TDBG ("Success getting v4 DNS configuration\n");
-
-	if (v4dnscfg.DNSDomainNameDhcpEnable != 0)
-	{
-		memset (interfaceName, 0, MAX_IFACE_NAME);
-		retval = LIBIPMI_HL_GetLANIndex(&((wc_get_ipmi_session(wp))->IPMISession),
-					v4dnscfg.DNSDomainNameEthIndex, (INT8U *)interfaceName, DEFAULT_IPMITIMEOUT);
+		TCRIT("Error in getting Domain configuration::%x\n",retval);
+		goto error_out;
+	}
+	TDBG ("Success getting Domain configuration\n");
+
+	if (domainCfg.DomainDHCP != 0)
+	{
+		memset (strDomainEth, 0, MAX_IFACE_NAME);
+		retval = LIBIPMI_HL_GetLANIndex(
+			&((wc_get_ipmi_session(wp))->IPMISession), domainCfg.DomainIndex,
+			(INT8U *)strDomainEth, DEFAULT_IPMITIMEOUT);
 		if(retval != RPC_HAPI_SUCCESS)
 		{
 			TCRIT("Error getting in Interface name for Index(%d)::%x\n",
-						v4dnscfg.DNSDomainNameEthIndex, retval);
+				domainCfg.DomainIndex, retval);
 			goto error_out;
 		}
 		TDBG("Success getting Interface name::%s for Eth Index::%d\n",
-					interfaceName, v4dnscfg.DNSDomainNameEthIndex);
+			strDomainEth, domainCfg.DomainIndex);
 	
-		if (v4dnscfg.v4v6 == DOMAIN_V4){
-			memset (domainsetting, 0, MAX_IFACE_NAME);
-			sprintf (domainsetting, "%s_%s", interfaceName, "v4");
-		}else if (v4dnscfg.v4v6 == DOMAIN_V6){
-			memset (domainsetting, 0, MAX_IFACE_NAME);
-			sprintf (domainsetting, "%s_%s", interfaceName, "v6");
+		if (domainCfg.Domainpriority == DOMAIN_V4){
+			memset (strDomainCfg, 0, MAX_IFACE_NAME);
+			sprintf (strDomainCfg, "%s_%s", strDomainEth, "v4");
+		}else if (domainCfg.Domainpriority == DOMAIN_V6){
+			memset (strDomainCfg, 0, MAX_IFACE_NAME);
+			sprintf (strDomainCfg, "%s_%s", strDomainEth, "v6");
 		}else{
 			TCRIT ("Unknown Domain V4_V6 data, so considering Manual configuration");
 		}
 	}
+
+	for(i=0; i<4; i++)
+	{
+		retval = LIBIPMI_HL_AMIGetDNSDomainName(
+			&((wc_get_ipmi_session(wp))->IPMISession), (i + 1),
+			&domainName[i * MAX_DOMAIN_BLOCK_SIZE], DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in getting DNS Domain for the block %d\n",i);
+			goto error_out;
+		}
+	}
 	TDBG("Success getting domain name configuration, Domain Name(%d)::%s, DomainSetting::%s\n",
-				v4dnscfg.DNSDomainNameLen,v4dnscfg.DNSDomainName,domainsetting);
-
-	if (v4dnscfg.DNSDhcpEnable != 0)
+		domainCfg.DomainLen, domainName, strDomainCfg);
+
+	/* Domain Name Server Configuration */
+	memset (&dnsCfg, 0, sizeof(DNSSetting));
+	retval = LIBIPMI_HL_AMIGetDNSSetting(
+		&((wc_get_ipmi_session(wp))->IPMISession),
+		(INT8U*)&dnsCfg, DEFAULT_IPMITIMEOUT);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in getting DNS configuration::%dx\n", retval);
+		goto error_out;
+	}
+
+	if (dnsCfg.DNSDHCP != 0)
 	{
 		retval = LIBIPMI_HL_GetChannelNum(&((wc_get_ipmi_session(wp))->IPMISession),
-					v4dnscfg.DNSServerEthIndex, &v4dnslanchannel, DEFAULT_IPMITIMEOUT);
+			dnsCfg.DNSIndex, &DNSLanChannel, DEFAULT_IPMITIMEOUT);
 		if(retval != RPC_HAPI_SUCCESS)
 		{
 			TCRIT("Error in getting Channel number for Index(%d)::%x\n",
-						v4dnscfg.DNSServerEthIndex, retval);
+				dnsCfg.DNSIndex, retval);
 			goto error_out;
 		}
 		TDBG ("Success getting Eth Index::%d, Channel Number::%d\n",
-					v4dnscfg.DNSServerEthIndex, v4dnslanchannel);
-	}
-
-	if(inet_ntop(AF_INET, v4dnscfg.DNSServer1, (char *)strV4DNSServer1, INET_ADDRSTRLEN) == NULL)
-	{
-		TCRIT("It is not a valid v4 DNS Server1\n");
-		retval = INVALID_V4IP;
-		goto error_out;
-	}
-	if(inet_ntop(AF_INET, v4dnscfg.DNSServer2, (char *)strV4DNSServer2, INET_ADDRSTRLEN) == NULL)
-	{
-		TCRIT("It is not a valid v4 DNS Server2\n");
-		retval = INVALID_V4IP;
-		goto error_out;
-	}
-	TDBG ("Success converting V4 DNS Server1::%s, DNS Server2::%s\n",
-				strV4DNSServer1, strV4DNSServer2);
-
-	/*This block will get the v6 DNS configuration such as DHCP Enable or not, 
-	Eth Index and v6 DNS Servers. Also it validates and converts IPv6 Address 
-	format for DNS Server1 & DNS Server2 using inet_ntop method*/
-	retval = LIBIPMI_HL_AMIGetV6DNSConf(&((wc_get_ipmi_session(wp))->IPMISession),
-					(INT8U *)&v6dnscfg, DEFAULT_IPMITIMEOUT);
-	if (retval != RPC_HAPI_SUCCESS)
-	{
-		TCRIT("Error in getting v6 DNS configuration::%x\n",retval);
-		goto error_out;
-	}
-	TDBG ("\nSuccess getting v6 DNS configuration\n");
-	if (v6dnscfg.v6DNSDhcpEnable != 0)
-	{
-		retval = LIBIPMI_HL_GetChannelNum(&((wc_get_ipmi_session(wp))->IPMISession),
-					v6dnscfg.v6DNSServerEthIndex, &v6dnslanchannel, DEFAULT_IPMITIMEOUT);
-		if(retval != RPC_HAPI_SUCCESS)
-		{
-			TCRIT("Error in getting Channel number for Index(%d)::%x\n",
-						v6dnscfg.v6DNSServerEthIndex, retval);
-			goto error_out;
-		}
-		TDBG ("Success getting Eth Index::%d, Channel Number::%d\n",
-					v6dnscfg.v6DNSServerEthIndex, v6dnslanchannel);
-	}
-	if(inet_ntop(AF_INET6, v6dnscfg.v6DNSServer1, (char *)strV6DNSServer1 , INET6_ADDRSTRLEN) == NULL)
-	{
-		TCRIT("It is not a valid v6 DNS Server1\n");
-		retval = INVALID_V6IP;
-		goto error_out;
-	}
-	if(inet_ntop(AF_INET6, v6dnscfg.v6DNSServer2, (char *)strV6DNSServer2 , INET6_ADDRSTRLEN) == NULL)
-	{
-		TCRIT("It is not a valid v6 DNS Server2\n");
-		retval = INVALID_V6IP;
-		goto error_out;
-	}
-	TDBG ("Success converting V6 DNS Server1::%s, DNS Server2::%s\n",
-				strV6DNSServer1, strV6DNSServer2);
+			dnsCfg.DNSIndex, DNSLanChannel);
+	}
+
+
+	for (i=0; i<3; i++)
+	{
+		memset(DNSIPAddr, 0, IP6_ADDR_LEN);
+		retval = LIBIPMI_HL_AMIGetDNSIPAddress(
+			&((wc_get_ipmi_session(wp))->IPMISession), i+1, DNSIPAddr,
+			&Addrlen, DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in Getting DNS IP Address::%x for block::%d\n", retval, i+1);
+			goto error_out;
+		}
+
+		memset(strDNSServer[i], 0, INET6_ADDRSTRLEN);
+		if(Addrlen == IP_ADDR_LEN)
+		{
+			//memset(IPv4Addr,0,IP_ADDR_LEN);
+			//memcpy(IPv4Addr,DNSIPAddr,IP_ADDR_LEN);
+			if(inet_ntop(AF_INET, DNSIPAddr, (char *)strDNSServer[i],
+				INET_ADDRSTRLEN) == NULL)
+			{
+				TCRIT("It is not a valid v4 DNS Server for block::%d\n", i+1);
+				retval = INVALID_V4IP;
+				goto error_out;
+			}
+		}
+		else
+		{
+			if(inet_ntop(AF_INET6, DNSIPAddr, (char *)strDNSServer[i],
+				INET6_ADDRSTRLEN) == NULL)
+			{
+				TCRIT("It is not a valid v6 DNS Server for block::%d\n", i+1);
+				retval = INVALID_V6IP;
+				goto error_out;
+			}
+		}
+		TDBG ("Success getting the DNS Address%d::%s\n", i+1, strDNSServer[i]);
+	}
+
+	/*printf ("Host Setting::%d\n", hostCfg.HostSetting);
+	printf ("Host Name::%s\n", hostCfg.HostName);
+	printf ("Register BMC0::%d\n", RegBMC_FQDN[0] & 0x1);
+	printf ("Register BMC1::%d\n", RegBMC_FQDN[1] & 0x1);
+	printf ("Register BMC2::%d\n", RegBMC_FQDN[2] & 0x1);
+	printf ("Register DHCP0::%d\n", (RegBMC_FQDN[0] & 0x10)>>4);
+	printf ("Register DHCP1::%d\n", (RegBMC_FQDN[1] & 0x10)>>4);
+	printf ("Register DHCP2::%d\n", (RegBMC_FQDN[2] & 0x10)>>4);
+	printf ("DomainSetting::%s\n", strDomainCfg);
+	printf ("Domain Name::%s\n", domainName);
+	printf ("DNS LAN Channel::%d\n", DNSLanChannel);
+	printf ("DNS IP Prirority::%d\n", dnsCfg.IPPriority);
+	printf ("DNS Server1::%s\n", strDNSServer[0]);
+	printf ("DNS Server2::%s\n", strDNSServer[1]);
+	printf ("DNS Server3::%s\n", strDNSServer[2]);*/
 
 	WEBPAGE_WRITE_JSON_RECORD(GETDNSCFG,
-					v4dnscfg.DNSBMCHostSetting,
-					v4dnscfg.DNSBMCHostName,
-					v4dnscfg.DNSRegisterBMC[0],
-					v4dnscfg.DNSRegisterBMC[1],
-					v4dnscfg.DNSRegisterBMC[2],
-					v4dnscfg.RegisterBMCFQDN[0],
-					v4dnscfg.RegisterBMCFQDN[1],
-					v4dnscfg.RegisterBMCFQDN[2],
-					domainsetting,
-					v4dnscfg.DNSDomainName,
-					v4dnslanchannel,
-					strV4DNSServer1,
-					strV4DNSServer2,
-					v6dnslanchannel,
-					strV6DNSServer1,
-					strV6DNSServer2);
+		hostCfg.HostSetting,
+		hostCfg.HostName,
+		RegBMC_FQDN[0] & 0x1,
+		RegBMC_FQDN[1] & 0x1,
+		RegBMC_FQDN[2] & 0x1,
+		(RegBMC_FQDN[0] & 0x10)>>4,
+		(RegBMC_FQDN[1] & 0x10)>>4,
+		(RegBMC_FQDN[2] & 0x10)>>4,
+		strDomainCfg,
+		domainName,
+		DNSLanChannel,
+		dnsCfg.IPPriority,
+		strDNSServer[0],
+		strDNSServer[1],
+		strDNSServer[2]);
 error_out:
 	WEBPAGE_WRITE_JSON_END(GETDNSCFG, retval);
 	WEBPAGE_WRITE_END();
@@ -1048,184 +1091,266 @@
 	return RPC_HAPI_SUCCESS;
 }
 
+int setDNSServerCfg(IPMI20_SESSION_T *ipmiSession, int index, char *DNSServer)
+{
+	int retval = RPC_HAPI_SUCCESS;
+	INT8U iDNSServer[IP6_ADDR_LEN];
+
+	memset(iDNSServer, 0, IP6_ADDR_LEN);
+	retval = inet_pton(AF_INET, (char *)DNSServer, iDNSServer);
+	if (retval <= 0)
+	{
+		TINFO("Error while converting IPv4 address%d, trying IPv6...\n", index);
+		memset(iDNSServer, 0, IP6_ADDR_LEN);
+		retval = inet_pton(AF_INET6, (char*)DNSServer, iDNSServer);
+		if(retval <= 0)
+		{
+			TCRIT("Error while converting IPv6 DNS Server%d\n", index);
+			retval = INVALID_V4IP;
+			return retval;
+		}
+		TDBG ("Success converting v6 DNS Server%d::%d.%d.%d.%d %d %d %d %d %d %d %d %d %d %d %d %d\n",
+			index, iDNSServer[0], iDNSServer[1], iDNSServer[2], iDNSServer[3],
+			iDNSServer[4], iDNSServer[5], iDNSServer[6], iDNSServer[7],
+			iDNSServer[8], iDNSServer[9],iDNSServer[10],iDNSServer[11],
+			iDNSServer[12],iDNSServer[13],iDNSServer[14],iDNSServer[15]);
+		retval = LIBIPMI_HL_AMISetDNSIPAddress(ipmiSession, index,
+			iDNSServer, IP6_ADDR_LEN, DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting v6 DNS Server Address%d::%x\n", index, retval);
+			return retval;
+		}
+	}
+	else
+	{
+		TDBG ("Success converting v4 DNS Server%d::%d.%d.%d.%d\n",
+			index, iDNSServer[0], iDNSServer[1], iDNSServer[2], iDNSServer[3]);
+		retval = LIBIPMI_HL_AMISetDNSIPAddress(ipmiSession, index,
+			iDNSServer, IP_ADDR_LEN, DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting v4 DNS Server Address%d::%x\n", index, retval);
+			return retval;
+		}
+	}
+	return RPC_HAPI_SUCCESS;
+}
+
 WEBIFC_HAPIFN_DEFINE(WebSetDNSCfg)
 {
 	int retval = RPC_HAPI_SUCCESS;
-
-	DNS_CONFIG v4dnscfg;
-	V6DNS_CONFIG v6dnscfg;
-
-	WP_VAR_DECLARE(hostSetting, int);
-	WP_VAR_DECLARE(hostName, char*);
-	WP_VAR_DECLARE(registerBMC0, int);
-	WP_VAR_DECLARE(registerBMC1, int);
-	WP_VAR_DECLARE(registerBMC2, int);
-	WP_VAR_DECLARE(registerDHCP0, int);
-	WP_VAR_DECLARE(registerDHCP1, int);
-	WP_VAR_DECLARE(registerDHCP2, int);
-	WP_VAR_DECLARE(domainEthIndex, int);
-	WP_VAR_DECLARE(domainV4V6, char*);
-	WP_VAR_DECLARE(domainName, char*);
-	WP_VAR_DECLARE(v4DNSDHCPEnable, int);
-	WP_VAR_DECLARE(v4DNSEthIndex, int);
-	WP_VAR_DECLARE(v4DNS1, char*);
-	WP_VAR_DECLARE(v4DNS2, char*);
-	WP_VAR_DECLARE(v6DNSDHCPEnable, int);
-	WP_VAR_DECLARE(v6DNSEthIndex, int);
-	WP_VAR_DECLARE(v6DNS1, char*);
-	WP_VAR_DECLARE(v6DNS2, char*);
+	int i;
+
+	HOSTNAMECONF hostCfg;
+	DomainSetting domainCfg;
+	INT8U RegBMC_FQDN[MAX_CHANNEL];
+	DNSSetting dnsCfg;
+
+	WP_VAR_DECLARE(HOST_CFG, VARTYPE_INT);
+	WP_VAR_DECLARE(HOST_NAME, VARTYPE_STR);
+	WP_VAR_DECLARE(REG_BMC0, VARTYPE_INT);
+	WP_VAR_DECLARE(REG_BMC1, VARTYPE_INT);
+	WP_VAR_DECLARE(REG_BMC2, VARTYPE_INT);
+	WP_VAR_DECLARE(REG_DHCP0, VARTYPE_INT);
+	WP_VAR_DECLARE(REG_DHCP1, VARTYPE_INT);
+	WP_VAR_DECLARE(REG_DHCP2, VARTYPE_INT);
+	WP_VAR_DECLARE(DOMAIN_ETH, VARTYPE_INT);
+	WP_VAR_DECLARE(DOMAIN_PRIORITY, VARTYPE_STR);
+	WP_VAR_DECLARE(DOMAIN_NAME, VARTYPE_STR);
+	WP_VAR_DECLARE(DNS_DHCP, VARTYPE_INT);
+	WP_VAR_DECLARE(DNS_ETH, VARTYPE_INT);
+	WP_VAR_DECLARE(DNS_PRIORITY, VARTYPE_INT);
+	WP_VAR_DECLARE(DNS_IP1, VARTYPE_STR);
+	WP_VAR_DECLARE(DNS_IP2, VARTYPE_STR);
+	WP_VAR_DECLARE(DNS_IP3, VARTYPE_STR);
 
 	WEBPAGE_WRITE_BEGIN();
 	WEBPAGE_WRITE_JSON_BEGIN(SETDNSCFG);
 
-	memset (&v4dnscfg, 0, sizeof(DNS_CONFIG));
 	/* This block will fill the host configuration arguments */
-	hostSetting = WP_GET_VAR_INT(hostSetting);
-	hostName = WP_GET_VAR_STR(hostName);
-	v4dnscfg.DNSBMCHostSetting = hostSetting;
-	v4dnscfg.DNSBMCHostNameLen = strlen(hostName);
-	strcpy((char *)&v4dnscfg.DNSBMCHostName, hostName);
-	TDBG ("WEBIFC, DNSBMCHostSetting::%d, DNSBMCHostNameLen::%d, DNSBMCHostName::%s\n",
-				v4dnscfg.DNSBMCHostSetting, v4dnscfg.DNSBMCHostNameLen, v4dnscfg.DNSBMCHostName);
+	memset(&hostCfg, 0, sizeof(HOSTNAMECONF));
+	HOST_CFG = WP_GET_VAR_INT(HOST_CFG);
+	hostCfg.HostSetting = HOST_CFG;
+	if (HOST_CFG == 0)
+	{
+		HOST_NAME = WP_GET_VAR_STR(HOST_NAME);
+		hostCfg.HostNameLen = strlen(HOST_NAME);
+		strcpy((char *)&hostCfg.HostName, HOST_NAME);
+		TDBG ("WebSetDNSCfg, HostSetting::%d, HostNameLen::%d, HostName::%s\n",
+			hostCfg.HostSetting, hostCfg.HostNameLen, hostCfg.HostName);
+	}
+
+	retval = LIBIPMI_HL_AMISetDNSHostSetting(
+		&((wc_get_ipmi_session(wp))->IPMISession),
+		(INT8U*)&hostCfg, DEFAULT_IPMITIMEOUT);
+	if (retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in setting Host configuration::%x\n",retval);
+		goto error_out;
+	}
 
 	/* This block will fill the register BMC arguments */
-	registerBMC0 = WP_GET_VAR_INT(registerBMC0);
-	registerBMC1 = WP_GET_VAR_INT(registerBMC1);
-	registerBMC2 = WP_GET_VAR_INT(registerBMC2);
-	v4dnscfg.DNSRegisterBMC[0] = registerBMC0;
-	v4dnscfg.DNSRegisterBMC[1] = registerBMC1;
-	v4dnscfg.DNSRegisterBMC[2] = registerBMC2;
-
-	registerDHCP0 = WP_GET_VAR_INT(registerDHCP0);
-	registerDHCP1 = WP_GET_VAR_INT(registerDHCP1);
-	registerDHCP2 = WP_GET_VAR_INT(registerDHCP2);
-	v4dnscfg.RegisterBMCFQDN[0] = registerDHCP0;
-	v4dnscfg.RegisterBMCFQDN[1] = registerDHCP1;
-	v4dnscfg.RegisterBMCFQDN[2] = registerDHCP2;
+	REG_BMC0 = WP_GET_VAR_INT(REG_BMC0);
+	REG_BMC1 = WP_GET_VAR_INT(REG_BMC1);
+	REG_BMC2 = WP_GET_VAR_INT(REG_BMC2);
+	RegBMC_FQDN[0] = REG_BMC0;
+	RegBMC_FQDN[1] = REG_BMC1;
+	RegBMC_FQDN[2] = REG_BMC2;
+
+	REG_DHCP0 = WP_GET_VAR_INT(REG_DHCP0);
+	REG_DHCP1 = WP_GET_VAR_INT(REG_DHCP1);
+	REG_DHCP2 = WP_GET_VAR_INT(REG_DHCP2);
+	RegBMC_FQDN[0] |= REG_DHCP0 << 4;
+	RegBMC_FQDN[1] |= REG_DHCP1 << 4;
+	RegBMC_FQDN[2] |= REG_DHCP2 << 4;
+
+	retval = LIBIPMI_HL_AMISetDNSRegister(
+		&((wc_get_ipmi_session(wp))->IPMISession), 
+		RegBMC_FQDN, DEFAULT_IPMITIMEOUT);
+	if (retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error in setting Register DNS configuration::%x\n",retval);
+		goto error_out;
+	}
 
 	/* This block will fill the domain name configuration arguments */
-	domainV4V6 = WP_GET_VAR_STR(domainV4V6);
-	if (strncmp(domainV4V6,"Manual",6) == 0)
-	{
-		v4dnscfg.DNSDomainNameDhcpEnable = 0;
-		domainName = WP_GET_VAR_STR(domainName);
-		v4dnscfg.DNSDomainNameLen = strlen(domainName);
-		strcpy((char *)&v4dnscfg.DNSDomainName, domainName);
+	memset (&domainCfg, 0, sizeof(DomainSetting));
+	DOMAIN_PRIORITY = WP_GET_VAR_STR(DOMAIN_PRIORITY);
+	if (strncmp(DOMAIN_PRIORITY, "Manual", 6) == 0)
+	{
+		domainCfg.DomainDHCP = 0;
+		DOMAIN_NAME = WP_GET_VAR_STR(DOMAIN_NAME);
+		domainCfg.DomainLen= strlen(DOMAIN_NAME);
+		TDBG("WebSetDNSCfg, Domain Priority::%s, Domain Name::%s\n",
+			DOMAIN_PRIORITY, DOMAIN_NAME);
+		retval = LIBIPMI_HL_AMISetDNSDomainSetting(
+			&((wc_get_ipmi_session(wp))->IPMISession),
+			(INT8U *) &domainCfg, DEFAULT_IPMITIMEOUT);
+		if (retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting DNS Domain configuration::%x\n", retval);
+			goto error_out;
+		}
+
+		for(i=0; i<4; i++)
+		{
+			retval = LIBIPMI_HL_AMISetDNSDomainName(
+				&((wc_get_ipmi_session(wp))->IPMISession),
+				i+1, (INT8U*)&DOMAIN_NAME[MAX_DOMAIN_BLOCK_SIZE * i],
+				DEFAULT_IPMITIMEOUT);
+			if(retval != RPC_HAPI_SUCCESS)
+			{
+				TCRIT("Error in setting Domain name configuration::%x for block::%d\n",
+					retval, i);
+				goto error_out;
+			}
+		}
 	}
 	else
 	{
-		v4dnscfg.DNSDomainNameDhcpEnable = 1;
-		domainEthIndex = WP_GET_VAR_INT(domainEthIndex);
-		v4dnscfg.DNSDomainNameEthIndex = domainEthIndex;
-		if (strcmp(domainV4V6, "v4") == 0){
-			v4dnscfg.v4v6 = DOMAIN_V4;
-		}else if(strcmp(domainV4V6, "v6") == 0){
-			v4dnscfg.v4v6 = DOMAIN_V6;
-		}
-	}
-
-	/* This block will validate and fill the v4 DNS server configuration arguments */
-	v4DNSDHCPEnable = WP_GET_VAR_INT(v4DNSDHCPEnable);
-	v4dnscfg.DNSDhcpEnable = v4DNSDHCPEnable;
-	if (v4DNSDHCPEnable == 1)		//Check Dynamic configuration
-	{
-		v4DNSEthIndex = WP_GET_VAR_INT(v4DNSEthIndex);
-		v4dnscfg.DNSServerEthIndex = v4DNSEthIndex;
-	}else		//Manual Configuration
-	{
-		v4DNS1 = WP_GET_VAR_STR(v4DNS1);
-		v4DNS2 = WP_GET_VAR_STR(v4DNS2);
-		retval = inet_pton(AF_INET, (char *)v4DNS1, v4dnscfg.DNSServer1);
-		if (retval <= 0)
-		{
-			TCRIT("Error while converting V4 DNS Server1::%d\n",retval);
-			retval = INVALID_V4IP;
-			goto error_out;
-		}
-		TDBG ("Success converting v4 DNS Server1::%d.%d.%d.%d\n",
-			v4dnscfg.DNSServer1[0], v4dnscfg.DNSServer1[1],
-			v4dnscfg.DNSServer1[2], v4dnscfg.DNSServer1[3]);
-
-		if (strlen(v4DNS2) != 0)
-		{
-			retval = inet_pton(AF_INET, (char *)v4DNS2, v4dnscfg.DNSServer2);
-			if (retval <= 0)
-			{
-				TCRIT("Error while converting V4 DNS Server2::%d\n",retval);
-				retval = INVALID_V4IP;
+		domainCfg.DomainDHCP = 1;
+		DOMAIN_ETH = WP_GET_VAR_INT(DOMAIN_ETH);
+		domainCfg.DomainIndex = DOMAIN_ETH;
+		if (strcmp(DOMAIN_PRIORITY, "v4") == 0){
+			domainCfg.Domainpriority = DOMAIN_V4;
+		}else if(strcmp(DOMAIN_PRIORITY, "v6") == 0){
+			domainCfg.Domainpriority = DOMAIN_V6;
+		}
+
+		TDBG("WebSetDNSCfg, DomainIndex::%d, Domainpriority::%d\n",
+			domainCfg.DomainIndex, domainCfg.Domainpriority);
+		retval = LIBIPMI_HL_AMISetDNSDomainSetting(
+			&((wc_get_ipmi_session(wp))->IPMISession), 
+			(INT8U*)&domainCfg, DEFAULT_IPMITIMEOUT);
+		if (retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting DNS Domain configuration::%x\n",retval);
+			goto error_out;
+		}
+	}
+
+	/* This block will validate and fill the DNS server configuration arguments */
+	memset (&dnsCfg, 0, sizeof(DNSSetting));
+	DNS_DHCP = WP_GET_VAR_INT(DNS_DHCP);
+	if (DNS_DHCP == 0)
+	{
+		dnsCfg.DNSDHCP = DNS_DHCP;
+		retval = LIBIPMI_HL_AMISetDNSSetting(
+			&((wc_get_ipmi_session(wp))->IPMISession),
+			(INT8U*)&dnsCfg, DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting DNS configuration::%x\n", retval);
+			goto error_out;
+		}
+
+		DNS_IP1 = WP_GET_VAR_STR(DNS_IP1);
+		if (strlen(DNS_IP1) != 0)
+		{
+			retval = setDNSServerCfg(&((wc_get_ipmi_session(wp))->IPMISession), 1,
+				DNS_IP1);
+			if(retval != RPC_HAPI_SUCCESS)
+			{
+				TCRIT("Error in setting  DNS Server Address1::%x\n", retval);
 				goto error_out;
 			}
-			TDBG ("Success converting v4 DNS Server2::%d.%d.%d.%d\n",
-				v4dnscfg.DNSServer2[0], v4dnscfg.DNSServer2[1],
-				v4dnscfg.DNSServer2[2], v4dnscfg.DNSServer2[3]);
-		}
-	}
-
-	/* This block will validate and fill the v6 DNS server configuration arguments */
-	memset (&v6dnscfg, 0, sizeof(V6DNS_CONFIG));
-	v6DNSDHCPEnable = WP_GET_VAR_INT(v6DNSDHCPEnable);
-	v6dnscfg.v6DNSDhcpEnable = v6DNSDHCPEnable;
-	if (v6DNSDHCPEnable == 1)		//Check Dynamic configuration
-	{
-		v6DNSEthIndex = WP_GET_VAR_INT(v6DNSEthIndex);
-		v6dnscfg.v6DNSServerEthIndex = v6DNSEthIndex;
-	}
-	else		//Manual Configuration
-	{
-		v6DNS1 = WP_GET_VAR_STR(v6DNS1);
-		v6DNS2 = WP_GET_VAR_STR(v6DNS2);
-		retval = inet_pton(AF_INET6, (char *)v6DNS1, v6dnscfg.v6DNSServer1);
-		if (retval <= 0)
-		{
-			TCRIT("Error while converting V6 DNS Server1::%d\n",retval);
-			retval = INVALID_V6IP;
-			goto error_out;
-		}
-		TDBG ("Success converting v6 DNS Server1::%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
-					v6dnscfg.v6DNSServer1[0],v6dnscfg.v6DNSServer1[1],v6dnscfg.v6DNSServer1[2],v6dnscfg.v6DNSServer1[3],
-					v6dnscfg.v6DNSServer1[4],v6dnscfg.v6DNSServer1[5],v6dnscfg.v6DNSServer1[6],v6dnscfg.v6DNSServer1[7],
-					v6dnscfg.v6DNSServer1[8],v6dnscfg.v6DNSServer1[9],v6dnscfg.v6DNSServer1[10],v6dnscfg.v6DNSServer1[11],
-					v6dnscfg.v6DNSServer1[12],v6dnscfg.v6DNSServer1[13],v6dnscfg.v6DNSServer1[14],v6dnscfg.v6DNSServer1[15]);
-
-		retval = inet_pton(AF_INET6, (char *)v6DNS2, v6dnscfg.v6DNSServer2);
-		if (retval <= 0)
-		{
-			TCRIT("Error while converting V6 DNS Server2::%d\n",retval);
-			retval = INVALID_V6IP;
-			goto error_out;
-		}
-		TDBG ("Success converting v6 DNS Server2::%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X\n",
-					v6dnscfg.v6DNSServer2[0],v6dnscfg.v6DNSServer2[1],v6dnscfg.v6DNSServer2[2],v6dnscfg.v6DNSServer2[3],
-					v6dnscfg.v6DNSServer2[4],v6dnscfg.v6DNSServer2[5],v6dnscfg.v6DNSServer2[6],v6dnscfg.v6DNSServer2[7],
-					v6dnscfg.v6DNSServer2[8],v6dnscfg.v6DNSServer2[9],v6dnscfg.v6DNSServer2[10],v6dnscfg.v6DNSServer2[11],
-					v6dnscfg.v6DNSServer2[12],v6dnscfg.v6DNSServer2[13],v6dnscfg.v6DNSServer2[14],v6dnscfg.v6DNSServer2[15]);
-	}
-
-	/* This block will save hostname, register BMC, domain name, v4 DNS server and
-	v6 DNS server configuration */
-	retval = LIBIPMI_HL_AMISetAllDNSConf(&((wc_get_ipmi_session(wp))->IPMISession),
-					(INT8U *)&v4dnscfg, DEFAULT_IPMITIMEOUT);
-	if (retval != RPC_HAPI_SUCCESS)
-	{
-		TCRIT("Error in setting Host, domain and v4 DNS configuration::%x\n",retval);
-		goto error_out;
-	}
-	TDBG ("Success setting Host, domain and v4 DNS configuration\n");
-
-	retval = LIBIPMI_HL_AMISetV6DNSConf(&((wc_get_ipmi_session(wp))->IPMISession),
-					(INT8U *)&v6dnscfg, DEFAULT_IPMITIMEOUT);
-	if (retval != RPC_HAPI_SUCCESS)
-	{
-		TCRIT("Error in setting v64 DNS configuration::%x\n",retval);
-		goto error_out;
-	}
-	TDBG ("Success setting v6 DNS configuration\n");
-
+		}
+
+		DNS_IP2 = WP_GET_VAR_STR(DNS_IP2);
+		if (strlen(DNS_IP2) != 0)
+		{
+			retval = setDNSServerCfg(&((wc_get_ipmi_session(wp))->IPMISession), 2,
+				DNS_IP2);
+			if(retval != RPC_HAPI_SUCCESS)
+			{
+				TCRIT("Error in setting  DNS Server Address2::%x\n", retval);
+				goto error_out;
+			}
+		}
+
+		DNS_IP3 = WP_GET_VAR_STR(DNS_IP3);
+		if (strlen(DNS_IP3) != 0)
+		{
+			retval = setDNSServerCfg(&((wc_get_ipmi_session(wp))->IPMISession), 3,
+				DNS_IP3);
+			if(retval != RPC_HAPI_SUCCESS)
+			{
+				TCRIT("Error in setting  DNS Server Address3::%x\n", retval);
+				goto error_out;
+			}
+		}
+	}
+	else 
+	{
+		DNS_ETH = WP_GET_VAR_INT(DNS_ETH);
+		DNS_PRIORITY = WP_GET_VAR_INT(DNS_PRIORITY);
+		dnsCfg.DNSDHCP = DNS_DHCP;
+		dnsCfg.DNSIndex = DNS_ETH;
+		dnsCfg.IPPriority = DNS_PRIORITY;
+		TDBG("WebSetDNSCfg, DNS DHCP::%d, DNS Index::%d, IP Priority::%d\n",
+			dnsCfg.DNSDHCP, dnsCfg.DNSIndex, dnsCfg.IPPriority);
+		retval = LIBIPMI_HL_AMISetDNSSetting(
+			&((wc_get_ipmi_session(wp))->IPMISession),
+			(INT8U*)&dnsCfg, DEFAULT_IPMITIMEOUT);
+		if(retval != RPC_HAPI_SUCCESS)
+		{
+			TCRIT("Error in setting DNS configuration::%x\n", retval);
+			goto error_out;
+		}
+	}
+
+	retval = LIBIPMI_HL_AMISetDNSRestart(
+		&((wc_get_ipmi_session(wp))->IPMISession), DEFAULT_IPMITIMEOUT);
+	if(retval != RPC_HAPI_SUCCESS)
+	{
+		TCRIT("Error applying DNS configuration changes\n");
+	}
 error_out:
 	WEBPAGE_WRITE_JSON_END(SETDNSCFG,retval);
 	WEBPAGE_WRITE_END();
 
-	return 0;
+	return RPC_HAPI_SUCCESS;
 }
 
 WEBIFC_HAPIFN_DEFINE(WEBIFC_IPMICMD_HL_GetLANDestsTable)
