--- linux_org/include/linux/mii.h	2016-05-13 10:37:29.253958572 +0530
+++ linux/include/linux/mii.h	2016-05-13 10:44:16.313968823 +0530
@@ -73,8 +73,10 @@
 		ret = LPA_100HALF;
 	else if (negotiated & LPA_10FULL)
 		ret = LPA_10FULL;
-	else
+	else if (negotiated & LPA_10HALF)
 		ret = LPA_10HALF;
+	else 
+		ret = 0;
 
 	return ret;
 }
