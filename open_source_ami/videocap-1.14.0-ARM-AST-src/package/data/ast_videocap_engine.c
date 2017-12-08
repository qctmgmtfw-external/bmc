/****************************************************************
 ****************************************************************
 **                                                            **
 **    (C)Copyright 2006-2009, American Megatrends Inc.        **
 **                                                            **
 **            All Rights Reserved.                            **
 **                                                            **
 **        5555 Oakbrook Pkwy Suite 200, Norcross              **
 **                                                            **
 **        Georgia - 30093, USA. Phone-(770)-246-8600.         **
 **                                                            **
 ****************************************************************
 ****************************************************************/

#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/delay.h>
#include <asm/io.h>
#include <asm/cacheflush.h>

#include "ast_videocap_ioctl.h"
#include "ast_videocap_data.h"
#include "ast_videocap_functions.h"

static unsigned char EncodeKeys[256];

static void *prev_cursor_pattern_addr;
static uint32_t prev_cursor_status;
static uint32_t prev_cursor_position;
static uint16_t prev_cursor_pos_x;
static uint16_t prev_cursor_pos_y;
static uint32_t prev_cursor_checksum;

static uint16_t cursor_pos_x;
static uint16_t cursor_pos_y;

static int ReInitVideoEngine = 0;
static int ISRDetectedModeOutOfLock = 1;

unsigned int TimeoutCount = 0;
unsigned int ModeDetTimeouts = 0;
unsigned int CaptureTimeouts = 0;
unsigned int CompressionTimeouts = 0;
unsigned int SpuriousTimeouts = 0;

struct ast_videocap_engine_info_t ast_videocap_engine_info;

static int ast_videocap_data_in_old_video_buf;

static int ModeDetectionIntrRecd;
static int CaptureIntrRecd;
static int CompressionIntrRecd;

/* Status variables for help in debugging  */
int WaitingForModeDetection;
int WaitingForCapture;
int WaitingForCompression;

static void *vga_mem_virt_addr;

/*  Timer related variables for Video Engine */
#define VE_TIMEOUT          1   /* in Seconds   */
unsigned long TimeToWait;
wait_queue_head_t mdwq;
wait_queue_head_t capwq;
wait_queue_head_t compwq;

/* Defines for Video Interrupt Control Register */
#define IRQ_WATCHDOG_OUT_OF_LOCK     (1<<0)
#define IRQ_CAPTURE_COMPLETE         (1<<1)
#define IRQ_COMPRESSION_PACKET_READY (1<<2)
#define IRQ_COMPRESSION_COMPLETE     (1<<3)
#define IRQ_MODE_DETECTION_READY     (1<<4)
#define IRQ_FRAME_COMPLETE           (1<<5)
#define ALL_IRQ_ENABLE_BITS (IRQ_WATCHDOG_OUT_OF_LOCK |  \
		IRQ_CAPTURE_COMPLETE | \
		IRQ_COMPRESSION_PACKET_READY | \
		IRQ_COMPRESSION_COMPLETE | \
		IRQ_MODE_DETECTION_READY | \
		IRQ_FRAME_COMPLETE)

/* Defines for Video Interrupt Status Register */
#define STATUS_WATCHDOG_OUT_OF_LOCK     (1<<0)
#define STATUS_CAPTURE_COMPLETE         (1<<1)
#define STATUS_COMPRESSION_PACKET_READY (1<<2)
#define STATUS_COMPRESSION_COMPLETE     (1<<3)
#define STATUS_MODE_DETECTION_READY     (1<<4)
#define STATUS_FRAME_COMPLETE           (1<<5)
#define ALL_IRQ_STATUS_BITS (STATUS_WATCHDOG_OUT_OF_LOCK |  \
		STATUS_CAPTURE_COMPLETE | \
		STATUS_COMPRESSION_PACKET_READY | \
		STATUS_COMPRESSION_COMPLETE | \
		STATUS_MODE_DETECTION_READY | \
		STATUS_FRAME_COMPLETE)

#define MAX_SYNC_CHECK_COUNT        (20)

INTERNAL_MODE Internal_Mode[] = {
	// 1024x768
	{1024, 768, 0, 65},
	{1024, 768, 1, 65},
	{1024, 768, 2, 75},
	{1024, 768, 3, 79},
	{1024, 768, 4, 95},

	// 1280x1024
	{1280, 1024, 0, 108},
	{1280, 1024, 1, 108},
	{1280, 1024, 2, 135},
	{1280, 1024, 3, 158},

	// 1600x1200
	{1600, 1200, 0, 162},
	{1600, 1200, 1, 162},
	{1600, 1200, 2, 176},
	{1600, 1200, 3, 189},
	{1600, 1200, 4, 203},
	{1600, 1200, 5, 230},

	// 1920x1200 reduce blank
	{1920, 1200, 0, 157},
	{1920, 1200, 1, 157},
};

static const unsigned int InternalModeTableSize = (sizeof(Internal_Mode) / sizeof(INTERNAL_MODE));

inline uint32_t ast_videocap_read_reg(uint32_t reg)
{
	return ioread32(ast_videocap_reg_virt_base + reg);
}

inline void ast_videocap_write_reg(uint32_t data, uint32_t reg)
{
	iowrite32(data, ast_videocap_reg_virt_base + reg);
}

inline void ast_videocap_set_reg_bits(uint32_t reg, uint32_t bits)
{
	uint32_t tmp;

	tmp = ast_videocap_read_reg(reg);
	tmp |= bits;
	ast_videocap_write_reg(tmp, reg);
}

inline void ast_videocap_clear_reg_bits(uint32_t reg, uint32_t bits)
{
	uint32_t tmp;

	tmp = ast_videocap_read_reg(reg);
	tmp &= ~bits;
	ast_videocap_write_reg(tmp, reg);
}

int ast_videocap_remap_vga_mem(void)
{
	uint32_t reg;
	unsigned int total_mem_size;
	unsigned int vga_mem_size;

	/* Total memory size
		AST2050 / AST2150: MMC Configuration Register[3:2]
			00: 32 MB
			01: 64 MB
			10: 128 MB
			11: 256 MB

		AST2300: MMC Configuration Register[1:0]
			00: 64 MB
			01: 128 MB
			10: 256 MB
			11: 512 MB
	*/
	reg = ioread32(SDRAM_CONFIG_REG);
	#ifdef SOC_AST2300
	total_mem_size = 0x4000000 << (reg & 0x03);
	#else
	total_mem_size = 0x2000000 << ((reg >> 2) & 0x03);
	#endif

	/* VGA memory size
		Hardware Strapping Register[3:2] definition:
		VGA Aperture Memory Size Setting:
		    00: 8 MB
		    01: 16 MB
		    10: 32 MB
		    11: 64 MB
	*/
	reg = ioread32(SCU_HW_STRAPPING_REG);
	vga_mem_size = 0x800000 << ((reg >> 2) & 0x03);

	vga_mem_virt_addr = ioremap(total_mem_size - vga_mem_size, vga_mem_size);
	if (vga_mem_virt_addr == NULL) {
		return -1;
	}

	return 0;
}

