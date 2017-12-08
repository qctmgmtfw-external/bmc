--- .pristine/cipher_hw-1.10.0-ARM-AST-src/data/ast_cipher.c	Thu Jan  8 16:12:56 2015
+++ source/cipher_hw-1.10.0-ARM-AST-src/data/ast_cipher.c	Thu Jan  8 16:28:10 2015
@@ -224,14 +224,14 @@
 
     /* Reset HAC Engine */
     data = ioread32(SCU_SYS_RESET_REG);
-    data |= 0x0010;   /* BIT4=1, Reset HAC engine */      
+    data |= 0x00000010;   /* BIT4=1, Reset HAC engine */      
     iowrite32(data, SCU_SYS_RESET_REG);
     
     udelay(100);
         
     /* Stop YCLK(For HAC) */ 
     data = ioread32(SCU_CLK_STOP_REG);	     
-    data &= 0xfdfff;   /* BIT13=0, enable clock running */    
+    data &= 0xffffdfff;   /* BIT13=0, enable clock running */    
     iowrite32(data, SCU_CLK_STOP_REG);
     
     wmb();
@@ -239,7 +239,7 @@
     
     /* Reset HAC Engine */
     data = ioread32(SCU_SYS_RESET_REG);
-    data &= 0xfffef;   /* BIT4=0, No operation */      
+    data &= 0xffffffef;   /* BIT4=0, No operation */      
     iowrite32(data, SCU_SYS_RESET_REG);
     
     iowrite32(0, SCU_KEY_CONTROL_REG); /* lock SCU */
