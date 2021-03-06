#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/mt_gpio.h>
#include <string.h>
#include <platform/mt_i2c.h>
#include <cust_gpio_usage.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
#include "data_rgba4444_roundedpattern.h"
#endif

#ifndef BUILD_LK
extern int bEnTGesture;
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1440)

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifdef BUILD_LK
#define LCM_PRINT printf
#else
#if defined(BUILD_UBOOT)
#define LCM_PRINT printf
#else
#define LCM_PRINT pr_debug
#endif
#endif

#define LCM_DBG(fmt, arg...) \
    LCM_PRINT ("[ft8613_tcl] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)                                    (lcm_util.set_reset_pin((v)))

#ifdef GPIO_LCM_PWR
#define SET_PWR_PIN(v)    \
    mt_set_gpio_mode(GPIO_LCM_PWR,GPIO_MODE_00);    \
    mt_set_gpio_dir(GPIO_LCM_PWR,GPIO_DIR_OUT);     \
    if(v)                                           \
        mt_set_gpio_out(GPIO_LCM_PWR,GPIO_OUT_ONE); \
    else                                            \
        mt_set_gpio_out(GPIO_LCM_PWR,GPIO_OUT_ZERO);
#endif
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#ifdef BUILD_LK
#define GPIO_LCD_ID (GPIO2 | 0x80000000)
#else
extern void lcm_enn(int onoff);
extern void lcm_enp(int onoff);
extern  int _lcm_i2c_write_bytes(unsigned char addr, unsigned char value);
extern int get_lcm_id_status(void);
#endif

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)            lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)                                           lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)                   lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE                          0

#define REGFLAG_END_OF_TABLE                                      0xFD   // END OF REGISTERS MARKER
#define REGFLAG_DELAY                                           0xFC

struct LCM_setting_table
{
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table  lcm_deep_sleep_mode_in_setting_v2[] =
{
   // {0x00, 1, {0x00}},
   // {0xff, 3, {0x87,0x16,0x01}},
   // {0x00, 1, {0x80}},
   // {0xff, 2, {0x87,0x16}},


    //{0x00, 1, {0xEA}},
   // {0xF5, 1, {0x01}}, 

    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}}, 

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}}, 

    {0x00, 1, {0x00}}, //deep standby
    {0xF7, 4, {0x5A,0xA5,0x87,0x16}}, 

};


static struct LCM_setting_table  lcm_deep_sleep_mode_in_setting_v3[] =
{
   // {0x00, 1, {0x00}},
   // {0xff, 3, {0x87,0x16,0x01}},
   // {0x00, 1, {0x80}},
   // {0xff, 2, {0x87,0x16}},

    // Display off sequence
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}}, 

    // Sleep Mode On
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}}, 

    {0x00, 1, {0x00}}, //deep standby
    {0xF7, 4, {0x5A,0xA5,0x87,0x16}}, 

};

static struct LCM_setting_table lcm_initialization_setting_v2[] =
{
    {0x00, 1, {0x00}},
    {0xff, 3, {0x87,0x16,0x01}},
    {0x00, 1, {0x80}},
    {0xff, 2, {0x87,0x16}},
    {0x00, 1, {0x80}},
    {0xC0, 15, {0x00,0xb2,0x00,0x10,0x10,0x00,0xb2,0x10,0x10,0x00,0xb2,0x00,0x10,0x10,0x00}},
    {0x00, 1, {0x80}},
    {0xF3, 1, {0x70}},

    {0x00, 1, {0xA0}},
    {0xC0, 7, {0x0b,0x01,0x01,0x01,0x01,0x26,0x0C}},
    {0x00, 1, {0xD0}},
    {0xC0, 7, {0x0b,0x01,0x01,0x01,0x01,0x26,0x0C}},