void ast_videocap_unmap_vga_mem(void)
{
	iounmap(vga_mem_virt_addr);
}

#define SCU_MISC_CTRL			0x2C
#define SCU_MISC_CTRL_DAC_DISABLE		0x00000008 /* bit 3 */
static int ast_Videocap_vga_dac_status(void)
{
	return (ioread32(AST_SCU_VA_BASE + SCU_MISC_CTRL) & SCU_MISC_CTRL_DAC_DISABLE) ? 0 : 1;
}

static void ast_videocap_vga_dac_ctrl(int enable)
{
	uint32_t reg;

	iowrite32(0x1688A8A8, SCU_KEY_CONTROL_REG); /* unlock SCU */

	reg = ioread32(AST_SCU_VA_BASE + SCU_MISC_CTRL);
	if (enable) {
		reg &= ~SCU_MISC_CTRL_DAC_DISABLE;
	} else {
		reg |= SCU_MISC_CTRL_DAC_DISABLE;
	}
	iowrite32(reg, AST_SCU_VA_BASE + SCU_MISC_CTRL);

	iowrite32(0, SCU_KEY_CONTROL_REG); /* lock SCU */
}

void ast_videocap_engine_data_init(void)
{
	ModeDetectionIntrRecd = 0;
	CaptureIntrRecd = 0;
	CompressionIntrRecd = 0;

	WaitingForModeDetection = 0;
	WaitingForCapture = 0;
	WaitingForCompression = 0;

	ast_videocap_data_in_old_video_buf = 0;

	prev_cursor_pattern_addr = NULL;
	prev_cursor_status = 0;
	prev_cursor_position = 0;
	prev_cursor_pos_x = 0;
	prev_cursor_pos_y = 0;
	prev_cursor_checksum = 0;

	memset((void *) &ast_videocap_engine_info, 0, sizeof(struct ast_videocap_engine_info_t));
	ast_videocap_engine_info.FrameHeader.CompressionMode = COMPRESSION_MODE;
	ast_videocap_engine_info.FrameHeader.JPEGTableSelector = JPEG_TABLE_SELECTOR;
	ast_videocap_engine_info.FrameHeader.AdvanceTableSelector = ADVANCE_TABLE_SELECTOR;
}

irqreturn_t ast_videocap_irq_handler(int irq, void *dev_id)
{
	uint32_t status;

	status = ast_videocap_read_reg(AST_VIDEOCAP_ISR);
	ast_videocap_write_reg(status, AST_VIDEOCAP_ISR); // clear interrupts flags

	/* Mode Detection Watchdog Interrupt */
	if (status & STATUS_WATCHDOG_OUT_OF_LOCK) {
		ISRDetectedModeOutOfLock = 1;
	}

	/* Mode Detection Ready */
	if (status & STATUS_MODE_DETECTION_READY) {
		ModeDetectionIntrRecd = 1;
		wake_up_interruptible(&mdwq);
	}

	/* Capture Engine Idle */
	if (status & STATUS_CAPTURE_COMPLETE) {
		CaptureIntrRecd = 1;
		wake_up_interruptible(&capwq);
	}

	/* Compression Engine Idle */
	if (status & STATUS_COMPRESSION_COMPLETE) {
		CompressionIntrRecd = 1;
		wake_up_interruptible(&compwq);
	}

	return IRQ_HANDLED;
}

void ast_videocap_reset_hw(void)
{
	uint32_t reg;

	iowrite32(0x1688A8A8, SCU_KEY_CONTROL_REG); /* unlock SCU */

	/* enable reset video engine */
	reg = ioread32(SCU_SYS_RESET_REG);
	reg |= 0x00000040;
	iowrite32(reg, SCU_SYS_RESET_REG);

	udelay(100);

	/* enable video engine clock */
	reg = ioread32(SCU_CLK_STOP_REG);
	reg &= ~(0x0000002B);
	iowrite32(reg, SCU_CLK_STOP_REG);

	wmb();

	mdelay(10);

	/* disable reset video engine */
	reg = ioread32(SCU_SYS_RESET_REG);
	reg &= ~(0x00000040);
	iowrite32(reg, SCU_SYS_RESET_REG);

	#ifdef SOC_AST2300
	/* support wide screen resolution */
	reg = ioread32(SCU_SOC_SCRATCH1_REG);
	reg |= (0x00000001);
	iowrite32(reg, SCU_SOC_SCRATCH1_REG);
	#endif

	iowrite32(0, SCU_KEY_CONTROL_REG); /* lock SCU */
}


void ast_videocap_hw_init(void)
{
	uint32_t reg;

	iowrite32(0x1688A8A8, SCU_KEY_CONTROL_REG); /* unlock SCU */

	/* enable reset video engine */
	reg = ioread32(SCU_SYS_RESET_REG);
	reg |= 0x00000040;
	iowrite32(reg, SCU_SYS_RESET_REG);

	udelay(100);

	/* enable video engine clock */
	reg = ioread32(SCU_CLK_STOP_REG);
	reg &= ~(0x0000002B);
	iowrite32(reg, SCU_CLK_STOP_REG);

	wmb();

	mdelay(10);

	/* disable reset video engine */
	reg = ioread32(SCU_SYS_RESET_REG);
	reg &= ~(0x00000040);
	iowrite32(reg, SCU_SYS_RESET_REG);

	iowrite32(0, SCU_KEY_CONTROL_REG); /* lock SCU */

	/* unlock video engine */
	udelay(100);
	ast_videocap_write_reg(AST_VIDEOCAP_KEY_MAGIC, AST_VIDEOCAP_KEY);

	/* clear interrupt status */
	ast_videocap_write_reg(0xFFFFFFFF, AST_VIDEOCAP_ISR);
	ast_videocap_write_reg(ALL_IRQ_STATUS_BITS, AST_VIDEOCAP_ISR);

	/* enable interrupt */
	reg = IRQ_WATCHDOG_OUT_OF_LOCK | IRQ_CAPTURE_COMPLETE | IRQ_COMPRESSION_COMPLETE | IRQ_MODE_DETECTION_READY;
	ast_videocap_write_reg(reg, AST_VIDEOCAP_ICR);

	/* ??? */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL2, 0x000000C0);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL2, 0x00000080);

	ast_videocap_write_reg(0, AST_VIDEOCAP_COMPRESS_STREAM_BUF_READ_OFFSET);

	/* reset RC4 */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL2, 0x00000100);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL2, 0x00000100);
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL2, 0x00000100);

	//ast_videocap_vga_dac_ctrl(1);

	/* clear flag buffer */
	memset(AST_VIDEOCAP_FLAG_BUF_ADDR, 0, AST_VIDEOCAP_FLAG_BUF_SZ);
	ast_videocap_data_in_old_video_buf = 0;
}

void StopVideoCapture(void)
{
}

