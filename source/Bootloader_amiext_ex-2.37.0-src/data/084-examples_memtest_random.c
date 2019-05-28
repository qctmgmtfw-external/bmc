--- uboot/examples/memtest/random.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.exam/examples/memtest/random.c	2014-07-28 17:23:30.961408921 -0400
@@ -0,0 +1,23 @@
+#define SEED 1753
+
+static unsigned long seed = SEED;
+static unsigned long call =1;
+
+unsigned long
+rand(void)
+{
+
+		if ((seed % 2) > 0) 
+		{
+			seed ^= call;
+			seed >>= 1;
+			seed |= 0x80000000;
+		} 
+		else 
+		{
+			seed ^= ~call;
+			seed >>= 1;
+		}						
+	call++;
+	return seed;
+}
