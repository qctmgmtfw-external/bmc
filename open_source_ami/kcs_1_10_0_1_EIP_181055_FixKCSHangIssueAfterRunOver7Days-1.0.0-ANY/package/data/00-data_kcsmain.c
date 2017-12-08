--- .pristine/kcs-1.10.0-src/data/kcsmain.c Wed Jan 28 13:39:19 2015
+++ source/kcs-1.10.0-src/data/kcsmain.c Wed Jan 28 14:18:32 2015
@@ -127,7 +127,6 @@
 static void kcs_recv_byte (struct kcs_hal *pkcs_hal, unsigned char ch_num);
 
 static void set_kcs_state (struct kcs_hal *pkcs_hal, unsigned char ch_num, u8 state_value);
-static void set_obf_status (struct kcs_hal *pkcs_hal, unsigned char ch_num);
 
 
 extern unsigned int m_kcs_hw_test_enable;
@@ -700,7 +699,7 @@
 	pKCSBuffer->KCSSendPktIx++;
 	pdev->pkcs_hal->pkcs_hal_ops->write_kcs_data_out (pdev->ch_num, pKCSBuffer->pKCSSendPkt[0]);
 	//SA Set OBF Byte
-	set_obf_status (pdev->pkcs_hal, pdev->ch_num);
+	pdev->pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (pdev->ch_num);
 
 	/* From now onwords the data is sent from the IBF Interrupt handler */
 	return;
@@ -713,15 +712,6 @@
         pkcs_hal->pkcs_hal_ops->read_kcs_status (ch_num, &status_val);
         status_val = ((status_val & (~0xC0)) | (state_value));
         pkcs_hal->pkcs_hal_ops->write_kcs_status (ch_num, status_val);
-}
-
-static void 
-set_obf_status (struct kcs_hal *pkcs_hal, unsigned char ch_num)
-{
-        u8 status_val = 0;
-        pkcs_hal->pkcs_hal_ops->read_kcs_status (ch_num, &status_val);
-        status_val = status_val | 0x01 ;
-        pkcs_hal->pkcs_hal_ops->write_kcs_status (ch_num, status_val); 
 }
 
 int 
@@ -781,6 +771,8 @@
 				dbgprint ("KCS_WRITE_START\n");
 				/* Set the Index to 0 */
 				pKCSBuffer->KCSRcvPktIx = 0;
+                /* Clearing Abort flag at the start of transaction */
+                pKCSBuffer->PrevCmdAborted = 0;
 				/* Set the phase to WRITE */
 				pKCSBuffer->KCSPhase = KCS_PHASE_WRITE;
 				break;
@@ -793,7 +785,7 @@
 
 			case KCS_ABORT : 
 				dbgprint ("KCS_ABORT\n");
-				if (!pKCSBuffer->KcsIFActive) 
+				if (!pKCSBuffer->KcsIFActive && (!pKCSBuffer->FirstTime)) 
 				{ 
 					/* Set flag to avoid sending response*/ 
 					pKCSBuffer->PrevCmdAborted = 1; 
@@ -811,7 +803,7 @@
 				/* Send the dummy byte  */
 				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
 				//SA Set OBF Byte
-				set_obf_status (pkcs_hal, ch_num);
+				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 				break;
 
 			default :
@@ -898,7 +890,7 @@
 						set_kcs_state (pkcs_hal, ch_num, KCS_ERROR_STATE);
 						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
 						//SA Set OBF Byte
-						set_obf_status (pkcs_hal, ch_num);
+						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 						break;
 					}
 				}
@@ -908,7 +900,7 @@
 					pKCSBuffer->KCSPhase = KCS_PHASE_IDLE;
 					pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
 					//SA Set OBF Byte
-					set_obf_status (pkcs_hal, ch_num);
+					pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 
 					/* Set Transmission Complete */
 					pKCSBuffer->TxReady     = 1;
@@ -920,7 +912,7 @@
 				/* Transmit the next byte from the send buffer */
 				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, pKCSBuffer->pKCSSendPkt [pKCSBuffer->KCSSendPktIx]);
 				//SA Set OBF Byte
-				set_obf_status (pkcs_hal, ch_num);
+				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 				pKCSBuffer->KCSSendPktIx++;
 				break;
 
@@ -938,7 +930,7 @@
 						/* Write the error code to Data out register */
 						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, pKCSBuffer->KCSError);
 						//SA Set OBF Byte
-						set_obf_status (pkcs_hal, ch_num);
+						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 						/* Set the abort phase to be error2 */
 						pKCSBuffer->AbortPhase = ABORT_PHASE_ERROR2;
 
@@ -961,7 +953,7 @@
 						/* Send the dummy byte  */
 						pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
 						//SA Set OBF Byte
-						set_obf_status (pkcs_hal, ch_num);
+						pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
 
 				}
 				break;
@@ -973,7 +965,7 @@
 				set_kcs_state (pkcs_hal, ch_num, KCS_ERROR_STATE);
 				pkcs_hal->pkcs_hal_ops->write_kcs_data_out (ch_num, 0);
 				//SA Set OBF Byte
-				set_obf_status (pkcs_hal, ch_num);
+				pkcs_hal->pkcs_hal_ops->kcs_set_obf_status (ch_num);
         }
     }
 }
