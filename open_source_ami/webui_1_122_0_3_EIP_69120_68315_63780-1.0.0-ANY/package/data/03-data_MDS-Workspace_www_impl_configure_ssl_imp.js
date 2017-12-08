--- .pristine/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_ssl_imp.js Tue Sep 20 20:25:10 2011
+++ source/webui-1.122.0-src/data/MDS-Workspace/www/impl/configure_ssl_imp.js Tue Sep 20 20:26:23 2011
@@ -284,14 +284,13 @@
 			break;
 		case 1: case 2:
 		case 3: case 5:
-		case 7:
+		case 0x80: case 0x81:
 			alert(eLang.getString("common", "STR_CONF_SSL_CERT_ERR" + sslValid));
 			break;
 		//case 4: case 6:
 		default:
 			alert(eLang.getString("common", "STR_CONF_SSL_CERT_ERR4"));
 			break;
-				
 		}
 	} else {
 		errstr = eLang.getString("common", "STR_CONF_SSL_VALIDATE_ERR");