void ProcessTimeOuts(unsigned char Type)
{
	static unsigned long ConsecutiveTimeouts = 0;

	if (Type == NO_TIMEOUT) {
		ConsecutiveTimeouts = 0;
		return;
	}

	if (Type == SPURIOUS_TIMEOUT) {
		SpuriousTimeouts ++;
		return;
	}

	TimeoutCount ++;
	ast_videocap_data_in_old_video_buf = 0;

	if (Type == MODE_DET_TIMEOUT) {
		ModeDetTimeouts ++;
		return;
	}

	if (Type == CAPTURE_TIMEOUT) {
		CaptureTimeouts ++;
		ReInitVideoEngine = 1;
		return;
	}

	if (Type == COMPRESSION_TIMEOUT) {
		CompressionTimeouts ++;
		ReInitVideoEngine = 1;
		return;
	}
}

/* return 0 on succeeds, -1 on times out */
int ast_videocap_trigger_mode_detection(void)
{
	init_waitqueue_head(&mdwq);
	ModeDetectionIntrRecd = 0;
	TimeToWait = VE_TIMEOUT * HZ;

	/* trigger mode detection by rising edge(0 -> 1) */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT);
	wmb();
	udelay(100);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT);

	if (wait_event_interruptible_timeout(mdwq, ModeDetectionIntrRecd, TimeToWait) != 0) {
		WaitingForModeDetection = 0;
	} else { /* timeout */
		if (ModeDetectionIntrRecd == 0) {
		    ProcessTimeOuts(MODE_DET_TIMEOUT);
		    return -1; /* Possibly out of lock or invalid input, we should send a blank screen */
		} else {
		    /* ModeDetectionIntrRecd is set, but timeout triggered - should never happen!! */
		    ProcessTimeOuts(SPURIOUS_TIMEOUT);
		    /* Not much to do here, just continue on */
		}
	}

	return 0;
}

/* Return 0 if Capture Engine becomes idle, -1 if engine remains busy */
int ast_videocap_trigger_capture(void)
{
	init_waitqueue_head(&capwq);
	TimeToWait = VE_TIMEOUT*HZ;
	CaptureIntrRecd = 0;

	if (!(ast_videocap_read_reg(AST_VIDEOCAP_SEQ_CTRL) & AST_VIDEOCAP_SEQ_CTRL_CAP_STATUS)) {
		return -1; /* capture engine remains busy */
	}

	/* trigger capture */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS_IDLE);
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_CAP);
	udelay(100);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_CAP);

	WaitingForCapture = 1;
	if (wait_event_interruptible_timeout(capwq, CaptureIntrRecd, TimeToWait) != 0) {
		WaitingForCapture = 0;
	} else {
		/* Did Video Engine timeout? */
		if (CaptureIntrRecd == 0) {
			ProcessTimeOuts(CAPTURE_TIMEOUT);
			return -1; /* Capture Engine remains busy */
		} else {
			/* CaptureIntrRecd is set, but timeout triggered - should never happen!! */
			ProcessTimeOuts(SPURIOUS_TIMEOUT);
			/* Not much to do here, just continue on */
		}
	}

	return 0;
}

/* Return 0 if Compression becomes idle, -1 if engine remains busy */
int ast_videocap_trigger_compression(void)
{
	init_waitqueue_head(&compwq);
	TimeToWait = VE_TIMEOUT*HZ;
	CompressionIntrRecd = 0;

	if (!(ast_videocap_read_reg(AST_VIDEOCAP_SEQ_CTRL) & AST_VIDEOCAP_SEQ_CTRL_COMPRESS_STATUS)) {
		return -1; /* compression engine remains busy */
	}

	/* trigger compression */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS_IDLE);
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_CAP);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS);
	udelay(100);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_COMPRESS);

	WaitingForCompression = 1;
	if (wait_event_interruptible_timeout(compwq, CompressionIntrRecd, TimeToWait) != 0) {
		WaitingForCompression = 0;
	} else {
		/* Did Video Engine timeout? */
		if (CompressionIntrRecd == 0) {
			ProcessTimeOuts(COMPRESSION_TIMEOUT);
			return -1; /* Compression remains busy */
		} else {
			/* CompressionIntrRecd is set, but timeout triggered - should never happen!! */
			ProcessTimeOuts(SPURIOUS_TIMEOUT);
			/* Not much to do here, just continue on */
		}
	}

	return 0;
}

void ast_videocap_determine_syncs(unsigned long *hp, unsigned long *vp)
{
	int i = 0;
	int hPcount = 0;
	int vPcount = 0;

	*vp = 0;
	*hp = 0;

	for (i = 0; i < MAX_SYNC_CHECK_COUNT; i ++) {
		if (ast_videocap_read_reg(AST_VIDEOCAP_MODE_DETECT_STATUS) & 0x20000000) // Hsync polarity is positive
		    hPcount ++;
		if (ast_videocap_read_reg(AST_VIDEOCAP_MODE_DETECT_STATUS) & 0x10000000) // Vsync polarity is positive
		    vPcount ++;
	}

	/* Majority readings indicate positive? Then it must be positive.*/
	if (hPcount > (MAX_SYNC_CHECK_COUNT / 2)) {
		*hp = 1;
	}

	if (vPcount > (MAX_SYNC_CHECK_COUNT / 2)) {
		*vp = 1;
	}
}

int ast_videocap_auto_position_adjust(struct ast_videocap_engine_info_t *info)
{
	uint32_t reg;
	unsigned long H_Start, H_End, V_Start, V_End, i, H_Temp = 0, V_Temp = 0;
	unsigned long Mode_PixelClock = 0, mode_bandwidth, refresh_rate_index, color_depth_index;
	unsigned long color_depth, mode_clock;

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0x0000FF00);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0x65 << 8);

