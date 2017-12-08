--- .pristine/libldapauth-1.16.0-src/data/Ldap_query.c Wed Sep 14 14:28:26 2011
+++ source/libldapauth-1.16.0-src/data/Ldap_query.c Wed Sep 14 15:47:30 2011
@@ -685,6 +685,7 @@
 					free(ret[i].bv_val);
 			}
 			free(ret);
+			return NULL;
 		}	
 	}
 	// search sucess, but no values to send or we have some result
