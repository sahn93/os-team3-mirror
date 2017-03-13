/* linux/drivers/video/decon_display/decon_mipi_dsi.h
 *
 * Header file for Samsung MIPI-DSI common driver.
 *
 * Copyright (c) 2013 Samsung Electronics
 * Haowei Li <haowei.li@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#ifndef _S5P_MIPI_DSI_H
#define _S5P_MIPI_DSI_H

#include <linux/device.h>
#include <linux/fb.h>
#include <linux/notifier.h>
#include <linux/kernel.h>

#include <linux/regulator/consumer.h>

#include "decon_display_driver.h"

#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif

#define to_dsim_plat(d)		(to_platform_device(d)->dev.platform_data)

struct dphy_timing_value {
	u32 bps;
	u32 clk_prepare;
	u32 clk_zero;
	u32 clk_post;
	u32 clk_trail;
	u32 hs_prepare;
	u32 hs_zero;
	u32 hs_trail;
	u32 lpx;
	u32 hs_exit;
	u32 b_dphyctl;
};

enum mipi_dsim_interface_type {
	DSIM_COMMAND,
	DSIM_VIDEO
};

enum mipi_dsim_virtual_ch_no {
	DSIM_VIRTUAL_CH_0,
	DSIM_VIRTUAL_CH_1,
	DSIM_VIRTUAL_CH_2,
	DSIM_VIRTUAL_CH_3
};

enum mipi_dsim_burst_mode_type {
	DSIM_NON_BURST_SYNC_EVENT,
	DSIM_NON_BURST_SYNC_PULSE = 2,
	DSIM_BURST = 1,
	DSIM_NON_VIDEO_MODE = 4
};

enum mipi_dsim_no_of_data_lane {
	DSIM_DATA_LANE_1,
	DSIM_DATA_LANE_2,
	DSIM_DATA_LANE_3,
	DSIM_DATA_LANE_4
};

enum mipi_dsim_byte_clk_src {
	DSIM_PLL_OUT_DIV8,
	DSIM_EXT_CLK_DIV8,
	DSIM_EXT_CLK_BYPASS
};

enum mipi_dsim_pixel_format {
	DSIM_CMD_3BPP,
	DSIM_CMD_8BPP,
	DSIM_CMD_12BPP,
	DSIM_CMD_16BPP,
	DSIM_VID_16BPP_565,
	DSIM_VID_18BPP_666PACKED,
	DSIM_18BPP_666LOOSELYPACKED,
	DSIM_24BPP_888
};

#ifdef CONFIG_DECON_MIPI_DSI_PKTGO
enum mipi_dsim_pktgo_state {
	DSIM_PKTGO_DISABLED,
	DSIM_PKTGO_STANDBY,
	DSIM_PKTGO_ENABLED
};
#endif

/**
 * struct mipi_dsim_config - interface for configuring mipi-dsi controller.
 *
 * @auto_flush: enable or disable Auto flush of MD FIFO using VSYNC pulse.
 * @eot_disable: enable or disable EoT packet in HS mode.
 * @auto_vertical_cnt: specifies auto vertical count mode.
 *	in Video mode, the vertical line transition uses line counter
 *	configured by VSA, VBP, and Vertical resolution.
 *	If this bit is set to '1', the line counter does not use VSA and VBP
 *	registers.(in command mode, this variable is ignored)
 * @hse: set horizontal sync event mode.
 *	In VSYNC pulse and Vporch area, MIPI DSI master transfers only HSYNC
 *	start packet to MIPI DSI slave at MIPI DSI spec1.1r02.
 *	this bit transfers HSYNC end packet in VSYNC pulse and Vporch area
 *	(in mommand mode, this variable is ignored)
 * @hfp: specifies HFP disable mode.
 *	if this variable is set, DSI master ignores HFP area in VIDEO mode.
 *	(in command mode, this variable is ignored)
 * @hbp: specifies HBP disable mode.
 *	if this variable is set, DSI master ignores HBP area in VIDEO mode.
 *	(in command mode, this variable is ignored)
 * @hsa: specifies HSA disable mode.
 *	if this variable is set, DSI master ignores HSA area in VIDEO mode.
 *	(in command mode, this variable is ignored)
 * @e_interface: specifies interface to be used.(CPU or RGB interface)
 * @e_virtual_ch: specifies virtual channel number that main or
 *	sub diaplsy uses.
 * @e_pixel_format: specifies pixel stream format for main or sub display.
 * @e_burst_mode: selects Burst mode in Video mode.
 *	in Non-burst mode, RGB data area is filled with RGB data and NULL
 *	packets, according to input bandwidth of RGB interface.
 *	In Burst mode, RGB data area is filled with RGB data only.
 * @e_no_data_lane: specifies data lane count to be used by Master.
 * @e_byte_clk: select byte clock source. (it must be DSIM_PLL_OUT_DIV8)
 *	DSIM_EXT_CLK_DIV8 and DSIM_EXT_CLK_BYPASSS are not supported.
 * @pll_stable_time: specifies the PLL Timer for stability of the ganerated
 *	clock(System clock cycle base)
 *	if the timer value goes to 0x00000000, the clock stable bit of
status
 *	and interrupt register is set.
 * @esc_clk: specifies escape clock frequency for getting the escape clock
 *	prescaler value.
 * @stop_holding_cnt: specifies the interval value between transmitting
 *	read packet(or write "set_tear_on" command) and BTA request.
 *	after transmitting read packet or write "set_tear_on" command,
 *	BTA requests to D-PHY automatically. this counter value specifies
 *	the interval between them.
 * @bta_timeout: specifies the timer for BTA.
 *	this register specifies time out from BTA request to change
 *	the direction with respect to Tx escape clock.
 * @rx_timeout: specifies the timer for LP Rx mode timeout.
 *	this register specifies time out on how long RxValid deasserts,
 *	after RxLpdt asserts with respect to Tx escape clock.
 *	- RxValid specifies Rx data valid indicator.
 *	- RxLpdt specifies an indicator that D-PHY is under RxLpdt mode.
 *	- RxValid and RxLpdt specifies signal from D-PHY.
 * @lcd_panel_info: pointer for lcd panel specific structure.
 *	this structure specifies width, height, timing and polarity and so
on.
 * @mipi_ddi_pd: pointer to lcd panel platform data.
 */