Redo:
	ast_videocap_set_reg_bits(AST_VIDEOCAP_ISR, 0x00000010);
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT);

	if (ast_videocap_trigger_mode_detection() < 0) {
		/*  Possibly invalid input, we should send a blank screen   */
		ISRDetectedModeOutOfLock = 1;
		return -1;
	}

	H_Start = ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_H) & AST_VIDEOCAP_EDGE_DETECT_START_MASK;
	H_End = (ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_H) & AST_VIDEOCAP_EDGE_DETECT_END_MASK) >> AST_VIDEOCAP_EDGE_DETECT_END_SHIFT;
	V_Start = ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_V) & AST_VIDEOCAP_EDGE_DETECT_START_MASK;
	V_End = (ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_V) & AST_VIDEOCAP_EDGE_DETECT_END_MASK) >> AST_VIDEOCAP_EDGE_DETECT_END_SHIFT;

	//Check if cable quality is too bad. If it is bad then we use 0x65 as threshold
	//Because RGB data is arrived slower than H-sync, V-sync. We have to read more times to confirm RGB data is arrived
	if ((abs(H_Temp - H_Start) > 1) || ((H_Start <= 1) || (V_Start <= 1) || (H_Start == 0x3FF) || (V_Start == 0x3FF))) {
		H_Temp = ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_H) & AST_VIDEOCAP_EDGE_DETECT_START_MASK;
		V_Temp = ast_videocap_read_reg(AST_VIDEOCAP_EDGE_DETECT_V) & AST_VIDEOCAP_EDGE_DETECT_START_MASK;
		//printk("REDO\n");
		//printk("Data = %x\n", ReadMMIOLong (MODE_DETECTION_REGISTER));
		//printk("H_Start = %x, H_End = %x\n", H_Start, H_End);
		//printk("V_Start = %x, V_End = %x\n", V_Start, V_End);
		goto Redo;
	}

	//printk("H_Start = %lx, H_End = %lx\n", H_Start, H_End);
	//printk("V_Start = %lx, V_End = %lx\n", V_Start, V_End);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_TIMING_GEN_H, AST_VIDEOCAP_TIMING_GEN_LAST_PIXEL_MASK);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_TIMING_GEN_H, H_End);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_TIMING_GEN_H, AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_MASK);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_TIMING_GEN_H, (H_Start - 1) << AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_SHIFT);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_TIMING_GEN_V, AST_VIDEOCAP_TIMING_GEN_LAST_PIXEL_MASK);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_TIMING_GEN_V, V_End + 1);

	ast_videocap_clear_reg_bits(AST_VIDEOCAP_TIMING_GEN_V, AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_MASK);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_TIMING_GEN_V, (V_Start) << AST_VIDEOCAP_TIMING_GEN_FIRST_PIXEL_SHIFT);

	info->src_mode.x = (H_End - H_Start) + 1;
	info->src_mode.y = (V_End - V_Start) + 1;
	info->dest_mode.x = info->src_mode.x;
	info->dest_mode.y = info->src_mode.y;

	printk("Mode Detected: %d x %d\n", info->src_mode.x, info->src_mode.y);

	//  Judge if bandwidth is not enough then enable direct mode in internal VGA

	reg = ast_videocap_read_reg(AST_VIDEOCAP_VGA_SCRATCH_9093);
	if (((reg & 0xff00) >> 8) == 0xA8) { // Driver supports to get display information in scratch register
		color_depth = (reg & 0xff0000) >> 16; //VGA's Color Depth is 0 when real color depth is less than 8
		mode_clock = (reg & 0xff000000) >> 24;
		if (color_depth == 0) {
			printk("Color Depth is not 15 bpp or higher\n");
			info->INFData.DirectMode = 0;
	    } else {
			mode_bandwidth = (mode_clock * (color_depth + 32)) / 8; //Video uses 32bits
			if (info->MemoryBandwidth < mode_bandwidth) {
				printk("Band Width is not enough\n");
				info->INFData.DirectMode = 1;
			} else {
				printk("Band Width is enough\n");
				info->INFData.DirectMode = 0;
			}
		}
	} else { /* old driver, get display information from table */
		refresh_rate_index = (ast_videocap_read_reg(AST_VIDEOCAP_VGA_SCRATCH_8C8F) >> 8) & 0x0F;
		color_depth_index = (ast_videocap_read_reg(AST_VIDEOCAP_VGA_SCRATCH_8C8F) >> 4) & 0x0F;
		if ((color_depth_index == 2) || (color_depth_index == 3) || (color_depth_index == 4)) { // 15 bpp /16 bpp / 32 bpp
			for (i = 0; i < InternalModeTableSize; i ++) { /* traverse table to find mode */
				if ((info->src_mode.x == Internal_Mode[i].HorizontalActive) &&
					(info->src_mode.y == Internal_Mode[i].VerticalActive) &&
					(refresh_rate_index == Internal_Mode[i].RefreshRateIndex)) {
					Mode_PixelClock = Internal_Mode[i].PixelClock;
					//printk("Mode_PixelClock = %ld\n", Mode_PixelClock);
				}
			}

			if (i == InternalModeTableSize) {
				printk("videocap: No match mode\n");
			}

			//  Calculate bandwidth required for this mode
			//  Video requires pixelclock * 4, VGA requires pixelclock * bpp / 8
			mode_bandwidth = Mode_PixelClock * (4 + 2 * (color_depth_index - 2));
			//printk("mode_bandwidth = %ld\n", mode_bandwidth);
			if (info->MemoryBandwidth < mode_bandwidth) {
				printk("Band Width is not enough\n");
				info->INFData.DirectMode = 1;
			} else {
				printk("Band Width is enough\n");
				info->INFData.DirectMode = 0;
			}
		} else {
			printk("Color Depth is not 15bpp or higher\n");
			info->INFData.DirectMode = 0;
		}
	}

	return 0;
}

int ast_videocap_mode_detection(struct ast_videocap_engine_info_t *info, bool in_video_capture)
{
	unsigned long HPolarity, VPolarity;
	uint32_t vga_status;

	ast_videocap_hw_init();

	/* Check if internal VGA screen is off */
	vga_status = ast_videocap_read_reg(AST_VIDEOCAP_VGA_STATUS) >> 24;
	if ((vga_status & 0xC0) || (vga_status & 0x04) || (!(vga_status & 0x3B))) {
		info->NoSignal = 1;
	} else {
		info->NoSignal = 0;
	}

	/* video source from internal VGA */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_SRC);

	/* Set VideoClockPhase Selection for Internal Video */
	//ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, 0x00000A00);

	/* polarity is same as source */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_HSYNC_POLARITY | AST_VIDEOCAP_CTRL_VSYNC_POLARITY);

	/* Set Stable Parameters */
	/*  VSync Stable Max*/
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0x000F0000);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, MODEDETECTION_VERTICAL_STABLE_MAXIMUM << 16);

	/*  HSync Stable Max*/
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0x00F00000);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, MODEDETECTION_HORIZONTAL_STABLE_MAXIMUM << 20);

	/*  VSync counter threshold */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0x0F000000);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, MODEDETECTION_VERTICAL_STABLE_THRESHOLD << 24);

	/*  HSync counter threshold */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, 0xF0000000);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_MODE_DETECT_PARAM, MODEDETECTION_HORIZONTAL_STABLE_THRESHOLD << 28);

	/* Disable Watchdog */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_WDT | AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT);

	/* New Mode Detection procedure prescribed by ASPEED:
	 *
	 * Trigger mode detection once
	 * After that succeeds, figure out the polarities of HSync and VSync
	 * Program the right polarities...
	 * Then Trigger mode detection again
	 * Now with the new values, use JudgeMode to figure out the correct mode
	 */

	/* Trigger first detection, and check for timeout? */
	if (ast_videocap_trigger_mode_detection() < 0) {
		/*  Possibly invalid input, we should send a blank screen   */
		ISRDetectedModeOutOfLock = 1;
		return -1;
	}

	/* Now, determine sync polarities and program appropriately */
	ast_videocap_determine_syncs(&HPolarity, &VPolarity);

	if (info->INFData.DirectMode == 1) {
		/* Direct mode always uses positive/positive so we inverse them */
		ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_HSYNC_POLARITY | AST_VIDEOCAP_CTRL_VSYNC_POLARITY);
	} else {
		ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, (VPolarity << 1) | HPolarity);
	}

	/* Disable Watchdog */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_WDT | AST_VIDEOCAP_SEQ_CTRL_MODE_DETECT);

	/* Trigger second detection, and check for timeout? */
	if (ast_videocap_trigger_mode_detection() < 0) {
		/*  Possibly invalid input, we should send a blank screen */
		ISRDetectedModeOutOfLock = 1;
		return -1;
	}

	vga_status = ast_videocap_read_reg(AST_VIDEOCAP_VGA_STATUS) >> 24;
	if ((vga_status & 0xC0) || (vga_status & 0x04) || (!(vga_status & 0x3B))) {
		info->NoSignal = 1;
		udelay(100);
	} else {
		info->NoSignal = 0;
	}

	/* Enable Watchdog detection for next time */
	ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_WDT);

	ISRDetectedModeOutOfLock = 0;

    if (in_video_capture == 1)
    {
		ast_videocap_auto_position_adjust(info);
    }
    else if (info->NoSignal == 0) {
		ast_videocap_auto_position_adjust(info);
	}

	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_BIT_NUM); /* 18 bits for single edge only */

	return 0;
}

