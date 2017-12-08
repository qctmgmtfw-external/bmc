--- .pristine/nsupdate-1.10.0-src/data/nsupdate.c Fri Feb  3 16:18:08 2012
+++ source/nsupdate-1.10.0-src/data/nsupdate.c Tue Jan 31 16:25:02 2012
@@ -2243,7 +2243,7 @@
 		ddebug("retrying soa request without TSIG");
 		result = dns_request_createvia3(requestmgr, soaquery,
 						localaddr, addr, 0, NULL,
-						FIND_TIMEOUT * 20,
+						FIND_TIMEOUT,
 						FIND_TIMEOUT, 3,
 						global_task, recvsoa, reqinfo,
 						&request);
@@ -2418,7 +2418,7 @@
 	reqinfo->addr = destaddr;
 	result = dns_request_createvia3(requestmgr, msg, srcaddr, destaddr, 0,
 					(userserver != NULL) ? tsigkey : NULL,
-					FIND_TIMEOUT * 20, FIND_TIMEOUT, 3,
+					3, FIND_TIMEOUT, 3,
 					global_task, recvsoa, reqinfo, request);
 	check_result(result, "dns_request_createvia");
 	requests++;
@@ -2535,7 +2535,7 @@
 
 	options |= DNS_REQUESTOPT_TCP;
 	result = dns_request_createvia3(requestmgr, msg, srcaddr, destaddr,
-					options, tsigkey, FIND_TIMEOUT * 20,
+					options, tsigkey, FIND_TIMEOUT,
 					FIND_TIMEOUT, 3, global_task, recvgss,
 					reqinfo, request);
 	check_result(result, "dns_request_createvia3");
