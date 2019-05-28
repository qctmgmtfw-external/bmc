--- linux-3.14.17/net/ipv6/addrconf.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/net/ipv6/addrconf.c	2014-08-21 11:18:30.849537016 -0400
@@ -169,9 +169,9 @@
 	.forwarding		= 0,
 	.hop_limit		= IPV6_DEFAULT_HOPLIMIT,
 	.mtu6			= IPV6_MIN_MTU,
-	.accept_ra		= 1,
+	.accept_ra		= 0,
 	.accept_redirects	= 1,
-	.autoconf		= 1,
+	.autoconf		= 0,
 	.force_mld_version	= 0,
 	.mldv1_unsolicited_report_interval = 10 * HZ,
 	.mldv2_unsolicited_report_interval = HZ,
@@ -197,7 +197,7 @@
 	.proxy_ndp		= 0,
 	.accept_source_route	= 0,	/* we do not accept RH0 by default. */
 	.disable_ipv6		= 0,
-	.accept_dad		= 1,
+	.accept_dad		= 2,
 	.suppress_frag_ndisc	= 1,
 };
 
@@ -233,7 +233,7 @@
 	.proxy_ndp		= 0,
 	.accept_source_route	= 0,	/* we do not accept RH0 by default. */
 	.disable_ipv6		= 0,
-	.accept_dad		= 1,
+	.accept_dad		= 2,
 	.suppress_frag_ndisc	= 1,
 };
 
@@ -4991,14 +4991,14 @@
 			.data		= &ipv6_devconf.rtr_solicit_interval,
 			.maxlen		= sizeof(int),
 			.mode		= 0644,
-			.proc_handler	= proc_dointvec_jiffies,
+			.proc_handler	= proc_dointvec_userhz_jiffies,
 		},
 		{
 			.procname	= "router_solicitation_delay",
 			.data		= &ipv6_devconf.rtr_solicit_delay,
 			.maxlen		= sizeof(int),
 			.mode		= 0644,
-			.proc_handler	= proc_dointvec_jiffies,
+			.proc_handler	= proc_dointvec_userhz_jiffies,
 		},
 		{
 			.procname	= "force_mld_version",
