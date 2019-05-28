--- uboot/examples/memtest/read.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.exam/examples/memtest/read.c	2014-07-28 17:23:30.965408920 -0400
@@ -0,0 +1,199 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+  MG9090 Diagnostics 
+
+  Author	: Samvinesh Christopher
+
+  Module	: Console Functions to read a string. Also uses 
+			  convert module from RexDebug to read signed and 
+			  unsigned long integers from console.
+
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+ *****************************************************************/
+
+#include <common.h>
+#include <exports.h>
+#include "types.h"
+extern BOOL Str2Unsigned(char *Str,UINT32 *Value);
+extern BOOL  Str2Signed(char *Str,SINT32 *Value);
+size_t
+strlen(const char *str)
+{
+	int i=0;
+	while (*str++)
+		i++;
+	return i;
+}
+
+
+#define CFG_CB_SIZE   64
+
+static char my_console_buffer[CFG_CB_SIZE];
+static char erase_seq[] = "\b \b";		/* erase sequence	*/
+static char tab_seq[] 	= "        ";		/* used to expand TABs	*/
+
+static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen);
+/****************************************************************************/
+char * 
+readstr (const char *const prompt)
+{
+	char   *p = my_console_buffer;
+	int	n = 0;				/* buffer index		*/
+	int plen=0;
+	int col = 0;			/* output column cnt    */
+	char	c;
+
+
+	if (prompt) {
+		plen = strlen(prompt);
+		puts(prompt);
+	}
+	col = plen;
+
+
+	for (;;) 
+	{
+		c = getc();
+
+		/* Special character handling */
+		switch (c) 
+		{
+		case '\r':				/* Enter		*/
+		case '\n':
+			*p = '\0';
+			puts ("\r\n");
+			return (my_console_buffer);
+
+		case '\0':				/* nul			*/
+			continue;
+
+		case 0x03:				/* ^C - break		*/
+			my_console_buffer[0] = '\0';	/* discard input */
+			return (my_console_buffer);
+
+		case 0x15:				/* ^U - erase line	*/
+			while (col > plen) 
+			{
+				puts (erase_seq);
+				--col;
+			}
+			p = my_console_buffer;
+			n = 0;
+			continue;
+
+		case 0x17:				/* ^W - erase word 	*/
+			p=delete_char(my_console_buffer, p, &col, &n, plen);
+			while ((n > 0) && (*p != ' ')) 
+			{
+				p=delete_char(my_console_buffer, p, &col, &n, plen);
+			}
+			continue;
+
+		case 0x08:				/* ^H  - backspace	*/
+		case 0x7F:				/* DEL - backspace	*/
+			p=delete_char(my_console_buffer, p, &col, &n, plen);
+			continue;
+
+		default:
+			/*
+			 * Must be a normal character then
+			 */
+			if (n < CFG_CB_SIZE-2) 
+			{
+				if (c == '\t') 
+				{	/* expand TABs		*/
+					puts (tab_seq+(col&07));
+					col += 8 - (col&07);
+				}
+				 else 
+				{
+					++col;		/* echo input		*/
+					putc (c);
+				}
+				*p++ = c;
+				++n;
+			} 
+			else 
+			{			/* Buffer full		*/
+				putc ('\a');
+			}
+		}
+	}
+}
+
+/****************************************************************************/
+
+static char * delete_char (char *buffer, char *p, int *colp, int *np, int plen)
+{
+	char *s;
+
+	if (*np == 0) {
+		return (p);
+	}
+
+	if (*(--p) == '\t') {			/* will retype the whole line	*/
+		while (*colp > plen) {
+			puts (erase_seq);
+			(*colp)--;
+		}
+		for (s=buffer; s<p; ++s) {
+			if (*s == '\t') {
+				puts (tab_seq+((*colp) & 07));
+				*colp += 8 - ((*colp) & 07);
+			} else {
+				++(*colp);
+				putc (*s);
+			}
+		}
+	} else {
+		puts (erase_seq);
+		(*colp)--;
+	}
+	(*np)--;
+	return (p);
+}
+
+
+
+UINT32
+GetUnsigned(const char *prompt)
+{
+	char *str;
+	UINT32 Value;
+
+	str = readstr(prompt);
+	if (str == NULL)
+		return 0;
+	if (Str2Unsigned(str,&Value) == FALSE)
+		return 0;
+	return Value;	
+}
+
+INT32
+GetSigned(const char *prompt)
+{
+	char *str;
+	INT32 Value;
+
+	str = readstr(prompt);
+	if (str == NULL)
+		return 0;
+	if (Str2Signed(str,&Value) == FALSE)
+		return 0;
+	return Value;	
+}