struct mipi_dsim_config {
	unsigned char auto_flush;
	unsigned char eot_disable;

	unsigned char auto_vertical_cnt;
	unsigned char hse;
	unsigned char hfp;
	unsigned char hbp;
	unsigned char hsa;

	enum mipi_dsim_interface_type	e_interface;
	enum mipi_dsim_virtual_ch_no	e_virtual_ch;
	enum mipi_dsim_pixel_format	e_pixel_format;
	enum mipi_dsim_burst_mode_type	e_burst_mode;
	enum mipi_dsim_no_of_data_lane	e_no_data_lane;
	enum mipi_dsim_byte_clk_src	e_byte_clk;

	unsigned int p;
	unsigned int m;
	unsigned int s;

	unsigned int pll_stable_time;
	unsigned long esc_clk;

	unsigned short stop_holding_cnt;
	unsigned char bta_timeout;
	unsigned short rx_timeout;

	void *lcd_panel_info;
	void *dsim_ddi_pd;
};

/* for RGB Interface */
struct mipi_dsi_lcd_timing {
	int	left_margin;
	int	right_margin;
	int	upper_margin;
	int	lower_margin;
	int	stable_vfp;
	int	hsync_len;
	int	vsync_len;
	int	cmd_allow;
};

/* for CPU Interface */
struct mipi_dsi_cpu_timing {
	unsigned int	cs_setup;
	unsigned int	wr_setup;
	unsigned int	wr_act;
	unsigned int	wr_hold;
};

struct mipi_dsi_lcd_size {
	unsigned int	width;
	unsigned int	height;
};

struct mipi_dsim_lcd_config {
	enum mipi_dsim_interface_type e_interface;
	unsigned int parameter[3];

	/* lcd panel info */
	struct	mipi_dsi_lcd_timing rgb_timing;
	struct	mipi_dsi_cpu_timing cpu_timing;
	struct	mipi_dsi_lcd_size lcd_size;
	/* platform data for lcd panel based on MIPI-DSI. */
	void *mipi_ddi_pd;
};