// Set defaults for user settings
void InitDefaultSettings(struct ast_videocap_engine_info_t *info)
{
	info->FrameHeader.JPEGYUVTableMapping = JPEG_YUVTABLE_MAPPING;
	info->FrameHeader.JPEGScaleFactor = JPEG_SCALE_FACTOR;
	info->FrameHeader.AdvanceScaleFactor = ADVANCE_SCALE_FACTOR;
	info->FrameHeader.SharpModeSelection = SHARP_MODE_SELECTION;
	info->INFData.DownScalingMethod = DOWN_SCALING_METHOD;
	info->INFData.DifferentialSetting = DIFF_SETTING;
	info->FrameHeader.RC4Enable = RC4_ENABLE;
	info->FrameHeader.RC4Reset = RC4_RESET;
	info->INFData.AnalogDifferentialThreshold = ANALOG_DIFF_THRESHOLD;
	info->INFData.DigitalDifferentialThreshold = DIGITAL_DIFF_THRESHOLD;
	info->INFData.ExternalSignalEnable = EXTERNAL_SIGNAL_ENABLE;
	info->INFData.AutoMode = AUTO_MODE;

	memcpy(EncodeKeys, ENCODE_KEYS, ENCODE_KEYS_LEN);
}

int ast_videocap_get_engine_config(struct ast_videocap_engine_info_t *info, struct ast_videocap_engine_config_t *config, unsigned long *size)
{
	config->differential_setting = info->INFData.DifferentialSetting;
	config->dct_quant_quality = info->FrameHeader.JPEGScaleFactor;
	config->dct_quant_tbl_select = info->FrameHeader.JPEGTableSelector;
	config->sharp_mode_selection = info->FrameHeader.SharpModeSelection;
	config->sharp_quant_quality = info->FrameHeader.AdvanceScaleFactor;
	config->sharp_quant_tbl_select = info->FrameHeader.AdvanceTableSelector;
	config->compression_mode = info->FrameHeader.CompressionMode;
	config->vga_dac = ast_Videocap_vga_dac_status();
	*size = sizeof(struct ast_videocap_engine_config_t);
	return 0;
}

int ast_videocap_set_engine_config(struct ast_videocap_engine_info_t *info, struct ast_videocap_engine_config_t *config)
{
	ast_videocap_data_in_old_video_buf = 0; /* configurations are changed, send a full screen next time */

	info->INFData.DifferentialSetting = config->differential_setting;
	info->FrameHeader.JPEGScaleFactor = config->dct_quant_quality;
	info->FrameHeader.JPEGTableSelector = config->dct_quant_tbl_select;
	info->FrameHeader.SharpModeSelection = config->sharp_mode_selection;
	info->FrameHeader.AdvanceScaleFactor = config->sharp_quant_quality;
	info->FrameHeader.AdvanceTableSelector = config->sharp_quant_tbl_select;
	info->FrameHeader.CompressionMode = config->compression_mode;

	if (config->vga_dac == 0) { /* turn off VGA output */
		ast_videocap_vga_dac_ctrl(0);
	} else { /* turn on VGA output */
		ast_videocap_vga_dac_ctrl(1);
	}

	return 0;
}

int ast_videocap_enable_video_dac(struct ast_videocap_engine_info_t *info, struct ast_videocap_engine_config_t *config)
{
	ast_videocap_data_in_old_video_buf = 0; /* configurations are changed, send a full screen next time */
	info->INFData.DifferentialSetting = config->differential_setting;
	info->FrameHeader.JPEGScaleFactor = config->dct_quant_quality;
	info->FrameHeader.JPEGTableSelector = config->dct_quant_tbl_select;
	info->FrameHeader.SharpModeSelection = config->sharp_mode_selection;
	info->FrameHeader.AdvanceScaleFactor = config->sharp_quant_quality;
	info->FrameHeader.AdvanceTableSelector = config->sharp_quant_tbl_select;
	info->FrameHeader.CompressionMode = config->compression_mode;
    config->vga_dac=1;
	ast_videocap_vga_dac_ctrl(1);
	return 0;
}

void ast_videocap_get_mem_bandwidth(struct ast_videocap_engine_info_t *info)
{
	uint32_t reg;
	unsigned long Numerator, Denumerator, PostDivider, OutputDivider, Buswidth = 16, MemoryClock;

	reg = ioread32(SCU_M_PLL_PARAM_REG);

	/* We discard M-PLL bypass and Turn-Off M-PLL mode */
	Numerator = ((reg >> 5) & 0x3F);
	Denumerator = (reg & 0xF);
	OutputDivider = ((reg >> 4) & 0x1);
	switch ((reg >> 12) & 0x7) {
	case 0x04: /* 100 */
		PostDivider = 2;
		break;
	case 0x05: /* 101 */
		PostDivider = 4;
		break;
	case 0x06: /* 110 */
		PostDivider = 8;
		break;
	case 0x07: /* 111 */
		PostDivider = 16;
		break;
	default:
		PostDivider = 1;
		break;
	}

	MemoryClock = (24 * (2 - OutputDivider) * (Numerator + 2) / (Denumerator + 1)) / PostDivider;
	//printk("Memory Clock = %ld\n", MemoryClock);

	#ifdef SOC_AST2300
	info->MemoryBandwidth = ((MemoryClock * 2 * Buswidth * 4) / 10) / 8;
	#else
	info->MemoryBandwidth = ((MemoryClock * 2 * Buswidth * 6) / 10) / 8;
	#endif
	//printk("Memory Bandwidth = %d\n", info->MemoryBandwidth);
}

