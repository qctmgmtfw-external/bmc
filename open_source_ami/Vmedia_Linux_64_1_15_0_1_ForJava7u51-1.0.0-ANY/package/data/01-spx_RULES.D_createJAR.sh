--- .pristine/Vmedia_Linux_64-1.15.0-src/spx/RULES.D/createJAR.sh Thu Jan 23 17:39:41 2014
+++ source/Vmedia_Linux_64-1.15.0-src/spx/RULES.D/createJAR.sh Thu Jan 23 19:22:53 2014
@@ -1,4 +1,27 @@
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
+
 
 cd  ${BUILD}/${PACKAGE_NAME}/data/
 
@@ -12,7 +35,7 @@
 	exit 1
 fi
 
-${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore ${TEMPDIR}/../certs/amiKeys -storepass megarac ${BUILD}/${PACKAGE_NAME}/data/Linux_x86_64.jar jviewer
+${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore $keystore -storepass $storepass ${BUILD}/${PACKAGE_NAME}/data/Linux_x86_64.jar $aliasname
 
 lin_err=$?
 
