--- u-boot-2013.07/arch/arm/cpu/astcommon/timer.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/arch/arm/cpu/astcommon/timer.c	2013-12-05 12:17:55.079503948 -0500
@@ -0,0 +1,153 @@
+/*
+ * (C) Copyright 2006
+ * American Megatrends Inc.
+ *
+ * Timer functions for the AST2X00 SOC
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+#include <common.h>
+#include "soc_hw.h"
+
+#define TIMER_LOAD_VAL 0xffffffff
+#define READ_TIMER (*(volatile ulong *) TIMER1_COUNT_REG) /* use timer 1 */
+
+DECLARE_GLOBAL_DATA_PTR;
+
+#define timestamp gd->arch.tbl
+#define lastdec gd->arch.lastinc
+
+int timer_init (void)
+{
+
+	/* Load the counter and start timer */
+	*((volatile ulong *) TIMER1_RELOAD_REG) = TIMER_LOAD_VAL;
+	*((volatile ulong *) TIMER_CONTROL_REG) = (TIMER1_ENABLE | TIMER1_CLOCK_SELECT);  /* enable timer 1, use external clock, disable interrupt */
+
+	/* init the timestamp and lastdec value */
+	reset_timer_masked();
+
+	return 0;
+}
+
+/*
+ * timer without interrupts
+ */
+ulong get_timer (ulong base)
+{
+	return get_timer_masked () - base;
+}
+
+/* delay x useconds AND preserve advance timestamp value */
+void __udelay (unsigned long usec)
+{
+	ulong tmo, tmp;
+
+	if(usec >= 1000){		/* if "big" number, spread normalization to seconds */
+		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
+		tmo *= CONFIG_SYS_HZ;	/* find number of "ticks" to wait to achieve target */
+		tmo /= 1000;		/* finish normalize. */
+	}else{				/* else small number, don't kill it prior to HZ multiply */
+		tmo = usec * CONFIG_SYS_HZ;
+		tmo /= (1000*1000);
+	}
+
+	tmp = get_timer (0);		/* get current timestamp */
+	if( (tmo + tmp + 1) < tmp )	/* if setting this fordward will roll time stamp */
+		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastdec value */
+	else
+		tmo += tmp;		/* else, set advancing stamp wake up time */
+
+	while (get_timer_masked () < tmo)/* loop till event */
+		/*NOP*/;
+}
+
+void reset_timer_masked (void)
+{
+	/* reset time */
+	lastdec = READ_TIMER;  /* capure current decrementer value time */
+	timestamp = 0;	       /* start "advancing" time stamp from 0 */
+}
+
+ulong get_timer_masked (void)
+{
+	ulong now = READ_TIMER;		/* current tick value */
+
+	if (lastdec >= now) {		/* normal mode (non roll) */
+		/* normal mode */
+		timestamp += lastdec - now; /* move stamp fordward with absoulte diff ticks */
+	} else {			/* we have overflow of the count down timer */
+		/* nts = ts + ld + (TLV - now)
+		 * ts=old stamp, ld=time that passed before passing through -1
+		 * (TLV-now) amount of time after passing though -1
+		 * nts = new "advancing time stamp"...it could also roll and cause problems.
+		 */
+		timestamp += lastdec + TIMER_LOAD_VAL - now;
+	}
+	lastdec = now;
+
+	return timestamp;
+}
+
+/* waits specified delay value and resets timestamp */
+void udelay_masked (unsigned long usec)
+{
+	ulong tmo;
+	ulong endtime;
+	signed long diff;
+
+	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
+		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
+		tmo *= CONFIG_SYS_HZ;		/* find number of "ticks" to wait to achieve target */
+		tmo /= 1000;		/* finish normalize. */
+	} else {			/* else small number, don't kill it prior to HZ multiply */
+		tmo = usec * CONFIG_SYS_HZ;
+		tmo /= (1000*1000);
+	}
+
+	endtime = get_timer_masked () + tmo;
+
+	do {
+		ulong now = get_timer_masked ();
+		diff = endtime - now;
+	} while (diff >= 0);
+}
+
+/*
+ * This function is derived from PowerPC code (read timebase as long long).
+ * On ARM it just returns the timer value.
+ */
+unsigned long long get_ticks(void)
+{
+	return get_timer(0);
+}
+
+/*
+ * This function is derived from PowerPC code (timebase clock frequency).
+ * On ARM it returns the number of timer ticks per second.
+ */
+ulong get_tbclk (void)
+{
+	ulong tbclk;
+
+	tbclk = CONFIG_SYS_HZ;
+	return tbclk;
+}
+