int StartVideoCapture(struct ast_videocap_engine_info_t *info)
{
	unsigned long DRAM_Data;

	InitDefaultSettings(info);
	prev_cursor_status = 0;
	prev_cursor_pattern_addr = NULL;

	// For first screen, whole frame should be reported as changed
	ast_videocap_write_reg(virt_to_phys(AST_VIDEOCAP_REF_BUF_ADDR), AST_VIDEOCAP_SRC_BUF2_ADDR);
	ast_videocap_write_reg(virt_to_phys(AST_VIDEOCAP_IN_BUF_ADDR), AST_VIDEOCAP_SRC_BUF1_ADDR);
	ast_videocap_write_reg(virt_to_phys(AST_VIDEOCAP_FLAG_BUF_ADDR), AST_VIDEOCAP_BCD_FLAG_BUF_ADDR);
	ast_videocap_write_reg(virt_to_phys(AST_VIDEOCAP_COMPRESS_BUF_ADDR), AST_VIDEOCAP_COMPRESSED_BUF_ADDR);

	/* reset compression control & disable RC4 encryption */
	ast_videocap_write_reg(0x00080000, AST_VIDEOCAP_COMPRESS_CTRL);

	DRAM_Data = *(volatile unsigned long *) SDRAM_CONFIG_REG;
	switch ((DRAM_Data >> 2) & 0x03) {
	case 0:
		info->TotalMemory = 32;
		break;
	case 1:
		info->TotalMemory = 64;
		break;
	case 2:
		info->TotalMemory = 128;
		break;
	case 3:
		info->TotalMemory = 256;
		break;
	}
	//printk("Total Memory = %ld MB\n", info->TotalMemory);

	switch ((DRAM_Data >> 4) & 0x03) {
	case 0:
		info->VGAMemory = 8;
		break;
	case 1:
		info->VGAMemory = 16;
		break;
	case 2:
		info->VGAMemory = 32;
		break;
	case 3:
		info->VGAMemory = 64;
		break;
	}
	//printk("VGA Memory = %ld MB\n", info->VGAMemory);

	ast_videocap_get_mem_bandwidth(info);

	ast_videocap_mode_detection(info,0);
	ast_videocap_data_in_old_video_buf = 0;

	return 0;
}

static void ast_videocap_horizontal_down_scaling(struct ast_videocap_engine_info_t *info)
{
	uint16_t factor;

	/* calculate scaling factor */
	/* D / S = 4096 / factor  => factor = (S / D) * 4096 */
	factor = (info->src_mode.x - 1) / (info->dest_mode.x - 1) * AST_VIDEOCAP_SCALING_FACTOR_MAX;

	/* We don't support up-scale */
	if (factor < AST_VIDEOCAP_SCALING_FACTOR_MAX) {
		factor = AST_VIDEOCAP_SCALING_FACTOR_MAX;
	}

	if ((factor * (info->dest_mode.x - 1)) != (info->src_mode.x - 1) * AST_VIDEOCAP_SCALING_FACTOR_MAX) {
		factor += 1;
	}

	//ast_videocap_clear_reg_bits(AST_VIDEOCAP_SCALING_FACTOR, AST_VIDEOCAP_SCALING_FACTOR_H_MASK);
	//ast_videocap_set_reg_bits(AST_VIDEOCAP_SCALING_FACTOR, factor << AST_VIDEOCAP_SCALING_FACTOR_H_SHIFT);
	ast_videocap_write_reg(0, AST_VIDEOCAP_SCALING_FACTOR);
	ast_videocap_write_reg(0x10001000, AST_VIDEOCAP_SCALING_FACTOR);
}

static void ast_videocap_vertical_down_scaling(struct ast_videocap_engine_info_t *info)
{
	uint16_t factor;

	/* calculate scaling factor */
	/* D / S = 4096 / factor  => factor = (S / D) * 4096 */
	factor = (info->src_mode.y - 1) / (info->dest_mode.y - 1) * AST_VIDEOCAP_SCALING_FACTOR_MAX;

	/* We don't support up-scale */
	if (factor < AST_VIDEOCAP_SCALING_FACTOR_MAX) {
		factor = AST_VIDEOCAP_SCALING_FACTOR_MAX;
	}

	if ((factor * (info->dest_mode.y - 1)) != (info->src_mode.y - 1) * AST_VIDEOCAP_SCALING_FACTOR_MAX) {
		factor += 1;
	}

	//ast_videocap_clear_reg_bits(AST_VIDEOCAP_SCALING_FACTOR, AST_VIDEOCAP_SCALING_FACTOR_V_MASK);
	//ast_videocap_set_reg_bits(AST_VIDEOCAP_SCALING_FACTOR, factor << AST_VIDEOCAP_SCALING_FACTOR_V_SHIFT);
	ast_videocap_write_reg(0, AST_VIDEOCAP_SCALING_FACTOR);
	ast_videocap_write_reg(0x10001000, AST_VIDEOCAP_SCALING_FACTOR);
}

