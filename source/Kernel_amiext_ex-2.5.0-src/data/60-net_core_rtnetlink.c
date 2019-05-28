--- linux-3.14.17/net/core/rtnetlink.c	2014-08-13 21:38:34.000000000 -0400
+++ linux.new/net/core/rtnetlink.c	2014-08-21 11:21:42.681540618 -0400
@@ -2881,6 +2881,8 @@
 	case NETDEV_UNREGISTER_FINAL:
 	case NETDEV_RELEASE:
 	case NETDEV_JOIN:
+	case NETDEV_LINK_UP:
+	case NETDEV_LINK_DOWN:
 		break;
 	default:
 		rtmsg_ifinfo(RTM_NEWLINK, dev, 0, GFP_KERNEL);