    //----- GOA -----
    // VST
    {0x00, 1, {0x80}},
    {0xC2, 12, {0x84,0x00,0x00,0x81,0x83,0x00,0x00,0x81,0x85,0x00,0x30,0xB7}},
    // CKV
    {0x00, 1, {0xB0}},
    {0xC2, 15, {0x84,0x01,0x00,0x07,0x88,0x83,0x02,0x00,0x07,0x88,0x82,0x03,0x00,0x07,0x88}},
    {0x00, 1, {0xC0}},
    {0xC2, 5, {0x81,0x04,0x00,0x07,0x88}},
    // CKV period
    {0x00, 1, {0xDA}},
    {0xC2, 2, {0x33,0x33}},
    //CKV 5 setting for D2U & GAS2 (replace GOFF)
    {0x00, 1, {0xC5}},
    {0xC2, 5, {0x81,0x03,0x00,0xB4,0x00}},

    // CKV TP ctrl
    {0x00, 1, {0xAA}},
    {0xC3, 2, {0x99,0x9C}},
    {0x00, 1, {0xAC}},
    {0xC3, 1, {0x99}},
    // CKH BP FB
    {0x00, 1, {0x82}},
    {0xa5, 3, {0x44,0x04,0x0C}},
    // CKH TP term
    {0x00, 1, {0x87}},
    {0xa5, 4, {0x00,0x07,0x77,0x77}},
    // EQ function
    {0x00, 1, {0x88}},
    {0xc3, 2, {0x22,0x22}},
    {0x00, 1, {0x98}},
    {0xc3, 2, {0x22,0x22}},