int ast_videocap_engine_init(struct ast_videocap_engine_info_t *info)
{
	uint32_t diff_setting;
	uint32_t compress_setting;
	unsigned long OldBufferAddress, NewBufferAddress;
	unsigned long buf_offset;
	unsigned long remainder;

	ast_videocap_write_reg(0, AST_VIDEOCAP_PRIMARY_CRC_PARAME);
	ast_videocap_write_reg(0, AST_VIDEOCAP_SECONDARY_CRC_PARAM);
	ast_videocap_write_reg(0, AST_VIDEOCAP_DATA_TRUNCATION); /* no reduction on RGB */

	/* Set to YUV444 mode */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_MASK);
	info->FrameHeader.Mode420 = 0;

	/* disable automatic compression */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_AUTO_COMPRESS);

	/* Internal always uses internal timing generation */
	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_DE_SIGNAL);

	/* Internal use inverse clock and software cursor */
	ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_CLK_DELAY_MASK);
	ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_INV_CLK_NO_DELAY);

	if (info->INFData.DirectMode) {
		ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF);

		ast_videocap_write_reg((info->TotalMemory - info->VGAMemory) << 20, AST_VIDEOCAP_DIRECT_FRAME_BUF_ADDR);
		ast_videocap_write_reg(info->src_mode.x * 4, AST_VIDEOCAP_DIRECT_FRAME_BUF_CTRL);

		/* always use auto mode since we can not get the correct setting when the VGA driver is not installed */
		ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_AUTO_MODE);
	} else {
		ast_videocap_clear_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_DIRECT_FETCH_FRAME_BUF);
		ast_videocap_set_reg_bits(AST_VIDEOCAP_CTRL, AST_VIDEOCAP_CTRL_HW_CSR_OVERLAY); /* without VGA hardware cursor overlay image */
	}

	/* set scaling filter parameters */
	if ((info->src_mode.x == info->dest_mode.x) && (info->src_mode.y == info->dest_mode.y)) {
		/* scaling factor == 1.0, that is, non-scaling */
		ast_videocap_write_reg(AST_VIDEOCAP_SCALING_FILTER_PARAM_ONE, AST_VIDEOCAP_SCALING_FILTER_PARAM0);
		ast_videocap_write_reg(AST_VIDEOCAP_SCALING_FILTER_PARAM_ONE, AST_VIDEOCAP_SCALING_FILTER_PARAM1);
		ast_videocap_write_reg(AST_VIDEOCAP_SCALING_FILTER_PARAM_ONE, AST_VIDEOCAP_SCALING_FILTER_PARAM2);
		ast_videocap_write_reg(AST_VIDEOCAP_SCALING_FILTER_PARAM_ONE, AST_VIDEOCAP_SCALING_FILTER_PARAM3);
	}

	/* set capture/compress window */
	ast_videocap_write_reg((info->src_mode.x << AST_VIDEOCAP_WINDOWS_H_SHIFT) | info->src_mode.y, AST_VIDEOCAP_CAPTURE_WINDOW);
	ast_videocap_write_reg((info->src_mode.x << AST_VIDEOCAP_WINDOWS_H_SHIFT) | info->src_mode.y, AST_VIDEOCAP_COMPRESS_WINDOW);

	/* set buffer offset based on detected mode */
	buf_offset = info->src_mode.x;
	remainder = buf_offset % 16;
	if (remainder != 0) {
		buf_offset += (16 - remainder);
	}
	ast_videocap_write_reg(buf_offset * 4, AST_VIDEOCAP_SCAN_LINE_OFFSET);

	/* set buffers addresses */
	if (ast_videocap_data_in_old_video_buf) {
		OldBufferAddress = ast_videocap_read_reg(AST_VIDEOCAP_SRC_BUF1_ADDR);
		NewBufferAddress = ast_videocap_read_reg(AST_VIDEOCAP_SRC_BUF2_ADDR);
		ast_videocap_write_reg(NewBufferAddress, AST_VIDEOCAP_SRC_BUF1_ADDR);
		ast_videocap_write_reg(OldBufferAddress, AST_VIDEOCAP_SRC_BUF2_ADDR);
		diff_setting = AST_VIDEOCAP_BCD_CTRL_ENABLE;
	} else {
		ast_videocap_write_reg((unsigned long) AST_VIDEOCAP_IN_BUF_ADDR, AST_VIDEOCAP_SRC_BUF1_ADDR);
		ast_videocap_write_reg((unsigned long) AST_VIDEOCAP_REF_BUF_ADDR, AST_VIDEOCAP_SRC_BUF2_ADDR);
		diff_setting = AST_VIDEOCAP_BCD_CTRL_DISABLE;
	}

	diff_setting |= (info->INFData.DigitalDifferentialThreshold << AST_VIDEOCAP_BCD_CTRL_TOLERANCE_SHIFT);
	ast_videocap_write_reg(diff_setting, AST_VIDEOCAP_BCD_CTRL);

	// Set this stream register even in frame mode
	ast_videocap_write_reg(0x0000001F, AST_VIDEOCAP_STREAM_BUF_SIZE);

	//  Configure JPEG parameters
	switch (info->FrameHeader.CompressionMode) {
	case COMP_MODE_YUV420:
		info->FrameHeader.Mode420 = 1;
		ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_MASK);
		ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_YUV420 << AST_VIDEOCAP_SEQ_CTRL_FORMAT_SHIFT);
		compress_setting = 1;
		break;
	case COMP_MODE_YUV444_JPG_ONLY:
		compress_setting = 1;
		break;
	case COMP_MODE_YUV444_2_CLR_VQ:
		info->INFData.VQMode = 2;
		compress_setting = 0;
		break;
	case COMP_MODE_YUV444_4_CLR_VQ:
		info->INFData.VQMode = 4;
		compress_setting = 2;
		break;
	default:
		printk("videocap: Default CompressionMode triggered!! %ld\n",
		    info->FrameHeader.CompressionMode);
		info->FrameHeader.Mode420 = 1;
		ast_videocap_clear_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_MASK);
		ast_videocap_set_reg_bits(AST_VIDEOCAP_SEQ_CTRL, AST_VIDEOCAP_SEQ_CTRL_FORMAT_YUV420 << AST_VIDEOCAP_SEQ_CTRL_FORMAT_SHIFT);
		compress_setting = 1;
		info->FrameHeader.CompressionMode = COMP_MODE_YUV420;
		break;
	}

	/* always use the same selector */
	compress_setting |= (info->FrameHeader.JPEGTableSelector) << 11;
	compress_setting |= (info->FrameHeader.JPEGTableSelector) << 6;
	/* if mapping is set, luminance and chrominance use the same table, else use respective table with same slector */
	if (!info->FrameHeader.JPEGYUVTableMapping)
		compress_setting |= 1 << 10;
	#ifdef SOC_AST2300
	if (!info->FrameHeader.Mode420) {
		compress_setting |= (info->FrameHeader.AdvanceTableSelector) << 27;
		compress_setting |= (info->FrameHeader.AdvanceTableSelector) << 22;
		if (!info->FrameHeader.JPEGYUVTableMapping)
			compress_setting |= 1 << 26;
		compress_setting |= 0x00010000;
	}
	#endif
	compress_setting |= 0x00080000;
	ast_videocap_write_reg(compress_setting, AST_VIDEOCAP_COMPRESS_CTRL);

	return 0;
}

void update_cursor_position(void)
{
	uint32_t cursor_position;

	cursor_position = ast_videocap_read_reg(AST_VIDEOCAP_HW_CURSOR_POSITION);
	if (cursor_position != prev_cursor_position) { /* position is changed */
		cursor_pos_x = (uint16_t) (cursor_position & 0x00000FFF);
		cursor_pos_y = (uint16_t) ((cursor_position >> 16) & 0x7FF);
		prev_cursor_position = cursor_position;
	}
}

