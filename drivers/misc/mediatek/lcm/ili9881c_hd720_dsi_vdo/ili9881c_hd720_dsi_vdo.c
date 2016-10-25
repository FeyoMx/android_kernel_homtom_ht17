#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
#elif defined(BUILD_UBOOT)
    #include <asm/arch/mt_gpio.h>
#else
    #include <mt-plat/mt_gpio.h>
    #include <linux/gpio.h>	
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1280)
#define LCM_ID       (0x10)
#define REGFLAG_DELAY             							0XFFFE
#define REGFLAG_END_OF_TABLE      							0xFFFF   // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0
#define GPIO_DISP_BL_EN     86
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)    
       

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_sleep_out_setting[] = 
{
{0xFF,3,{0x98,0x81,0x03}},
{0x01,1,{0x08}},
{0x02,1,{0x00}},
{0x03,1,{0x73}},
{0x04,1,{0x73}},
{0x05,1,{0x14}},
{0x06,1,{0x06}},
{0x07,1,{0x02}},
{0x08,1,{0x05}},
{0x09,1,{0x14}},
{0x0A,1,{0x14}},
{0x0B,1,{0x00}},
{0x0C,1,{0x14}},
{0x0D,1,{0x14}},
{0x0E,1,{0x00}},
{0x0F,1,{0x0C}},
{0x10,1,{0x0C}},
{0x11,1,{0x0C}},
{0x12,1,{0x0C}},
{0x13,1,{0x14}},
{0x14,1,{0x0C}},
{0x15,1,{0x00}},
{0x16,1,{0x00}},
{0x17,1,{0x00}},
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1A,1,{0x00}},
{0x1B,1,{0x00}},
{0x1C,1,{0x00}},
{0x1D,1,{0x00}},
{0x1E,1,{0xC8}},
{0x1F,1,{0x80}},
{0x20,1,{0x02}},
{0x21,1,{0x00}},
{0x22,1,{0x02}},
{0x23,1,{0x00}},
{0x24,1,{0x00}},
{0x25,1,{0x00}},
{0x26,1,{0x00}},
{0x27,1,{0x00}},
{0x28,1,{0xFB}},
{0x29,1,{0x43}},
{0x2A,1,{0x00}},
{0x2B,1,{0x00}},
{0x2C,1,{0x07}},
{0x2D,1,{0x07}},
{0x2E,1,{0xFF}},
{0x2F,1,{0xFF}},
{0x30,1,{0x11}},
{0x31,1,{0x00}},
{0x32,1,{0x00}},
{0x33,1,{0x00}},
{0x34,1,{0x84}},
{0x35,1,{0x80}},
{0x36,1,{0x07}},
{0x37,1,{0x00}},
{0x38,1,{0x00}},
{0x39,1,{0x00}},
{0x3A,1,{0x00}},
{0x3B,1,{0x00}},
{0x3C,1,{0x00}},
{0x3D,1,{0x00}},
{0x3E,1,{0x00}},
{0x3F,1,{0x00}},
{0x40,1,{0x00}},
{0x41,1,{0x88}},
{0x42,1,{0x00}},
{0x43,1,{0x80}},
{0x44,1,{0x08}},
{0x50,1,{0x01}},
{0x51,1,{0x23}},
{0x52,1,{0x45}},
{0x53,1,{0x67}},
{0x54,1,{0x89}},
{0x55,1,{0xAB}},
{0x56,1,{0x01}},
{0x57,1,{0x23}},
{0x58,1,{0x45}},
{0x59,1,{0x67}},
{0x5A,1,{0x89}},
{0x5B,1,{0xAB}},
{0x5C,1,{0xCD}},
{0x5D,1,{0xEF}},
{0x5E,1,{0x10}},
{0x5F,1,{0x02}},
{0x60,1,{0x08}},
{0x61,1,{0x09}},
{0x62,1,{0x10}},
{0x63,1,{0x12}},
{0x64,1,{0x11}},
{0x65,1,{0x13}},
{0x66,1,{0x0C}},
{0x67,1,{0x02}},
{0x68,1,{0x02}},
{0x69,1,{0x02}},
{0x6A,1,{0x02}},
{0x6B,1,{0x02}},
{0x6C,1,{0x0E}},
{0x6D,1,{0x0D}},
{0x6E,1,{0x0F}},
{0x6F,1,{0x02}},
{0x70,1,{0x02}},
{0x71,1,{0x06}},
{0x72,1,{0x07}},
{0x73,1,{0x02}},
{0x74,1,{0x02}},
{0x75,1,{0x02}},
{0x76,1,{0x07}},
{0x77,1,{0x06}},
{0x78,1,{0x11}},
{0x79,1,{0x13}},
{0x7A,1,{0x10}},
{0x7B,1,{0x12}},
{0x7C,1,{0x0F}},
{0x7D,1,{0x02}},
{0x7E,1,{0x02}},
{0x7F,1,{0x02}},
{0x80,1,{0x02}},
{0x82,1,{0x0D}},
{0x83,1,{0x0E}},
{0x84,1,{0x0C}},
{0x85,1,{0x02}},
{0x86,1,{0x02}},
{0x87,1,{0x09}},
{0x88,1,{0x08}},
{0x89,1,{0x02}},
{0x8A,1,{0x02}},
{0xFF,3,{0x98,0x81,0x04}},
{0x6C,1,{0x15}},
{0x6E,1,{0x2A}},
{0x6F,1,{0x35}},
{0x3A,1,{0x24}},
{0x8D,1,{0x14}},
{0x87,1,{0xBA}},
{0x26,1,{0x76}},
{0xB2,1,{0xD1}},
{0xFF,3,{0x98,0x81,0x01}},
{0x22,1,{0x3A}},
{0x31,1,{0x00}},
{0x53,1,{0x4F}},
{0x55,1,{0x93}},
{0x50,1,{0x82}},
{0x51,1,{0x82}},
{0x60,1,{0x14}},
{0xA0,1,{0x08}},
{0xA1,1,{0x14}},
{0xA2,1,{0x1F}},
{0xA3,1,{0x0F}},
{0xA4,1,{0x11}},
{0xA5,1,{0x24}},
{0xA6,1,{0x16}},
{0xA7,1,{0x1A}},
{0xA8,1,{0x75}},
{0xA9,1,{0x1C}},
{0xAA,1,{0x28}},
{0xAB,1,{0x74}},
{0xAC,1,{0x1A}},
{0xAD,1,{0x18}},
{0xAE,1,{0x4C}},
{0xAF,1,{0x23}},
{0xB0,1,{0x27}},
{0xB1,1,{0x5A}},
{0xB2,1,{0x6A}},
{0xB3,1,{0x39}},
{0xC0,1,{0x08}},
{0xC1,1,{0x13}},
{0xC2,1,{0x1E}},
{0xC3,1,{0x0F}},
{0xC4,1,{0x10}},
{0xC5,1,{0x23}},
{0xC6,1,{0x18}},
{0xC7,1,{0x1A}},
{0xC8,1,{0x76}},
{0xC9,1,{0x1C}},
{0xCA,1,{0x28}},
{0xCB,1,{0x74}},
{0xCC,1,{0x1A}},
{0xCD,1,{0x1A}},
{0xCE,1,{0x4D}},
{0xCF,1,{0x23}},
{0xD0,1,{0x28}},
{0xD1,1,{0x5B}},
{0xD2,1,{0x69}},
{0xD3,1,{0x39}},
{0xFF,3,{0x98,0x81,0x00}},
{0x11,1,{0x00}},
{REGFLAG_DELAY, 120, {}},  
{0x29,1,{0x00}},	
{REGFLAG_DELAY, 20, {}},  
{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_in_setting[] = {
{0xFF, 3, {0x98, 0x81, 0x00}},
 // Display off sequence
 {0x28, 0, {0x00}},
 {REGFLAG_DELAY, 40, {}},
 // Sleep Mode On
 {0x10, 0, {0x00}},
 {REGFLAG_DELAY, 150, {}},
 {REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_backlight_level_setting[] = {	
{0xFF, 3, {0x98, 0x81, 0x00}},	
{0x51, 2, {0x0F,0xFF}},
{REGFLAG_END_OF_TABLE, 0x00, {}}

};
static void lcm_init_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(1);  //liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");	//liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void lcm_suspend_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(0);//liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerDown(MT6325_POWER_LDO_VGP1, "LCM_DRV");	//liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void lcm_resume_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
#ifdef BUILD_LK
	//mt6325_upmu_set_rg_vgp1_en(1);  //liuyi
#else
	printk("%s, begin\n", __func__);
	//hwPowerOn(MT6325_POWER_LDO_VGP1, VOL_DEFAULT, "LCM_DRV");	 //liuyi
	printk("%s, end\n", __func__);
#endif
#endif
}

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE; //BURST_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
            //The following defined the fomat for data coming from LCD engine.
            params->dsi.data_format.color_order     = LCM_COLOR_ORDER_RGB;
            params->dsi.data_format.trans_seq       = LCM_DSI_TRANS_SEQ_MSB_FIRST;
            params->dsi.data_format.padding         = LCM_DSI_PADDING_ON_LSB;
            params->dsi.data_format.format              = LCM_DSI_FORMAT_RGB888;

            // Highly depends on LCD driver capability.
                 params->dsi.packet_size=256;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				=15; // 6; //4   
		params->dsi.vertical_backporch				       = 20;  //14  
		params->dsi.vertical_frontporch				       = 30; //14;  //16  
		params->dsi.vertical_active_line				       = FRAME_HEIGHT;     
		params->dsi.horizontal_sync_active				= 40; // 60;   //4
		params->dsi.horizontal_backporch				= 140; //100;  //60  
		params->dsi.horizontal_frontporch				= 120; //100;    //60
		params->dsi.horizontal_blanking_pixel				= 60;   
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;  
		params->dsi.HS_TRAIL = 12;
		
		params->dsi.PLL_CLOCK = 216 ; //212;   //245
///////////////////////////////////////////////////////ESD			
		params->dsi.esd_check_enable = 1;
		params->dsi.customization_esd_check_enable = 1;
		params->dsi.lcm_esd_check_table[0].cmd 			= 0x0a;
		params->dsi.lcm_esd_check_table[0].count 		= 1;
		params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
		
		params->dsi.lcm_esd_check_table[1].cmd 			= 0x54;
		params->dsi.lcm_esd_check_table[1].count 		= 1;
		params->dsi.lcm_esd_check_table[1].para_list[0] = 0x2c;
		params->dsi.noncont_clock = 1;
		params->dsi.noncont_clock_period = 1;
///////////////////////////////////////////////////////			
		params->dsi.vertical_vfp_lp = 100;

}

#define GPIO_PCD_ID0  124 //62|0x80000000
static unsigned int lcm_compare_id(void)
{
    int pin_lcd_id0=0;	
    unsigned int lcd_id = 0;
    unsigned char buffer;
    unsigned int array[16];	
    pin_lcd_id0= mt_get_gpio_in(GPIO_PCD_ID0);
    if ( 0== pin_lcd_id0)
    {
        SET_RESET_PIN(1);  // NOTE : should reset LCM firstly
        SET_RESET_PIN(0);
        MDELAY(1);
        SET_RESET_PIN(1);
        MDELAY(10);        
        array[0] = 0x00013700;	/* read id return one byte*/
        dsi_set_cmdq(array, 1, 1);
        
        read_reg_v2(0xDA, &buffer, 1);
        lcd_id = buffer;		/* we only need ID */       
        #ifdef BUILD_LK
        dprintf(0,"%s, yashi ili9881c , pin_lcd_id0= %d  lcd_vendor_id is 0x%x \n", __func__, pin_lcd_id0,lcd_id);
        #else
        printk("%s, yashi ili9881c , pin_lcd_id0= %d \n", __func__, pin_lcd_id0);
        #endif
        return (LCM_ID == lcd_id)?1 :0;	
        
    }
    else 
         return 0;
}
#if 0//zhangjian delete
static void ILI9881C_DCS_write_1A_1P(unsigned char cmd, unsigned char para)
{
	unsigned int data_array[16];

	data_array[0] = (0x00023902);
	data_array[1] = (0x00000000 | (para << 8) | (cmd));
	dsi_set_cmdq(data_array, 2, 1);
}
static void ILI9881C_DCS_write_1A_3P(unsigned char cmd, unsigned int para1, unsigned int para2,unsigned int para3)
{
	unsigned int data_array[16];

	data_array[0] = (0x00023902);
	data_array[1] = (0x00000000 | (para1 << 8) | (para1 << 16) | (para1 << 24) | (cmd));
	printk("ILI9881C_DCS_write_1A_3P: %0x\n", data_array[1]);
	dsi_set_cmdq(data_array, 2, 1);
}

#define ILI9881C_DCS_write_1A_0P(cmd)		data_array[0]=(0x00000500 | (cmd<<16)); \
											dsi_set_cmdq(data_array, 1, 1);

static void ILI9881C_GEN_write_1A_1P(unsigned char cmd, unsigned char para)
{
	unsigned int data_array[16];

	data_array[0] = (0x00022902);
	data_array[1] = (0x00000000 | (para << 8) | (cmd));
	dsi_set_cmdq(data_array, 2, 1);
}
static void init_lcm_registers(void)
{
	unsigned int data_array[16];
	printk("yashi init\n");
	ILI9881C_DCS_write_1A_3P(0xFF, 0x98, 0x81, 0x03);

//GIP_1

ILI9881C_DCS_write_1A_1P(0x01, 0x00);
ILI9881C_DCS_write_1A_1P(0x02, 0x00);
ILI9881C_DCS_write_1A_1P(0x03, 0x73);
ILI9881C_DCS_write_1A_1P(0x04, 0x00);
ILI9881C_DCS_write_1A_1P(0x05, 0x00);
ILI9881C_DCS_write_1A_1P(0x06, 0x0C);
ILI9881C_DCS_write_1A_1P(0x07, 0x00);
ILI9881C_DCS_write_1A_1P(0x08, 0x00);
ILI9881C_DCS_write_1A_1P(0x09, 0x19);
ILI9881C_DCS_write_1A_1P(0x0a, 0x01);
ILI9881C_DCS_write_1A_1P(0x0b, 0x01);
ILI9881C_DCS_write_1A_1P(0x0c, 0x0B);
ILI9881C_DCS_write_1A_1P(0x0d, 0x01);
ILI9881C_DCS_write_1A_1P(0x0e, 0x01);
ILI9881C_DCS_write_1A_1P(0x0f, 0x26);
ILI9881C_DCS_write_1A_1P(0x10, 0x26); 
ILI9881C_DCS_write_1A_1P(0x11, 0x00);
ILI9881C_DCS_write_1A_1P(0x12, 0x00);
ILI9881C_DCS_write_1A_1P(0x13, 0x02);
ILI9881C_DCS_write_1A_1P(0x14, 0x00);
ILI9881C_DCS_write_1A_1P(0x15, 0x00);
ILI9881C_DCS_write_1A_1P(0x16, 0x00); 
ILI9881C_DCS_write_1A_1P(0x17, 0x00); 
ILI9881C_DCS_write_1A_1P(0x18, 0x00);
ILI9881C_DCS_write_1A_1P(0x19, 0x00);
ILI9881C_DCS_write_1A_1P(0x1a, 0x00);
ILI9881C_DCS_write_1A_1P(0x1b, 0x00);
ILI9881C_DCS_write_1A_1P(0x1c, 0x00);
ILI9881C_DCS_write_1A_1P(0x1d, 0x00);
ILI9881C_DCS_write_1A_1P(0x1e, 0x44);
ILI9881C_DCS_write_1A_1P(0x1f, 0xC0);  
ILI9881C_DCS_write_1A_1P(0x20, 0x0A);
ILI9881C_DCS_write_1A_1P(0x21, 0x03);
ILI9881C_DCS_write_1A_1P(0x22, 0x0A);   
ILI9881C_DCS_write_1A_1P(0x23, 0x00);
ILI9881C_DCS_write_1A_1P(0x24, 0x8C);  
ILI9881C_DCS_write_1A_1P(0x25, 0x8C); 
ILI9881C_DCS_write_1A_1P(0x26, 0x00);
ILI9881C_DCS_write_1A_1P(0x27, 0x00);
ILI9881C_DCS_write_1A_1P(0x28, 0x3B);  
ILI9881C_DCS_write_1A_1P(0x29, 0x03);
ILI9881C_GEN_write_1A_1P(0x2a, 0x00);
ILI9881C_GEN_write_1A_1P(0x2b, 0x00);
ILI9881C_DCS_write_1A_1P(0x2c, 0x00);
ILI9881C_DCS_write_1A_1P(0x2d, 0x00);
ILI9881C_DCS_write_1A_1P(0x2e, 0x00);
ILI9881C_DCS_write_1A_1P(0x2f, 0x00);
ILI9881C_DCS_write_1A_1P(0x30, 0x00);
ILI9881C_DCS_write_1A_1P(0x31, 0x00);
ILI9881C_DCS_write_1A_1P(0x32, 0x00);
ILI9881C_DCS_write_1A_1P(0x33, 0x00);
ILI9881C_DCS_write_1A_1P(0x34, 0x00);
ILI9881C_DCS_write_1A_1P(0x35, 0x00);
ILI9881C_DCS_write_1A_1P(0x36, 0x00);
ILI9881C_DCS_write_1A_1P(0x37, 0x00);
ILI9881C_DCS_write_1A_1P(0x38, 0x00);
ILI9881C_DCS_write_1A_1P(0x39, 0x00);
ILI9881C_DCS_write_1A_1P(0x3a, 0x00);
ILI9881C_DCS_write_1A_1P(0x3b, 0x00);
ILI9881C_DCS_write_1A_1P(0x3c, 0x00);
ILI9881C_DCS_write_1A_1P(0x3d, 0x00);
ILI9881C_DCS_write_1A_1P(0x3e, 0x00);
ILI9881C_DCS_write_1A_1P(0x3f, 0x00);
ILI9881C_DCS_write_1A_1P(0x40, 0x00);
ILI9881C_DCS_write_1A_1P(0x41, 0x00);
ILI9881C_DCS_write_1A_1P(0x42, 0x00);
ILI9881C_DCS_write_1A_1P(0x43, 0x00);
ILI9881C_DCS_write_1A_1P(0x44, 0x00);
                            
                            
//GIP_2                     
ILI9881C_DCS_write_1A_1P(0x50, 0x01);
ILI9881C_DCS_write_1A_1P(0x51, 0x23);
ILI9881C_DCS_write_1A_1P(0x52, 0x45);
ILI9881C_DCS_write_1A_1P(0x53, 0x67);
ILI9881C_DCS_write_1A_1P(0x54, 0x89);
ILI9881C_DCS_write_1A_1P(0x55, 0xab);
ILI9881C_DCS_write_1A_1P(0x56, 0x01);
ILI9881C_DCS_write_1A_1P(0x57, 0x23);
ILI9881C_DCS_write_1A_1P(0x58, 0x45);
ILI9881C_DCS_write_1A_1P(0x59, 0x67);
ILI9881C_DCS_write_1A_1P(0x5a, 0x89);
ILI9881C_DCS_write_1A_1P(0x5b, 0xab);
ILI9881C_DCS_write_1A_1P(0x5c, 0xcd);
ILI9881C_DCS_write_1A_1P(0x5d, 0xef);
                            
//GIP_3                     
ILI9881C_DCS_write_1A_1P(0x5e, 0x11);
ILI9881C_DCS_write_1A_1P(0x5f, 0x02);
ILI9881C_DCS_write_1A_1P(0x60, 0x00);
ILI9881C_DCS_write_1A_1P(0x61, 0x0C);
ILI9881C_DCS_write_1A_1P(0x62, 0x0D);
ILI9881C_DCS_write_1A_1P(0x63, 0x0E);
ILI9881C_DCS_write_1A_1P(0x64, 0x0F);
ILI9881C_DCS_write_1A_1P(0x65, 0x02);
ILI9881C_DCS_write_1A_1P(0x66, 0x02);
ILI9881C_DCS_write_1A_1P(0x67, 0x02);
ILI9881C_DCS_write_1A_1P(0x68, 0x02);
ILI9881C_DCS_write_1A_1P(0x69, 0x02);
ILI9881C_DCS_write_1A_1P(0x6a, 0x02);
ILI9881C_DCS_write_1A_1P(0x6b, 0x02);
ILI9881C_DCS_write_1A_1P(0x6c, 0x02);
ILI9881C_DCS_write_1A_1P(0x6d, 0x02);
ILI9881C_DCS_write_1A_1P(0x6e, 0x05);
ILI9881C_DCS_write_1A_1P(0x6f, 0x05);
ILI9881C_DCS_write_1A_1P(0x70, 0x05);
ILI9881C_DCS_write_1A_1P(0x71, 0x05);
ILI9881C_DCS_write_1A_1P(0x72, 0x01);
ILI9881C_DCS_write_1A_1P(0x73, 0x06);
ILI9881C_DCS_write_1A_1P(0x74, 0x07);	
ILI9881C_DCS_write_1A_1P(0x75, 0x02);
ILI9881C_DCS_write_1A_1P(0x76, 0x00);
ILI9881C_DCS_write_1A_1P(0x77, 0x0C);
ILI9881C_DCS_write_1A_1P(0x78, 0x0D);
ILI9881C_DCS_write_1A_1P(0x79, 0x0E);
ILI9881C_DCS_write_1A_1P(0x7a, 0x0F);
ILI9881C_DCS_write_1A_1P(0x7b, 0x02);
ILI9881C_DCS_write_1A_1P(0x7c, 0x02);
ILI9881C_DCS_write_1A_1P(0x7d, 0x02);
ILI9881C_DCS_write_1A_1P(0x7e, 0x02);
ILI9881C_DCS_write_1A_1P(0x7f, 0x02);
ILI9881C_DCS_write_1A_1P(0x80, 0x02);
ILI9881C_DCS_write_1A_1P(0x81, 0x02);
ILI9881C_DCS_write_1A_1P(0x82, 0x02);
ILI9881C_DCS_write_1A_1P(0x83, 0x02);
ILI9881C_DCS_write_1A_1P(0x84, 0x05);
ILI9881C_DCS_write_1A_1P(0x85, 0x05);
ILI9881C_DCS_write_1A_1P(0x86, 0x05);
ILI9881C_DCS_write_1A_1P(0x87, 0x05);
ILI9881C_DCS_write_1A_1P(0x88, 0x01);
ILI9881C_DCS_write_1A_1P(0x89, 0x06);
ILI9881C_DCS_write_1A_1P(0x8A, 0x07);
                            
                            
//CMD_Page 4                
ILI9881C_DCS_write_1A_3P(0xFF, 0x98, 0x81, 0x04);
ILI9881C_DCS_write_1A_1P(0x6C, 0x15);
ILI9881C_DCS_write_1A_1P(0x6E, 0x1A);               //di_pwr_reg=0 VGH clamp 
ILI9881C_DCS_write_1A_1P(0x6F, 0x25);              // reg vcl + VGH pumping ratio 3x VGL=-2x
                              
ILI9881C_DCS_write_1A_1P(0x8D, 0x20);               //VGL clamp -10V
ILI9881C_DCS_write_1A_1P(0x87, 0xBA);     
ILI9881C_DCS_write_1A_1P(0x26, 0x76);		//5.2 none
ILI9881C_DCS_write_1A_1P(0xb2, 0xd1);          //5.2 none
ILI9881C_DCS_write_1A_1P(0x3B, 0xC0);
ILI9881C_DCS_write_1A_1P(0x7A, 0x10);
ILI9881C_DCS_write_1A_1P(0x71, 0xB0);


//CMD_Page 1
ILI9881C_DCS_write_1A_3P(0xFF, 0x98, 0x81, 0x01);
ILI9881C_DCS_write_1A_1P(0x22, 0x0A);		//BGR, 0x SS
ILI9881C_DCS_write_1A_1P(0x53, 0x7D);		//VCOM1
ILI9881C_DCS_write_1A_1P(0x55, 0x8A);		//VCOM2
ILI9881C_DCS_write_1A_1P(0x50, 0x95);         	//VREG1OUT=4.9V
ILI9881C_DCS_write_1A_1P(0x51, 0x95);         	//VREG2OUT=-4.9V
ILI9881C_DCS_write_1A_1P(0x31, 0x00);		//column inversion
ILI9881C_DCS_write_1A_1P(0x40, 0x53);
ILI9881C_DCS_write_1A_1P(0x43, 0x66);
ILI9881C_DCS_write_1A_1P(0x60, 0x1B);               //SDT
ILI9881C_DCS_write_1A_1P(0x61, 0x01);
ILI9881C_DCS_write_1A_1P(0x62, 0x0C);
ILI9881C_DCS_write_1A_1P(0x63, 0x00);

//VP255	Gamma P
ILI9881C_DCS_write_1A_1P(0xA0, 0x1D);
//VP251		                    
ILI9881C_DCS_write_1A_1P(0xA1, 0x28);
//VP247                    
ILI9881C_DCS_write_1A_1P(0xA2, 0x31);
//VP243                         
ILI9881C_DCS_write_1A_1P(0xA3, 0x15);
//VP239                    
ILI9881C_DCS_write_1A_1P(0xA4, 0x16); 
//VP231                         
ILI9881C_DCS_write_1A_1P(0xA5, 0x22);
//VP219                    
ILI9881C_DCS_write_1A_1P(0xA6, 0x17);
//VP203                         
ILI9881C_DCS_write_1A_1P(0xA7, 0x1B);
//VP175                         
ILI9881C_DCS_write_1A_1P(0xA8, 0x91);
//VP144                         
ILI9881C_DCS_write_1A_1P(0xA9, 0x1C);
//VP111                         
ILI9881C_DCS_write_1A_1P(0xAA, 0x29);
//VP80                          
ILI9881C_DCS_write_1A_1P(0xAB, 0x91);
//VP52                     
ILI9881C_DCS_write_1A_1P(0xAC, 0x1D);
//VP36                          
ILI9881C_DCS_write_1A_1P(0xAD, 0x1C);
//VP24                          
ILI9881C_DCS_write_1A_1P(0xAE, 0x55);
//VP16                          
ILI9881C_DCS_write_1A_1P(0xAF, 0x21);
//VP12                          
ILI9881C_DCS_write_1A_1P(0xB0, 0x25);
//VP8                           
ILI9881C_DCS_write_1A_1P(0xB1, 0x5E);
//VP4                      
ILI9881C_DCS_write_1A_1P(0xB2, 0x6D);
//VP0                           
ILI9881C_DCS_write_1A_1P(0xB3, 0x39);                        
                                
                              
//VN255 GAMMA N                                                 
ILI9881C_DCS_write_1A_1P(0xC0, 0x1D);		
//VN251                         
ILI9881C_DCS_write_1A_1P(0xC1, 0x27);
//VN247                         
ILI9881C_DCS_write_1A_1P(0xC2, 0x2F);
//VN243                   
ILI9881C_DCS_write_1A_1P(0xC3, 0x08);
//VN239                         
ILI9881C_DCS_write_1A_1P(0xC4, 0x0A); 
//VN231                         
ILI9881C_DCS_write_1A_1P(0xC5, 0x1E);
//VN219                         
ILI9881C_DCS_write_1A_1P(0xC6, 0x12);
//VN203                         
ILI9881C_DCS_write_1A_1P(0xC7, 0x17);
//VN175                         
ILI9881C_DCS_write_1A_1P(0xC8, 0x92);
//VN144                    
ILI9881C_DCS_write_1A_1P(0xC9, 0x1D);
//VN111                         
ILI9881C_DCS_write_1A_1P(0xCA, 0x28);
//VN80                          
ILI9881C_DCS_write_1A_1P(0xCB, 0x8F);
//VN52                          
ILI9881C_DCS_write_1A_1P(0xCC, 0x19);
//VN36                          
ILI9881C_DCS_write_1A_1P(0xCD, 0x15);
//VN24                          
ILI9881C_DCS_write_1A_1P(0xCE, 0x47);
//VN16                     
ILI9881C_DCS_write_1A_1P(0xCF, 0x21);
//VN12                          
ILI9881C_DCS_write_1A_1P(0xD0, 0x29); 
//VN8                           
ILI9881C_DCS_write_1A_1P(0xD1, 0x61);
//VN4                           
ILI9881C_DCS_write_1A_1P(0xD2, 0x6F);
//VN0                           
ILI9881C_DCS_write_1A_1P(0xD3, 0x39);              
                                
                              

//CMD_Page 0
ILI9881C_DCS_write_1A_3P(0xFF, 0x98, 0x81, 0x00);
ILI9881C_DCS_write_1A_0P(0x35);
ILI9881C_DCS_write_1A_0P(0x11);
MDELAY(120);
ILI9881C_DCS_write_1A_0P(0x29);
MDELAY(20);                  
         
                     
}
#endif
static void lcm_init(void)
{
    #ifdef BUILD_LK
    printf("lcm_init:yashi ili9881c \n");
    #else
    printk("lcm_init:yashi ili9881c \n");
    #endif
    SET_RESET_PIN(1);
    MDELAY(10);//5
    SET_RESET_PIN(0);
    MDELAY(10);//50
    SET_RESET_PIN(1);
    MDELAY(120);//100
    
    //init_lcm_registers();
    push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(5);//100
    gpio_set_value(GPIO_DISP_BL_EN, 1);


}
static void lcm_suspend(void)
{	
    push_table(lcm_sleep_in_setting, sizeof(lcm_sleep_in_setting) / sizeof(struct LCM_setting_table), 1);
    MDELAY(5);
    SET_RESET_PIN(0);
    printk("lcm_suspend:yashi ili9881c 101ms \n");
    MDELAY(100);
      gpio_set_value(GPIO_DISP_BL_EN, 0);
	
}



static void lcm_resume(void)
{

	lcm_init();
//	push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);	
}
#endif
static void lcm_setbacklight(unsigned int level)
{
        unsigned char level_high,level_low;
#ifdef BUILD_LK
	dprintf(0,"%s,zhangjian lk ili9881c_dsi_vdo_yashi_hd720: level = %d\n", __func__, level);
#else
	printk("%s, zhangjian kernel ili9881c_dsi_vdo_yashi_hd720 backlight: level = %d\n", __func__, level);
#endif
        // Refresh value of backlight level.
        
        level = 4095*level /255;	
        level_high = (level >> 8) & 0x0F;
        level_low = level & 0x0FF;
        printk("%s,  ili9881c_dsi_vdo_yashi_hd720 backlight: new level = 0x%x evel_high, level_low 0x%x,0x%x\n", __func__, level,level_high, level_low);
        
        lcm_backlight_level_setting[1].para_list[0] = level_high;
        lcm_backlight_level_setting[1].para_list[1] = level_low;
        
        push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);       
}

LCM_DRIVER ili9881c_hd720_dsi_vdo_lcm_drv = 
{
	.name		= "aeon_ili9881c_hd720_dsi_vdo_f509_xiaozhixing",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.init_power		= lcm_init_power,
    .resume_power = lcm_resume_power,
    .suspend_power = lcm_suspend_power,
	.set_backlight	= lcm_setbacklight,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
};