    //----- tcon TP setting -----
    {0x00, 1, {0x80}},
    {0xCE, 9, {0x25,0x00,0x78,0x00,0x78,0xFF,0x00,0x00,0x05}},
    {0x00, 1, {0x90}},
    {0xCE, 8, {0x00,0x5C,0x06,0xce,0x00,0x5C,0x00,0x00}},
    {0x00, 1, {0xA0}},
    {0xCE, 8, {0x00,0x05,0x01,0x01,0x01,0x01,0x3F,0x0A}},
    {0x00, 1, {0xB0}},
    {0xCE, 6, {0x00,0x00,0x60,0x60,0x00,0x60}},
    //----- panel interface -----
    {0x00, 1, {0x80}},  //U2D
    {0xCC, 12, {0x02,0x03,0x06,0x07,0x08,0x09,0x0A,0x18,0x22,0x22,0x22,0x22}},
    {0x00, 1, {0x90}},  //D2U
    {0xCC, 12, {0x03,0x02,0x09,0x08,0x07,0x06,0x19,0x0A,0x22,0x22,0x22,0x22}},
    {0x00, 1, {0xA0}},  //no dir 1
    {0xCC, 15, {0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x18,0x19,0x20,0x21,0x04,0x14,0x15,0x0A,0x0A}},
    {0x00, 1, {0xB0}},  //no dir 2
    {0xCC, 5, {0x22,0x22,0x22,0x22,0x22}},
    {0x00, 1, {0x80}},  //slp in
    {0xCB, 8, {0x0F,0xC0,0x00,0x3F,0x00,0xC0,0x00,0x00}},
    {0x00, 1, {0x90}},  //power on 1
    {0xCB, 15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0x00, 1, {0xA0}},  //power on 2
    {0xCB, 15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
    {0x00, 1, {0xB0}},  //power on 3
    {0xCB, 1, {0x00,0x00}},
    {0x00, 1, {0xC0}},  //power off 1
    {0xCB, 15, {0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x05,0x05,0x05}},
    {0x00, 1, {0xD0}},  //power off 2
    {0xCB, 15, {0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x05,0x0A,0x00,0x00,0x00}},
    {0x00, 1, {0xE0}},  //power off 3
    {0xCB, 2, {0x00,0x00}},
    {0x00, 1, {0xF0}},  //LVD
    {0xCB, 8, {0x0F,0xC0,0x00,0x3F,0x00,0xC0,0x00,0x00}},
    {0x00, 1, {0x80}},  //CGOUT R1
    {0xCD, 15, {0x1B,0x1B,0x19,0x01,0x04,0x06,0x08,0x07,0x18,0x17,0x03,0x05,0x1A,0x22,0x0F}},
    {0x00, 1, {0x90}},  //CGOUT R2
    {0xCD, 3, {0x0E,0x0D,0x22}},
    {0x00, 1, {0xA0}},  //CGOUT L1
    {0xCD, 15, {0x1B,0x1B,0x19,0x02,0x05,0x03,0x08,0x07,0x18,0x17,0x06,0x04,0x1A,0x22,0x0F}},
    {0x00, 1, {0xB0}},  //CGOUT L2
    {0xCD, 3, {0x0E,0x0D,0x22}},
    {0x00, 1, {0x81}},      //ALL gate on off
    {0xF3, 12, {0x08,0xC1,0x40,0x08,0xC1,0x40,0x00,0x12,0x00,0x00,0x12,0x00}},
    //----- other -----

    {0x00, 1, {0x80}},
    {0xC5, 10, {0x00,0xC1,0xDD,0xC4,0x14,0x1E,0x00,0x55,0x50,0x05}},
    {0x00, 1, {0x90}},
    {0xC5, 10, {0x77,0x1E,0x14,0x00,0x88,0x10,0x4C,0x53,0x55,0x50}},
    {0x00, 1, {0x00}},
    {0xD8, 2, {0x31,0x31}}, //modified
    {0x00, 1, {0x00}},
    {0xE1, 24, {0x00,0x07,0x1B,0x2E,0x3A,0x47,0x59,0x68,0x6E,0x77,0x80,0x8A,0x6E,0x66,0x63,0x5C,0x4E,0x43,0x35,0x2C,0x23,0x15,0x08,0x07}},
    {0x00, 1, {0x00}},
    {0xE2, 24, {0x00,0x07,0x1B,0x2E,0x3A,0x47,0x59,0x68,0x6E,0x77,0x80,0x8A,0x6E,0x66,0x63,0x5C,0x4E,0x43,0x35,0x2C,0x23,0x15,0x08,0x07}},
    //{0x00, 1, {0x00}},
    //{0xD9, 5, {0x80,0xBC,0xBC,0xBC,0xBC}},    //VCOM Setting
    //{0x00, 1, {0xd1}},
    //{0xcf, 12, {0x02,0x04,0x00,0x0A,0x00,0x14,0x02,0x04,0x00,0x0C,0x00,0x12}},
    {0x00, 1, {0xc0}},
    {0xc0, 2, {0x11,0x11}}, //poweroff2
    //add vcom pull GND
    {0x00, 1, {0x8d}},
    {0xf5, 1, {0x21}},
    //For 720x1440
    {0x00, 1, {0x00}},
    {0x2a, 4, {0x00,0x00,0x02,0xcf}},
    {0x00, 1, {0x00}},
    {0x2b, 4, {0x00,0x00,0x05,0x9f}},
    {0x00, 1, {0xa0}},
    {0xb3, 7, {0x03,0x02,0xD0,0x05,0xA0,0x00,0x58}},

    {0x00, 1, {0x81}},  
    {0xCF, 2, {0x05,0x04}},
    {0x00, 1, {0x84}},  
    {0xCF, 2, {0x05,0x04}},
    {0x00, 1, {0xC3}},
    {0xF5, 1, {0x26}},
    {0x00, 1, {0xC7}},
    {0xF5, 1, {0x26}},    
    {0x00, 1, {0xD3}},
    {0xF5, 1, {0x26}},
    {0x00, 1, {0xD7}},
    {0xF5, 1, {0x26}},    
    {0x00, 1, {0x95}},
    {0xF5, 1, {0x26}},
    {0x00, 1, {0x98}},
    {0xF5, 1, {0x26}}, 
    {0x00, 1, {0xB1}},
    {0xF5, 1, {0x21}},   
    {0x35, 1, {0x00}},

    {0x00, 1, {0x00}},
    {0xff, 3, {0x00,0x00,0x00}},
    {0x00, 1, {0x80}},
    {0xff, 2, {0x00,0x00}},

    {0x11, 1, {0x00}},
    {REGFLAG_DELAY,120,{}},
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY,10,{}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;
    for(i = 0; i < count; i++)
    {
        unsigned cmd;
        cmd = table[i].cmd;
        switch (cmd)
        {
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

    params->physical_width  = 64.38;
    params->physical_height = 130.71;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode   = CMD_MODE;
#else
    params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;
#endif

    // DSI
    /* Command mode setting */
    //1 Three lane or Four lane
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    // Video mode setting
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 4;  // 4
    params->dsi.vertical_backporch = 16;  //16
    params->dsi.vertical_frontporch = 24; // 16
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 24;  // 24
    params->dsi.horizontal_backporch = 64; // 64
    params->dsi.horizontal_frontporch = 64; // 64
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;

    // Bit rate calculation
    params->dsi.PLL_CLOCK=241;
    params->dsi.ssc_disable=1;

    //yixuhong 20150511 add esd check function
#ifndef BUILD_LK
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 0;//0:te esd check 1:read register
    params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
    params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
#endif /*BUILD_LK*/

// huaidong.tan , CDAAAE-652 , DATE20171113 , porting mtk round corner
#ifdef CONFIG_MTK_ROUND_CORNER_SUPPORT
    params->corner_pattern_width = 32;//720;//Note:\u8fd9\u91cc\u662f\u5c4f\u5e55\u7684\u5bbd\u5ea6\uff0c\u4e0d\u662f\u539f\u59cb\u56fe\u7247\u5bbd\u5ea6
    params->corner_pattern_height = 32;//\u5706\u89d2\u7684\u9ad8\u5ea6
#endif

#ifdef MTK_ROUND_CORNER_SUPPORT
    params->round_corner_params.w = ROUND_CORNER_W;
    params->round_corner_params.h = ROUND_CORNER_H;
    params->round_corner_params.lt_addr = left_top;
    params->round_corner_params.rt_addr = right_top;
    params->round_corner_params.lb_addr = left_bottom;
    params->round_corner_params.rb_addr = right_bottom;
#endif
}

#ifdef BUILD_LK
static int gpio_bl_enp   = (54 | 0x80000000);
static int gpio_bl_enn   = (53 | 0x80000000);
static struct mt_i2c_t LCD_i2c;

static kal_uint32 lcd_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    LCD_i2c.id = I2C1;

    LCD_i2c.addr = (0x3e);
    LCD_i2c.mode = ST_MODE;
    LCD_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&LCD_i2c, write_data, len);


    return ret_code;
}
#endif

static void lcm_init(void)
{
    LCM_DBG();
    SET_RESET_PIN(1);
    MDELAY(5);
#ifdef BUILD_LK
    mt_set_gpio_mode(gpio_bl_enp,GPIO_MODE_00);
    mt_set_gpio_dir(gpio_bl_enp,GPIO_DIR_OUT);
    mt_set_gpio_mode(gpio_bl_enn,GPIO_MODE_00);
    mt_set_gpio_dir(gpio_bl_enn,GPIO_DIR_OUT);
    mt_set_gpio_out(gpio_bl_enp,GPIO_OUT_ONE);
    MDELAY(1);
    lcd_write_byte(0,0x12);
    MDELAY(1);
    lcd_write_byte(1,0x12);
    MDELAY(1);
    mt_set_gpio_out(gpio_bl_enn,GPIO_OUT_ONE);
#else //Kernel driver
    lcm_enp(1);
    MDELAY(3);
    _lcm_i2c_write_bytes(0,0x12);//5.8
    MDELAY(3);
    _lcm_i2c_write_bytes(1,0x12);
    MDELAY(3);
    lcm_enn(1);
#endif
#ifdef GPIO_LCM_PWR
    SET_PWR_PIN(0);
    MDELAY(20);
    SET_PWR_PIN(1);
    MDELAY(150);
#endif
    SET_RESET_PIN(0);
    MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(60);
    LCM_DBG("jacky debug,lcm reset end \n");
    push_table(lcm_initialization_setting_v2, sizeof(lcm_initialization_setting_v2) / sizeof(struct LCM_setting_table), 1);
    LCM_DBG("jacy debug,lcm init end \n");
}

static void lcm_suspend(void)
{
    LCM_DBG();

#ifndef BUILD_LK
    LCM_DBG("geroge suspend bEnTGesture  = %d\n",bEnTGesture);
    if(!bEnTGesture)
    {
     push_table(lcm_deep_sleep_mode_in_setting_v2, sizeof(lcm_deep_sleep_mode_in_setting_v2) / sizeof(struct LCM_setting_table), 1);
       
        SET_RESET_PIN(1);
        SET_RESET_PIN(0);
        MDELAY(10); // 1ms

        lcm_enp(0);
        MDELAY(1);
        lcm_enn(0);

	SET_RESET_PIN(1);
        MDELAY(50);

    }else{

       push_table(lcm_deep_sleep_mode_in_setting_v3, sizeof(lcm_deep_sleep_mode_in_setting_v3) / sizeof(struct LCM_setting_table), 1);

}
#endif
   
}

static void lcm_resume(void)
{

#ifndef BUILD_LK
    LCM_DBG("geroge resume bEnTGesture  = %d\n",bEnTGesture);
    if(!bEnTGesture)
    {
        lcm_enp(1);
        MDELAY(1);
        _lcm_i2c_write_bytes(0,0x12);
        MDELAY(1);
        _lcm_i2c_write_bytes(1,0x12);
        MDELAY(1);
        lcm_enn(1);
    }
#endif
    SET_RESET_PIN(1);
    MDELAY(1);
    SET_RESET_PIN(0);
    MDELAY(5);
    SET_RESET_PIN(1);
    MDELAY(20);
    push_table(lcm_initialization_setting_v2, sizeof(lcm_initialization_setting_v2) / sizeof(struct LCM_setting_table), 1);
    LCM_DBG();
}

#define READ_LCD_REGISTER
#define LCD_ID 0x8716
static unsigned int lcm_compare_id(void)
{
#ifdef READ_LCD_REGISTER
    char id_high=0;
    char id_low=0;
    int id=0;
    int array[4];
    char buffer[5];

    array[0]=0x00043700;//0x00023700;
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0xA1, buffer,4);
    LCM_DBG("buffer = 0x%x,0x%x,0x%x,0x%x\n", buffer[0],buffer[1],buffer[2],buffer[3]);
    id_high = buffer[2]; ///////////////////////0x87
    id_low = buffer[3]; ///////////////////////0x16

    id = (id_high << 8) | id_low;
    LCM_DBG("id = %x\n", id);

    if(LCD_ID == id)
    {
        return 1;
    }
    return 0;

#else //read hw id
    s32 lcd_hw_id = -1;

#ifdef BUILD_LK
    mt_set_gpio_mode(GPIO_LCD_ID,GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_ID,GPIO_DIR_IN);
    lcd_hw_id = mt_get_gpio_in(GPIO_LCD_ID);
#else
    lcd_hw_id = get_lcm_id_status();
#endif
    LCM_DBG("lcm_compare_id lcd_hw_id=%d \n",lcd_hw_id);
    if (0 == lcd_hw_id)
    {
        return 1;
    }
    else
    {
        return 0;
    }
#endif /*READ_LCD_REGISTER*/
}

LCM_DRIVER ft8613_p200_hdplus_dsi_vdo_tcl_lcm_drv =
{
    .name       = "ft8613_hdplus_dsi_vdo_tcl",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,
    .compare_id     = lcm_compare_id,
};

