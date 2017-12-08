--- .pristine/libuserm-1.12.0-src/data/commonio.h Wed Sep 14 14:28:24 2011
+++ source/libuserm-1.12.0-src/data/commonio.h Wed Sep 14 15:40:47 2011
@@ -7,7 +7,7 @@
 	char *line;
 	void *eptr;  /* struct passwd, struct spwd, ... */
 	struct commonio_entry *prev, *next;
-	int changed:1;
+	unsigned int changed:1;
 };
 
 /*
@@ -78,10 +78,10 @@
 	/*
 	 * Various flags.
 	 */
-	int changed:1;
-	int isopen:1;
-	int locked:1;
-	int readonly:1;
+	unsigned changed:1;
+	unsigned  isopen:1;
+	unsigned  locked:1;
+	unsigned  readonly:1;
 };
 
 extern int commonio_setname(struct commonio_db *, const char *);
