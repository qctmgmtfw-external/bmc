--- .pristine/ipmi_dev-2.100.0-src/data/AMIConf.h Thu Sep 15 22:48:04 2011
+++ source/ipmi_dev-2.100.0-src/data/AMIConf.h Thu Sep 15 22:58:39 2011
@@ -46,5 +46,6 @@
 
 extern int AMIGetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
 extern int AMISetSELPolicy (_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
+extern int AMIGetSELEntires(_NEAR_ INT8U *pReq, INT32U ReqLen, _NEAR_ INT8U *pRes, int BMCInst);
 
 #endif /* __AMICONF_H__ */
