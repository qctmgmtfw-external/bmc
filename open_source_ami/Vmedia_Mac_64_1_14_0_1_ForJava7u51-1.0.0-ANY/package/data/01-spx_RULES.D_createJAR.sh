--- .pristine/Vmedia_Mac_64-1.14.0-src/spx/RULES.D/createJAR.sh Thu Jan 23 17:39:41 2014
+++ source/Vmedia_Mac_64-1.14.0-src/spx/RULES.D/createJAR.sh Thu Jan 23 19:23:07 2014
@@ -1,4 +1,26 @@
 #/bin/sh
+
+#Location of valid Jar signing key and credentials
+KeyStore=${TOOLDIR}/../../JavaSigning/JViewerKey
+KeyCredentials=${TOOLDIR}/../../JavaSigning/KeyCredentials
+
+#Dummy keyfile, storepass and alias name
+keystore=${TEMPDIR}/../certs/amiKeys
+storepass=megarac
+aliasname=jviewer
+
+if [ -f $KeyStore ]; then
+	if [ -f $KeyCredentials ]; then
+		echo "Signing with Valid key..."
+		keystore=$KeyStore
+		storepass=$(grep storepass $KeyCredentials | cut -d':' -f2)
+		aliasname=$(grep aliasname $KeyCredentials | cut -d':' -f2)
+	else
+		echo "Signing with Self Signed key. This jar file might get blocked by Java Web Start!" 
+	fi
+else 
+	echo "Signing with Self Signed key. This jar file might get blocked by Java Web Start!" 
+fi
 
 
 cd  ${BUILD}/${PACKAGE_NAME}/data/
@@ -13,7 +35,7 @@
 	exit 1
 fi
 
-${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore ${TEMPDIR}/../certs/amiKeys -storepass megarac ${BUILD}/${PACKAGE_NAME}/data/Mac64.jar jviewer
+${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore $keystore -storepass $storepass ${BUILD}/${PACKAGE_NAME}/data/Mac64.jar $aliasname
 
 lin_err=$?
 
