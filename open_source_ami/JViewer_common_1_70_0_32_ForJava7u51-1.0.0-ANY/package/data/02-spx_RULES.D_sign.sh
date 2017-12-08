--- .pristine/JViewer_common-1.70.0-src/spx/RULES.D/sign.sh Thu Jan 23 17:39:33 2014
+++ source/JViewer_common-1.70.0-src/spx/RULES.D/sign.sh Thu Jan 23 19:18:30 2014
@@ -2,8 +2,30 @@
 
 echo "Signing the jar files"
 
+#Location of valid Jar signing key and credentials
+KeyStore=${TOOLDIR}/../../JavaSigning/JViewerKey
+KeyCredentials=${TOOLDIR}/../../JavaSigning/KeyCredentials
 
-${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore ${TEMPDIR}/../certs/amiKeys -storepass megarac ${BUILD}/${PACKAGE_NAME}/data/JViewer.jar jviewer
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
+
+${TOOLDIR}/JDK/jdk1.5.0_01/bin/jarsigner -keystore $keystore -storepass $storepass ${BUILD}/${PACKAGE_NAME}/data/JViewer.jar $aliasname
 
 lin_err=$?
 