/**
 * struct mipi_dsim_device - global interface for mipi-dsi driver.
 *
 * @dev: driver model representation of the device.
 * @clock: pointer to MIPI-DSI clock of clock framework.
 * @irq: interrupt number to MIPI-DSI controller.
 * @reg_base: base address to memory mapped SRF of MIPI-DSI controller.
 *	(virtual address)
 * @pd: pointer to MIPI-DSI driver platform data.
 * @dsim_info: infomation for configuring mipi-dsi controller.
 * @master_ops: callbacks to mipi-dsi operations.
 * @lcd_info: pointer to mipi_lcd_info structure.
 * @state: specifies status of MIPI-DSI controller.
 *	the status could be RESET, INIT, STOP, HSCLKEN and ULPS.
 * @data_lane: specifiec enabled data lane number.
 *	this variable would be set by driver according to e_no_data_lane
 *	automatically.
 * @e_clk_src: select byte clock source.
 *	this variable would be set by driver according to e_byte_clock
 *	automatically.
 * @hs_clk: HS clock rate.
 *	this variable would be set by driver automatically.
 * @byte_clk: Byte clock rate.
 *	this variable would be set by driver automatically.
 * @escape_clk: ESCAPE clock rate.
 *	this variable would be set by driver automatically.
 * @freq_band: indicates Bitclk frequency band for D-PHY global timing.
 *	Serial Clock(=ByteClk X 8)		FreqBand[3:0]
 *		~ 99.99 MHz				0000
 *		100 ~ 119.99 MHz			0001
 *		120 ~ 159.99 MHz			0010
 *		160 ~ 199.99 MHz			0011
 *		200 ~ 239.99 MHz			0100
 *		140 ~ 319.99 MHz			0101
 *		320 ~ 389.99 MHz			0110
 *		390 ~ 449.99 MHz			0111
 *		450 ~ 509.99 MHz			1000
 *		510 ~ 559.99 MHz			1001
 *		560 ~ 639.99 MHz			1010
 *		640 ~ 689.99 MHz			1011
 *		690 ~ 769.99 MHz			1100
 *		770 ~ 869.99 MHz			1101
 *		870 ~ 949.99 MHz			1110
 *		950 ~ 1000 MHz				1111
 *	this variable would be calculated by driver automatically.
 */
struct mipi_dsim_device {
	struct device *dev;
	struct resource *res;
	struct clk *clock;
	unsigned int irq;
	void __iomem *reg_base;

	struct s5p_platform_mipi_dsim *pd;
	struct mipi_dsim_config *dsim_config;

	unsigned int state;
	unsigned int data_lane;
	enum mipi_dsim_byte_clk_src e_clk_src;
	unsigned long hs_clk;
	unsigned long byte_clk;
	unsigned long escape_clk;
	unsigned char freq_band;
	unsigned char id;
	struct notifier_block fb_notif;

	struct mipi_dsim_lcd_driver	*dsim_lcd_drv;
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend	early_suspend;
#endif
	struct lcd_device	*lcd;
	unsigned int enabled;
	struct decon_lcd	*lcd_info;
	struct dphy_timing_value	timing;
#ifdef CONFIG_DECON_MIPI_DSI_PKTGO
	int				pktgo;
#endif
	spinlock_t slock;
	struct mutex lock;
};

/**
 * struct s5p_platform_mipi_dsim - interface to platform data
 *	for mipi-dsi driver.
 *
 * @mipi_dsim_config: pointer of structure for configuring mipi-dsi controller.
 * @dsim_lcd_info: pointer to structure for configuring
 *	mipi-dsi based lcd panel.
 * @mipi_power: callback pointer for enabling or disabling mipi power.
 * @init_d_phy: callback pointer for enabing d_phy of dsi master.
 * @get_fb_frame_done: callback pointer for getting frame done status of
the
 *	display controller(FIMD).
 * @trigger: callback pointer for triggering display controller(FIMD)
 *	in case of CPU mode.
 */
struct s5p_platform_mipi_dsim {
	const char	clk_name[16];

	struct mipi_dsim_config *dsim_config;
	struct mipi_dsim_lcd_config *dsim_lcd_config;

	int (*mipi_power) (struct mipi_dsim_device *dsim, unsigned int
		enable);
	int (*init_d_phy) (struct mipi_dsim_device *dsim, unsigned int enable);
	int (*get_fb_frame_done) (struct fb_info *info);
	void (*trigger) (struct fb_info *info);
};

/**
 * driver structure for mipi-dsi based lcd panel.
 *
 * this structure should be registered by lcd panel driver.
 * mipi-dsi driver seeks lcd panel registered through name field
 * and calls these callback functions in appropriate time.
 */

struct mipi_dsim_lcd_driver {
	int	(*probe)(struct mipi_dsim_device *dsim);
	int	(*suspend)(struct mipi_dsim_device *dsim);
	int	(*displayon)(struct mipi_dsim_device *dsim);
	int	(*resume)(struct mipi_dsim_device *dsim);
};

/**
 * register mipi_dsim_lcd_driver object defined by lcd panel driver
 * to mipi-dsi driver.
 */
extern int s5p_dsim_init_d_phy(struct mipi_dsim_device *dsim,
	unsigned int enable);
