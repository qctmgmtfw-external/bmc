--- .pristine/ipmi_dev-2.100.0-src/data/AMIConf.h Fri Sep 16 23:40:54 2011
+++ source/ipmi_dev-2.100.0-src/data/AMIConf.h Sat Sep 17 00:01:46 2011
@@ -47,5 +47,5 @@
 extern int AMIGetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
 extern int AMISetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
 extern int AMIGetSELEntires(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
-
+extern int AMIGetSenforInfo(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
 #endif /* __AMICONF_H__ */