int ast_videocap_create_cursor_packet(struct ast_videocap_engine_info_t *info, unsigned long *size)
{
	struct ast_videocap_cursor_info_t *cursor_info;
	void *cursor_pattern_addr;
	uint32_t cursor_status;
	uint32_t cursor_checksum;

	cursor_status = ast_videocap_read_reg(AST_VIDEOCAP_HW_CURSOR_STATUS);

	if ((cursor_status & AST_VIDEOCAP_HW_CURSOR_STATUS_DRIVER_MASK) != AST_VIDEOCAP_HW_CURSOR_STATUS_DRIVER_MGAIC) { /* VGA driver is not installed */
		//printk("VGA driver is not installed\n");
		return -1;
	}

	if (!(cursor_status & AST_VIDEOCAP_HW_CURSOR_STATUS_ENABLE)) { /* HW cursor is disabled */
		//printk("HW cursor is disabled\n");
		return -1;
	}

	update_cursor_position();
	cursor_pattern_addr = vga_mem_virt_addr + ast_videocap_read_reg(AST_VIDEOCAP_HW_CURSOR_PATTERN_ADDR);
	cursor_checksum = ioread32(cursor_pattern_addr + AST_VIDEOCAP_CURSOR_BITMAP_DATA); /* cursor checksum is behind the cursor pattern data */

	if ((cursor_status != prev_cursor_status) || /* type or offset changed */
		(cursor_pattern_addr != prev_cursor_pattern_addr) || /* pattern address changed */
		(cursor_checksum != prev_cursor_checksum) || /* pattern changed */
		(cursor_pos_x != prev_cursor_pos_x) || (cursor_pos_y != prev_cursor_pos_y)) { /* position changed */
		cursor_info = (struct ast_videocap_cursor_info_t *) (AST_VIDEOCAP_HDR_BUF_ADDR + AST_VIDEOCAP_HDR_BUF_VIDEO_SZ);
		cursor_info->type = (cursor_status & AST_VIDEOCAP_HW_CURSOR_STATUS_TYPE) ? AST_VIDEOCAP_HW_CURSOR_TYPE_COLOR : AST_VIDEOCAP_HW_CURSOR_TYPE_MONOCHROME;
		cursor_info->pos_x = cursor_pos_x;
		cursor_info->pos_y = cursor_pos_y;
		cursor_info->offset_x = (uint16_t) ((cursor_status >> 24) & 0x3F); /* bits 29:24 */
		cursor_info->offset_y = (uint16_t) ((cursor_status >> 16) & 0x3F); /* bits 21:16 */
		cursor_info->checksum = cursor_checksum;

		/* only send the cursor pattern to client when checksum is changed */
		if ((cursor_checksum != prev_cursor_checksum) || (cursor_pattern_addr != prev_cursor_pattern_addr)) { /* pattern is changed */
			dmac_inv_range(cursor_pattern_addr, cursor_pattern_addr + AST_VIDEOCAP_CURSOR_BITMAP_DATA);
			memcpy((void *) cursor_info->pattern, cursor_pattern_addr, AST_VIDEOCAP_CURSOR_BITMAP_DATA);
			prev_cursor_checksum = cursor_checksum;
			*size = sizeof(struct ast_videocap_cursor_info_t);
		} else {
			*size = sizeof(struct ast_videocap_cursor_info_t) - AST_VIDEOCAP_CURSOR_BITMAP_DATA;
		}

		/* save current status */
		prev_cursor_status = cursor_status;
		prev_cursor_pos_x = cursor_pos_x;
		prev_cursor_pos_y = cursor_pos_y;
		prev_cursor_pattern_addr = cursor_pattern_addr;
		return 0;
	} else { /* no change */
		return -1;
	}
}

int VideoCapture(struct ast_videocap_engine_info_t *info)
{
	if (ISRDetectedModeOutOfLock) {
		ast_videocap_data_in_old_video_buf = 0;
		if (ast_videocap_mode_detection(info,1) < 0) {
			return ASTCAP_IOCTL_BLANK_SCREEN;
		}
	}

	ast_videocap_engine_init(info);
	ast_videocap_horizontal_down_scaling(info);
	ast_videocap_vertical_down_scaling(info);

	if (ast_videocap_trigger_capture() < 0) {
		return ASTCAP_IOCTL_BLANK_SCREEN;
	}

	// If RC4 is required, here is where we enable it

	if (ast_videocap_trigger_compression() < 0) {
		return ASTCAP_IOCTL_BLANK_SCREEN;
	}

	ast_videocap_data_in_old_video_buf = 1;
	info->FrameHeader.NumberOfMB = ast_videocap_read_reg(AST_VIDEOCAP_COMPRESSED_BLOCK_COUNT) >> AST_VIDEOCAP_COMPRESSED_BLOCK_COUNT_SHIFT;
	info->CompressData.CompressSize = ast_videocap_read_reg(AST_VIDEOCAP_COMPRESSED_DATA_COUNT) * AST_VIDEOCAP_COMPRESSED_DATA_COUNT_UNIT;

	update_cursor_position();

	return (info->CompressData.CompressSize == 12) ? ASTCAP_IOCTL_NO_VIDEO_CHANGE : ASTCAP_IOCTL_SUCCESS;
}

int ast_videocap_create_video_packet(struct ast_videocap_engine_info_t *info, unsigned long *size)
{
	struct ast_videocap_video_hdr_t *video_hdr = NULL;
	unsigned long compressed_buf_size;
	unsigned long VideoCapStatus;

	/* Capture Video */
	VideoCapStatus = VideoCapture(info);
	if (VideoCapStatus != ASTCAP_IOCTL_SUCCESS) {
		*size = 0;
		return VideoCapStatus;
	}

	/* Check if Mode changed while capturing */
	if (ISRDetectedModeOutOfLock) {
		/* Send a blank screen this time */
		*size = 0;
		return ASTCAP_IOCTL_BLANK_SCREEN;
	}

	compressed_buf_size = info->CompressData.CompressSize;
	dmac_inv_range((void *) AST_VIDEOCAP_COMPRESS_BUF_ADDR, (void *) AST_VIDEOCAP_COMPRESS_BUF_ADDR + AST_VIDEOCAP_COMPRESS_BUF_SZ);

	/* fill AST video data header */
	video_hdr = (struct ast_videocap_video_hdr_t *) (AST_VIDEOCAP_HDR_BUF_ADDR);
	video_hdr->iEngVersion = 1;
	video_hdr->wHeaderLen = AST_VIDEOCAP_VIDEO_HEADER_SIZE;
	video_hdr->CompressData_CompressSize = compressed_buf_size;

	video_hdr->SourceMode_X = info->src_mode.x;
	video_hdr->SourceMode_Y = info->src_mode.y;
	video_hdr->DestinationMode_X = info->src_mode.x;
	video_hdr->DestinationMode_Y = info->src_mode.y;

	video_hdr->FrameHdr_RC4Enable = info->FrameHeader.RC4Enable;
	video_hdr->FrameHdr_JPEGScaleFactor = info->FrameHeader.JPEGScaleFactor;
	video_hdr->FrameHdr_Mode420 = info->FrameHeader.Mode420;
	video_hdr->FrameHdr_NumberOfMB = info->FrameHeader.NumberOfMB;
	video_hdr->FrameHdr_AdvanceScaleFactor = info->FrameHeader.AdvanceScaleFactor;
	video_hdr->FrameHdr_JPEGTableSelector = info->FrameHeader.JPEGTableSelector;
	video_hdr->FrameHdr_AdvanceTableSelector = info->FrameHeader.AdvanceTableSelector;
	video_hdr->FrameHdr_JPEGYUVTableMapping = info->FrameHeader.JPEGYUVTableMapping;
	video_hdr->FrameHdr_SharpModeSelection = info->FrameHeader.SharpModeSelection;
	video_hdr->FrameHdr_CompressionMode = info->FrameHeader.CompressionMode;
	video_hdr->FrameHdr_RC4Reset = info->FrameHeader.RC4Reset;

	video_hdr->InfData_DownScalingMethod = info->INFData.DownScalingMethod;
	video_hdr->InfData_DifferentialSetting = info->INFData.DifferentialSetting;
	video_hdr->InfData_AnalogDifferentialThreshold = info->INFData.AnalogDifferentialThreshold;
	video_hdr->InfData_DigitalDifferentialThreshold = info->INFData.DigitalDifferentialThreshold;
	video_hdr->InfData_AutoMode = info->INFData.AutoMode;
	video_hdr->InfData_VQMode = info->INFData.VQMode;

	video_hdr->Cursor_XPos = cursor_pos_x;
	video_hdr->Cursor_YPos = cursor_pos_y;

	*size = compressed_buf_size;
	return ASTCAP_IOCTL_SUCCESS;
}