extern void s5p_dsim0_set_platdata(struct s5p_platform_mipi_dsim *pd);
extern void s5p_dsim1_set_platdata(struct s5p_platform_mipi_dsim *pd);
/* MIPI-DSIM status types. */
enum {
	DSIM_STATE_INIT,	/* should be initialized. */
	DSIM_STATE_STOP,	/* CPU and LCDC are LP mode. */
	DSIM_STATE_HSCLKEN,	/* HS clock was enabled. */
	DSIM_STATE_ULPS,
	DSIM_STATE_SUSPEND	/* DSIM is suspend state */
};

/* define DSI lane types. */
enum {
	DSIM_LANE_CLOCK	= (1 << 0),
	DSIM_LANE_DATA0	= (1 << 1),
	DSIM_LANE_DATA1	= (1 << 2),
	DSIM_LANE_DATA2	= (1 << 3),
	DSIM_LANE_DATA3	= (1 << 4),
};

#define FIN_HZ			(24 * MHZ)

#define DFIN_PLL_MIN_HZ		(6 * MHZ)
#define DFIN_PLL_MAX_HZ		(12 * MHZ)

#define DFVCO_MIN_HZ		(500 * MHZ)
#define DFVCO_MAX_HZ		(1000 * MHZ)

#define TRY_GET_FIFO_TIMEOUT	(5000 * 2)

/* DSIM Interrupt Sources */
#define SFR_PL_FIFO_EMPTY	(1 << 29)
#define SFR_PH_FIFO_EMPTY	(1 << 28)
#define MIPI_FRAME_DONE		(1 << 24)
#define RX_DAT_DONE		(1 << 18)
#define ERR_RX_ECC		(1 << 15)

#define DSIM_RX_FIFO_READ_DONE	(0x30800002)
#define DSIM_MAX_RX_FIFO	(64)
int s5p_mipi_dsi_wr_data(struct mipi_dsim_device *dsim, unsigned int
	data_id, unsigned int data0, unsigned int data1);
int s5p_mipi_dsi_rd_data(struct mipi_dsim_device *dsim, u32 data_id,
	u32 addr, u32 count, u8 *buf, u8 rxfifo_done);
#ifdef CONFIG_DECON_MIPI_DSI_PKTGO
void s5p_mipi_dsi_te_triggered(void);
void s5p_mipi_dsi_trigger_unmask(void);
#endif

#if defined(CONFIG_LCD_MIPI_S6E8AB0)
extern struct mipi_dsim_lcd_driver s6e8ab0_mipi_lcd_driver;
#elif defined(CONFIG_LCD_MIPI_S6E8AA0)
extern struct mipi_dsim_lcd_driver s6e8aa0_mipi_lcd_driver;
#elif defined(CONFIG_DECON_LCD_S6E8AA0)
extern struct mipi_dsim_lcd_driver s6e8aa0_mipi_lcd_driver;
#elif defined(CONFIG_LCD_MIPI_S6E63M0)
extern struct mipi_dsim_lcd_driver s6e63m0_mipi_lcd_driver;
#elif defined(CONFIG_LCD_MIPI_TC358764)
extern struct mipi_dsim_lcd_driver tc358764_mipi_lcd_driver;
#elif defined(CONFIG_LCD_MIPI_HYDISWUXGA)
extern struct mipi_dsim_lcd_driver hydiswuxga_mipi_lcd_driver;
#elif defined(CONFIG_DECON_LCD_S6E3FA0)
extern struct mipi_dsim_lcd_driver s6e3fa0_mipi_lcd_driver;
#elif defined(CONFIG_DECON_LCD_S6E3HA0)
extern struct mipi_dsim_lcd_driver s6e3ha0_mipi_lcd_driver;
#elif defined(CONFIG_DECON_LCD_S6E3HA0K)
extern struct mipi_dsim_lcd_driver s6e3ha0k_mipi_lcd_driver;
#elif defined(CONFIG_DECON_LCD_S6E3FA2)
extern struct mipi_dsim_lcd_driver s6e3fa2_mipi_lcd_driver;
#endif


enum mipi_ddi_interface {
	RGB_IF = 0x4000,
	I80_IF = 0x8000,
	YUV_601 = 0x10000,
	YUV_656 = 0x20000,
	MIPI_VIDEO = 0x1000,
	MIPI_COMMAND = 0x2000,
};

enum mipi_ddi_panel_select {
	DDI_MAIN_LCD = 0,
	DDI_SUB_LCD = 1,
};

enum mipi_ddi_model {
	S6DR117 = 0,
};

enum mipi_ddi_parameter {
	/* DSIM video interface parameter */
	DSI_VIRTUAL_CH_ID = 0,
	DSI_FORMAT = 1,
	DSI_VIDEO_MODE_SEL = 2,
};
#endif /* _S5P_MIPI_DSI_H */
