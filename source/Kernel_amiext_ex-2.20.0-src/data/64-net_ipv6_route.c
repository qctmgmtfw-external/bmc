--- linux.pristine/net/ipv6/route.c	2013-12-12 01:34:28.000000000 -0500
+++ linux-amiext/net/ipv6/route.c	2014-02-03 15:11:17.167028630 -0500
@@ -696,6 +696,16 @@
 
 		if (next != rt0)
 			fn->rr_ptr = next;
+
+		/*
+			It's a workaround to avoid selecting the second
+			router as a default router when the first router
+			has not expired
+		*/
+		if(fn->rr_ptr != fn->leaf)
+		{
+			fn->rr_ptr = fn->leaf;
+		}
 	}
 
 	net = dev_net(rt0->dst.dev);
