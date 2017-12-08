--- linux-2.6.28.10-pristine/arch/arm/include/asm/mach/time.h	2009-05-02 14:54:43.000000000 -0400
+++ linux-2.6.28.10/arch/arm/include/asm/mach/time.h	2009-10-07 10:16:37.000000000 -0400
@@ -1,5 +1,5 @@
 /*
- * arch/arm/include/asm/mach/time.h
+ * linux/include/asm-arm/mach/time.h
  *
  * Copyright (C) 2004 MontaVista Software, Inc.
  *
@@ -38,20 +38,37 @@
 	void			(*init)(void);
 	void			(*suspend)(void);
 	void			(*resume)(void);
-#ifndef CONFIG_GENERIC_TIME
 	unsigned long		(*offset)(void);
+
+#ifdef CONFIG_NO_IDLE_HZ
+	struct dyn_tick_timer	*dyn_tick;
 #endif
 };
 
+#ifdef CONFIG_NO_IDLE_HZ
+
+#define DYN_TICK_ENABLED	(1 << 1)
+
+struct dyn_tick_timer {
+	unsigned int	state;			/* Current state */
+	int		(*enable)(void);	/* Enables dynamic tick */
+	int		(*disable)(void);	/* Disables dynamic tick */
+	void		(*reprogram)(unsigned long); /* Reprograms the timer */
+	int		(*handler)(int, void *, struct pt_regs *);
+};
+
+void timer_dyn_reprogram(void);
+#else
+#define timer_dyn_reprogram()	do { } while (0)
+#endif
+
 extern struct sys_timer *system_timer;
-extern void timer_tick(void);
+extern void timer_tick(void);
 
 /*
  * Kernel time keeping support.
  */
-struct timespec;
 extern int (*set_rtc)(void);
-extern void save_time_delta(struct timespec *delta, struct timespec *rtc);
 extern void restore_time_delta(struct timespec *delta, struct timespec *rtc);
 
 #endif
