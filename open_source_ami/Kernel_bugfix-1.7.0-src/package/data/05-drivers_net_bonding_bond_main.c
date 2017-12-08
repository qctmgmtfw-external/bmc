--- linux/drivers/net/bonding/bond_main.c	2011-02-18 15:41:01.343754422 +0530
+++ linux.new/drivers/net/bonding/bond_main.c	2011-03-07 17:58:57.611129907 +0530
@@ -1711,6 +1711,7 @@
 	case BOND_MODE_ALB:
 		new_slave->state = BOND_STATE_ACTIVE;
 		bond_set_slave_inactive_flags(new_slave);
+		bond_select_active_slave(bond);
 		break;
 	default:
 		dprintk("This slave is always active in trunk mode\n");
