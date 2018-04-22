/*

  generate and update c/c++ files
  this also replaces the buildcpp tool

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <error.h>
#include <ctype.h>
#include <unistd.h>

/*===========================================*/

/* ll_hvline */
#define U8X8_HVLINE "u8g2_ll_hvline_vertical_top_lsb"

#define COM_4WSPI		0x0001
#define COM_3WSPI		0x0002
#define COM_6800		0x0004
#define COM_8080		0x0008
#define COM_I2C		0x0010
#define COM_ST7920SPI	0x0020			/* mostly identical to COM_4WSPI, but does not use DC */
#define COM_UART		0x0040
#define COM_KS0108	0x0080			/* mostly identical to 6800 mode, but has more chip select lines */
#define COM_SED1520	0x0100			

struct interface
{
  const char *interface_name;		/* SW_SPI */
  const char *setpin_function;		/* u8x8_SetPin_4Wire_SW_SPI */
  const char *arduino_com_procedure;	/* u8x8_byte_4wire_sw_spi */
  const char *arduino_gpio_procedure;	/* u8x8_gpio_and_delay_arduino */
  const char *pins_with_type;		/* uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE */
  const char *pins_plain;			/* clock, data, cs, dc, reset */
  const char *pins_md_plain;			/* clock, data, cs, dc, reset */
  const char *generic_com_procedure;	/* u8x8_byte_4wire_sw_spi, this is used for documentation, so it suould be generic for all uC architectures */
};


struct display
{
  /* this name must match the display part of the device procedure */
  /* u8x8_d_<controller>_<display> */
  

  const char *name;	
};

struct controller
{
  /* the name must match the controller part of the device procedure */
  /* u8x8_d_<controller>_<display> */
  const char *name;
  
  int tile_width;
  int tile_height;
  const char *ll_hvline;
  const char *cad;
  const char *cad_shortname;
  unsigned com;
  char *note;
  unsigned is_generate_u8g2_class;	/* currently not used, instead conrolled by COM_UART */
  struct display display_list[10];	/* usually not used completly, but space does not matter much here */  
};

struct controller controller_list[] =
{
  {
    "ssd1305", 	16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32_noname" },
      { NULL }
    }
  },
  {
    "ssd1305", 	16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32_noname" },
      { NULL }
    }
  },

  {
    "ssd1305", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_adafruit" },
      { NULL }
    }
  },
  {
    "ssd1305", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_adafruit" },
      { NULL }
    }
  },
  
  {
    "ssd1306", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname" },
      { "128x64_vcomh0" },
      { "128x64_alt0" },
      { NULL }
    }
  },
  {
    "ssd1306", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname" },
      { "128x64_vcomh0" },
      { "128x64_alt0" },
      { NULL }
    }
  },

   {
    "sh1106", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname" },
      { "128x64_vcomh0" },
      { "128x64_winstar" },
      { NULL }
    }
  },
  {
    "sh1106", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname" },
      { "128x64_vcomh0" },
      { "128x64_winstar" },
      { NULL }
    }
  },

   {
    "sh1106", 	9, 	5, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "72x40_wise" },
      { NULL }
    }
  },
  {
    "sh1106", 	9, 	5, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "72x40_wise" },
      { NULL }
    }
  },

   {
    "sh1106", 	8, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "64x32" },
      { NULL }
    }
  },
  {
    "sh1106", 	8, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "64x32" },
      { NULL }
    }
  },
  
   {
    "sh1107", 	 8, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "64x128" },
      { NULL }
    }
  },
  {
    "sh1107", 	 8, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "64x128" },
      { NULL }
    }
  },
  
   {
    "sh1107", 	12, 	12, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "seeed_96x96" },
      { NULL }
    }
  },
  {
    "sh1107", 	12, 	12, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "seeed_96x96" },
      { NULL }
    }
  },

   {
    "sh1107", 	 16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "128x128" },
      { NULL }
    }
  },
  {
    "sh1107", 	 16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "128x128" },
      { NULL }
    }
  },
  
   {
    "sh1122", 	 32, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "256x64" },
      { NULL }
    }
  },
  {
    "sh1122", 	 32, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "256x64" },
      { NULL }
    }
  },

  {
    "ssd1306", 	16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32_univision" },
      { NULL }
    }
  },
  {
    "ssd1306", 	16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32_univision" },
      { NULL }
    }
  },  

  {
    "ssd1306", 	8, 	6, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "64x48_er" },
      { NULL }
    }
  },
  {
    "ssd1306", 	8, 	6, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "64x48_er" },
      { NULL }
    }
  },  
  
  {
    "ssd1306", 	6, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "48x64_winstar" },
      { NULL }
    }
  },
  {
    "ssd1306", 	6, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "48x64_winstar" },
      { NULL }
    }
  },  


  {
    "ssd1306", 	8, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "64x32_noname" },
      { "64x32_1f" },
      { NULL }
    }
  },
  {
    "ssd1306", 	8, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "64x32_noname" },
      { "64x32_1f" },
      { NULL }
    }
  }, 

  {
    "ssd1306", 	12, 	2, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "96x16_er" },
      { NULL }
    }
  },
  {
    "ssd1306", 	12, 	2, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "96x16_er" },
      { NULL }
    }
  },  
  
  
  {
    "ssd1309", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname2" },
      { NULL }
    }
  },
  {
    "ssd1309", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname2" },
      { NULL }
    }
  },
  

  {
    "ssd1309", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname0" },
      { NULL }
    }
  },
  {
    "ssd1309", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64_noname0" },
      { NULL }
    }
  },
  
  {
    "ssd1325", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_128x64" },
      { NULL }
    }
  },
  {
    "ssd1325", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_128x64" },
      { NULL }
    }
  },  

  {
    "ssd1326", 	32, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "er_256x32" },
      { NULL }
    }
  },
  {
    "ssd1326", 	32, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "er_256x32" },
      { NULL }
    }
  },  
  

  {
    "ssd1327", 	12, 	12, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "seeed_96x96" },
      { NULL }
    }
  },
  {
    "ssd1327", 	12, 	12, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "seeed_96x96" },
      { NULL }
    }
  },  

  {
    "ssd1327", 	16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "midas_128x128" },
      { NULL }
    }
  },
  {
    "ssd1327", 	16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "midas_128x128" },
      { NULL }
    }
  },  

  {
    "ssd1329", 	16, 	12, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x96_noname" },
      { NULL }
    }
  },

  
  {
    "ld7032", 	8, 	4, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_011", "", COM_4WSPI,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "60x32" },
      { NULL }
    }
  },
  {
    "ld7032", 	8, 	4, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_ld7032_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "60x32" },
      { NULL }
    }
  },
  {
    "st7920", 	24, 	4, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "p", COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "192x32" },
      { NULL }
    }
  },
  {
    "st7920", 	24, 	4, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "", COM_6800,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "192x32" },
      { NULL }
    }
  },
  {
    "st7920", 	24, 	4, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_st7920_spi", "s", COM_ST7920SPI,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "192x32" },
      { NULL }
    }
  },
  {
    "st7920", 	16, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "p", COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64" },
      { NULL }
    }
  },
  {
    "st7920", 	16, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "", COM_6800,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64" },
      { NULL }
    }
  },
  {
    "st7920", 	16, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_st7920_spi", "s", COM_ST7920SPI,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64" },
      { NULL }
    }
  },
  {
    "ls013b7dh03", 	16, 	16, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_001", "", COM_4WSPI,		/* cad procedure is not required (no DC for this display) so it could be a dummy procedure here */
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x128" },
      { NULL }
    }
  },
  {
    "uc1701", 		13, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogs102" },
      { NULL }
    }
  },
  {
    "uc1701", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "mini12864" },
      { NULL }
    }
  },
  {
    "pcd8544", 		11, 	6, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI,
    "No HW flip", /* is_generate_u8g2_class= */ 1,
    {
      { "84x48" },
      { NULL }
    }
  },
  {
    "pcf8812", 		12, 	9, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI,
    "No HW flip", /* is_generate_u8g2_class= */ 1,
    {
      { "96x65" },
      { NULL }
    }
  },
  {
    "hx1230", 		12, 	9, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI,
    "No HW flip", /* is_generate_u8g2_class= */ 1,
    {
      { "96x68" },
      { NULL }
    }
  },  
  {
    "uc1604", 	24, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx19264" },
      { NULL }
    }
  },  
  {
    "uc1604", 	24, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx19264" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erc24064" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erc24064" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	15, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erc240120" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	15, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erc240120" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "240x128" },
      { NULL }
    }
  },  
  {
    "uc1608", 	30, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "240x128" },
      { NULL }
    }
  },  

  {
    "uc1638", 	20, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "160x128" },
      { NULL }
    }
  },  
  //{
	/* this device requires cd=1 for arguments, not clear whether the u8x8_cad_uc16xx_i2c works */
    //"uc1638", 	20, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    //"", /* is_generate_u8g2_class= */ 1,
    //{
     // { "160x128" },
     // { NULL }
    //}
  //},  
  
  {
    "uc1610", 		20, 	13, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "3W SPI not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogxl160" },
      { NULL }
    }
  },  
  {
    "uc1610", 		20, 	13, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "3W SPI not tested, I2C not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogxl160" },
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogm240" },
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogm240" },
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogxl240" },
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogxl240" },
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "240x160, active high chip select", /* is_generate_u8g2_class= */ 1,
    {
      { "ew50850" },		/* 240x160 */
      { NULL }
    }
  },  
  {
    "uc1611", 	30, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "240x160, active high chip select", /* is_generate_u8g2_class= */ 1,
    {
      { "ew50850" },		/* 240x160 */
      { NULL }
    }
  },  
  
#ifdef NOTUSED
  {
    "uc1617", 	16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx128128" },
      { NULL }
    }
  },  
  {
    "uc1617", 	16, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx128128" },
      { NULL }
    }
  },  
#endif
  
  {
    "st7565", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogm128" },
      { "64128n" },       
      { "zolen_128x64" },
      { "lm6059" },
      { "lx12864" },
      { "erc12864" },
      { "nhd_c12864" },
      { "jlx12864" },
      { NULL }
    },
  },
  {
    "st7565", 		16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_c12832" },
      { NULL }
    }
  },

  {
    "uc1601", 		16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32" },
      { NULL }
    }
  },
  {
    "uc1601", 	16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_uc16xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x32" },
      { NULL }
    }
  },  
  
  {
    "st7565", 		17, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "ea_dogm132" },
      { NULL }
    }
  },
  {
    "st7567", 		17, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "pi_132x64" },
      { NULL }
    }
  },

  {
    "st7567", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx12864" },
      { NULL }
    }
  },
  
  {
    "st7588", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx12864" },
      { NULL }
    },
  },
  {  /* the ST7588 has the same I2C protocol as the SSD13xx */
    "st7588", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_ssd13xx_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx12864" },
      { NULL }
    }
  },  

  {
    "st75256", 		32, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx256128" },
      { NULL }
    },
  },
  /* the ST75256 has the same I2C protocol as the SSD13xx, BUT: for arguments have the data bit set!!!! */
  /* this means, we need to implement a u8x8_cad_ssd13xx_i2c procedure with cad 011 functionality */
  /* done: u8x8_cad_st75256_i2c */
  {  
    "st75256", 	32, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_st75256_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx256128" },
      { NULL }
    }
  },  

 {
    "st75256", 		32, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx256160" },
      { NULL }
    },
  },
  /* the ST75256 has the same I2C protocol as the SSD13xx, BUT: for arguments have the data bit set!!!! */
  /* this means, we need to implement a u8x8_cad_ssd13xx_i2c procedure with cad 011 functionality */
  /* done: u8x8_cad_st75256_i2c */
  {  
    "st75256", 	32, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_st75256_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx256160" },
      { NULL }
    }
  },  

  
  {
    "st75256", 		30, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx240160" },
      { NULL }
    },
  },
  {  
    "st75256", 	30, 	20, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_st75256_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx240160" },
      { NULL }
    }
  },  

  {
    "st75256", 		32, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx25664" },
      { NULL }
    },
  },
  {  
    "st75256", 	32, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_st75256_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx25664" },
      { NULL }
    }
  },  
  
  {
    "st75256", 		22, 	13, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx172104" },
      { NULL }
    },
  },
  
  {
    "st75256", 		22, 	13, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_st75256_i2c", "i2c", COM_I2C,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "jlx172104" },
      { NULL }
    },
  },

  {
    "nt7534", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "tg12864r" },
      { NULL }
    }
  },
  {
    "ist3020", 		24, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_4WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erc19264" },
      { NULL }
    }
  },

  {
    "sbn1661", 		16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_SED1520,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "122x32" },
      { NULL }
    },
  },
  {
    "sed1520", 		16, 	4, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_SED1520,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "122x32" },
      { NULL }
    },
  },
  
  {
    "ks0108", 		16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_KS0108,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64" },
      { NULL }
    },
  },
  {
    "ks0108", 		24, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_001", "", COM_KS0108,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "erm19264" },
      { NULL }
    },
  },
  {
    "lc7981", 	20, 	10, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800,
    "U8x8 not supported, no powerdown, no HW flip, no constrast", /* is_generate_u8g2_class= */ 1,
    {
      { "160x80" },
      { NULL }
    }
  },
  {
    "lc7981", 	20, 	20, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800,
    "U8x8 not supported, no powerdown, no HW flip, no constrast", /* is_generate_u8g2_class= */ 1,
    {
      { "160x160" },
      { NULL }
    }
  },
  {
    "lc7981", 	30, 	16, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800,
    "U8x8 not supported, no powerdown, no HW flip, no constrast", /* is_generate_u8g2_class= */ 1,
    {
      { "240x128" },
      { NULL }
    }
  },
  
  {
    "t6963", 	30, 	16, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "240x128" },
      { NULL }
    }
  },
  {
    "t6963", 	30, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "240x64" },
      { NULL }
    }
  },
  {
    "t6963", 	32, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "256x64" },
      { NULL }
    }
  },
  {
    "t6963", 	16, 	8, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_8080,
    "Not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "128x64" },
      { NULL }
    }
  },
  {
    "ssd1322", 	32, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "Requires U8G2_16BIT (see u8g2.h)", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_256x64" },
      { NULL }
    }
  },
  {
    "ssd1322", 	16, 	8, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI|COM_6800|COM_8080,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_128x64" },
      { NULL }
    }
  },
  {
    "ssd1606", 	22, 	9, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI,
    "Partly supported by U8x8, no HW flip, no contrast setting", /* is_generate_u8g2_class= */ 1,
    {
      { "172x72" },
      { NULL }
    }
  },
  {
    "ssd1607", 	25, 	25, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI,
    "Partly supported by U8x8, no HW flip, no contrast setting, v2 includes an optimized LUT", /* is_generate_u8g2_class= */ 1,
    {
      { "200x200" },
      { "gd_200x200" },	// GDEP015OC1
      //{ "v2_200x200" },
      { NULL }
    }
  },
  {
    "il3820", 	37, 	16, 	"u8g2_ll_hvline_vertical_top_lsb", "u8x8_cad_011", "", COM_4WSPI|COM_3WSPI,
    "Partly supported by U8x8, no HW flip, no contrast setting, V2 produces lesser screen-flicker", /* is_generate_u8g2_class= */ 1,
    {
      { "296x128" },
      { "v2_296x128" },
      { NULL }
    }
  },
  {
    "sed1330", 	30, 	16, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800|COM_8080,
    "Not tested, might work for RA8835 and SED1335 also", /* is_generate_u8g2_class= */ 1,
    {
      { "240x128" },
      { NULL }
    }
  },
  {
    "ra8835", 	30, 	16, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800|COM_8080,
    "Tested with RA8835", /* is_generate_u8g2_class= */ 1,
    {
      { "nhd_240x128" },
      { NULL }
    }
  },
  {
    "ra8835", 	40, 	30, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_100", "", COM_6800|COM_8080,
    "not tested", /* is_generate_u8g2_class= */ 1,
    {
      { "320x240" },
      { NULL }
    }
  },
  {
    "max7219", 	4, 	1, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_empty", "", COM_4WSPI,
    "", /* is_generate_u8g2_class= */ 1,
    {
      { "32x8" },
      { NULL }
    }
  },
  {
    "a2printer", 	48, 	30, 	"u8g2_ll_hvline_horizontal_right_lsb", "u8x8_cad_empty", "", COM_UART,
    "", /* is_generate_u8g2_class= */ 0,
    {
      { "384x240" },
      { NULL }
    }
  }
};

struct interface interface_list[] = 
{
  /* 0 */
  {
    "4W_SW_SPI",
    "u8x8_SetPin_4Wire_SW_SPI",
    "u8x8_byte_arduino_4wire_sw_spi",	/* improved version over u8x8_byte_4wire_sw_spi */
    "u8x8_gpio_and_delay_arduino",
    "uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE",
    "clock, data, cs, dc, reset",
    "clock, data, cs, dc [, reset]",
    "u8x8_byte_4wire_sw_spi"
  },
  /* 1 */
  {
    "4W_HW_SPI",
    "u8x8_SetPin_4Wire_HW_SPI",
    "u8x8_byte_arduino_hw_spi",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE",
    "cs, dc, reset",
    "cs, dc [, reset]",
    "uC specific"
  },  
  /* 2 */
  {
    "6800",
    "u8x8_SetPin_8Bit_6800",
    "u8x8_byte_8bit_6800mode",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t enable, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc, reset",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc [, reset]",
    "u8x8_byte_8bit_6800mode"
  },
  /* 3 */
  {
    "8080",
    "u8x8_SetPin_8Bit_8080",
    "u8x8_byte_arduino_8bit_8080mode",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t enable, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc, reset",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, cs, dc [, reset]",
    "u8x8_byte_8bit_8080mode"
  },
  /* 4 */
  {
    "3W_SW_SPI",
    "u8x8_SetPin_3Wire_SW_SPI",
    "u8x8_byte_3wire_sw_spi",
    "u8x8_gpio_and_delay_arduino",
    "uint8_t clock, uint8_t data, uint8_t cs, uint8_t reset = U8X8_PIN_NONE",
    "clock, data, cs, reset",
    "clock, data, cs [, reset]",
    "u8x8_byte_3wire_sw_spi"
  },
  /* 5 */
  {
    "3W_HW_SPI",
    "",
    "",
    "",   
    "uint8_t cs, uint8_t reset = U8X8_PIN_NONE",
    "cs, reset",
    "cs [, reset]",
    "uC specific"
  },  
  /* 6 */
  {
    "SW_I2C",
    "u8x8_SetPin_SW_I2C",
    "u8x8_byte_sw_i2c" /* "u8x8_byte_ssd13xx_sw_i2c" */,
    "u8x8_gpio_and_delay_arduino",
    "uint8_t clock, uint8_t data, uint8_t reset = U8X8_PIN_NONE",
    "clock,  data,  reset",
    "clock,  data [,  reset]",
    "u8x8_byte_sw_i2c" /* "u8x8_byte_ssd13xx_sw_i2c" */
  },
  /* 7 */
  {
    "HW_I2C",
    "u8x8_SetPin_HW_I2C",
    "u8x8_byte_arduino_hw_i2c",
    "u8x8_gpio_and_delay_arduino",
    "uint8_t reset = U8X8_PIN_NONE, uint8_t clock = U8X8_PIN_NONE, uint8_t data = U8X8_PIN_NONE",
    "reset, clock, data",
    "[reset [, clock, data]]",
    "uC specific"
  },  
  /* 8 */
  {
    /* ST7920 */ "SW_SPI",
    "u8x8_SetPin_3Wire_SW_SPI",		/* use the 3 wire interface, because the DC is not used */
    "u8x8_byte_arduino_4wire_sw_spi",	/* improved version over u8x8_byte_4wire_sw_spi */
    "u8x8_gpio_and_delay_arduino",
    "uint8_t clock, uint8_t data, uint8_t cs, uint8_t reset = U8X8_PIN_NONE",
    "clock, data, cs, reset",
    "clock, data, cs [, reset]",
    "u8x8_byte_4wire_sw_spi", /* "u8x8_byte_st7920_sw_spi" */
  },
  /* 9 */
  {
    /* ST7920 */ "HW_SPI",
    "u8x8_SetPin_ST7920_HW_SPI",
    "u8x8_byte_arduino_hw_spi",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t cs, uint8_t reset = U8X8_PIN_NONE",
    "cs, reset",
    "cs [, reset]",
    "uC specific"
  },  
  /* 10 */
  {
    "2ND_HW_I2C",
    "u8x8_SetPin_HW_I2C",
    "u8x8_byte_arduino_2nd_hw_i2c",
    "u8x8_gpio_and_delay_arduino",
    "uint8_t reset = U8X8_PIN_NONE",
    "reset",
    "[reset]",
    "uC specific"
  },  
  /* 11 */
  {
    "",
    "u8x8_SetPin_KS0108",
    "u8x8_byte_arduino_ks0108",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t enable, uint8_t dc, uint8_t cs0, uint8_t cs1, uint8_t cs2, uint8_t reset = U8X8_PIN_NONE",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, dc, cs0, cs1, cs2, reset",
    "d0, d1, d2, d3, d4, d5, d6, d7, enable, dc, cs0, cs1, cs2 [, reset]",
    "u8x8_byte_ks0108"
  },
  /* 12 */
  {
    "2ND_4W_HW_SPI",
    "u8x8_SetPin_4Wire_HW_SPI",
    "u8x8_byte_arduino_2nd_hw_spi",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE",
    "cs, dc, reset",
    "cs, dc [, reset]",
    "uC specific"
  },  
  /* 13 */
  {
    "",
    "u8x8_SetPin_SED1520",
    "u8x8_byte_sed1520",
    "u8x8_gpio_and_delay_arduino",   
    "uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t dc, uint8_t e1, uint8_t e2, uint8_t reset",
    "d0, d1, d2, d3, d4, d5, d6, d7, dc, e1, e2, reset",
    "d0, d1, d2, d3, d4, d5, d6, d7, dc, e1, e2, reset",
    "u8x8_byte_sed1520"
  },  
  

  
};


/*===========================================*/

#define STR_MAX 1024
char *str_list[STR_MAX];
int str_cnt = 0;

int str_exists(const char *s)
{
  int i;
  for( i = 0; i < str_cnt; i++ )
  {
    if ( strcmp(str_list[i], s) == 0 )
      return 1;
  }
  return 0;
}

void str_add(const char *s)
{
  if ( str_exists(s) )
    return;
  if ( str_cnt >= STR_MAX )
  {
    error(1,0, "max number of strings reached");
  }
  else
  {
    str_list[str_cnt] = strdup(s);
    str_cnt++;
  }
}


/*===========================================*/

/*
  copy file from source_file_name to dest_file_name
*/
int file_copy(const char *source_file_name, const char *dest_file_name)
{
  int ch;
  FILE *source_fp;
  FILE *dest_fp;
  
  source_fp = fopen(source_file_name, "r");
  dest_fp = fopen(dest_file_name, "w");
  
  if ( source_fp == NULL || dest_fp == NULL )
    return 0;
  
  while( ( ch = fgetc(source_fp) ) != EOF )
    fputc(ch, dest_fp);
  
  fclose(source_fp);
  fclose(dest_fp);
  
  return 1;
}


/*
  Insert file "insertname" between lines "start_line" and "end_line" of file "filename"
*/
int insert_into_file(const char *filename, const char *insertname, const char *start_line, const char *end_line)
{
  int ch;
  static char line[1024*4];
  const char *tmpname = "tmp.h";
  FILE *source_fp;
  FILE *dest_fp;
  FILE *insert_fp;
  
  if ( file_copy(filename, tmpname) == 0 )
    return 0;

  source_fp = fopen(tmpname, "r");
  dest_fp = fopen(filename, "w");
  insert_fp = fopen(insertname, "r");

  if ( source_fp == NULL || dest_fp == NULL || insert_fp == NULL )
    return 0;
  
  for(;;)
  {
    if ( fgets(line, 1024*4, source_fp) == NULL )
      break;
    if ( strncmp(line, start_line, strlen(start_line)) == 0 )
    {
      fputs(line, dest_fp);

      while( ( ch = fgetc(insert_fp) ) != EOF )
	fputc(ch, dest_fp);
      
      fputs("\n", dest_fp);
      
      for(;;)
      {
	if ( fgets(line, 1024*4, source_fp) == NULL )
	  break;
	if ( strncmp(line, end_line, strlen(end_line)) == 0 )
	{
	  fputs(line, dest_fp);
	  break;
	}
      }
    }
    else
    {
      fputs(line, dest_fp);
    }
  }

  fclose(insert_fp);
  fclose(source_fp);
  fclose(dest_fp);
  
  unlink(tmpname);
  
  printf("patched %s\n", filename);

  return 1;
}

/*===========================================*/

char *strlowercase(const char *s)
{
  int i, len = strlen(s);
  static char buf[1024];
  for( i = 0; i <= len; i++ )
    buf[i] = tolower(s[i]);
  return buf;
}

char *struppercase(const char *s)
{
  int i, len = strlen(s);
  static char buf[1024];
  for( i = 0; i <= len; i++ )
    buf[i] = toupper(s[i]);
  return buf;
}

/*===========================================*/

FILE *buf_code_fp;
FILE *buf_header_fp;
FILE *setup_code_fp;
FILE *setup_header_fp;
FILE *u8g2_cpp_header_fp;
FILE *u8x8_cpp_header_fp;

FILE *u8x8_setup_c_md_fp;
FILE *u8x8_setup_cpp_md_fp;
FILE *u8g2_setup_c_md_fp;
FILE *u8g2_setup_cpp_md_fp;

const char *get_setup_function_name(int controller_idx, int display_idx, const char *postfix)
{
  static char s[1024];
  strcpy(s, "u8g2_Setup_");
  strcat(s, strlowercase(controller_list[controller_idx].name));
  strcat(s, "_");  
  if ( controller_list[controller_idx].cad_shortname[0] != '\0' )
  {
    strcat(s, strlowercase(controller_list[controller_idx].cad_shortname));
    strcat(s, "_");
  }
  strcat(s, strlowercase(controller_list[controller_idx].display_list[display_idx].name));
  strcat(s, "_");
  strcat(s, postfix);
  return s;
}

void do_setup_prototype(FILE *fp, int controller_idx, int display_idx, const char *postfix)
{
  /*
  fprintf(fp, "void u8g2_Setup_");
  fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].name));
  fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(fp, "%s(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)", postfix);
  */
  fprintf(fp, "void %s", get_setup_function_name(controller_idx, display_idx, postfix));
  fprintf(fp, "(u8g2_t *u8g2, const u8g2_cb_t *rotation, u8x8_msg_cb byte_cb, u8x8_msg_cb gpio_and_delay_cb)");
}

/*===========================================*/

/*
class U8X8_SSD1306_128X64_4W_SW_SPI : public U8X8 {
  public: U8X8_SSD1306_128X64_4W_SW_SPI(uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE) : U8X8() {
    u8x8_Setup(getU8x8(), u8x8_d_ssd1306_128x64_noname, u8x8_cad_001, u8x8_byte_4wire_sw_spi, u8x8_gpio_and_delay_arduino);
    u8x8_SetPin_4Wire_SW_SPI(getU8x8(), clock, data, cs, dc, reset);
  }
}
*/
void do_u8x8_header(int controller_idx, int display_idx, int interface_idx)
{
  FILE *fp = u8x8_cpp_header_fp;
  fprintf(fp, "class U8X8_");
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
  fprintf(fp, "%s", struppercase(controller_list[controller_idx].display_list[display_idx].name));
  if ( interface_list[interface_idx].interface_name[0] != '\0' )
    fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));
  fprintf(fp, " : public U8X8 {\n");
  fprintf(fp, "  public: ");
  fprintf(fp, "U8X8_");
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
  fprintf(fp, "%s", struppercase(controller_list[controller_idx].display_list[display_idx].name));
  if ( interface_list[interface_idx].interface_name[0] != '\0' )
    fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));
  fprintf(fp, "(%s) : U8X8() {\n", interface_list[interface_idx].pins_with_type);
  fprintf(fp, "    ");
  fprintf(fp, "u8x8_Setup(getU8x8(), u8x8_d_");
  fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].name));
  fprintf(fp, "%s, ", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(fp, "%s, ", strlowercase(controller_list[controller_idx].cad));
  fprintf(fp, "%s, ", interface_list[interface_idx].arduino_com_procedure);  
  fprintf(fp, "%s);\n", interface_list[interface_idx].arduino_gpio_procedure);  
  fprintf(fp, "    ");
  fprintf(fp, "%s(getU8x8(), ", interface_list[interface_idx].setpin_function);  
  fprintf(fp, "%s);\n", interface_list[interface_idx].pins_plain);
  fprintf(fp, "  }\n");
  fprintf(fp, "};\n");
  
}

/*
class U8G2_SSD1306_128x64_NONAME_1_SW_SPI : public U8G2
{
  public:
    U8G2_SSD1306_128x64_NONAME_1_SW_SPI(const u8g2_cb_t *rotation, uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE)
    {
      u8g2_Setup_ssd1306_128x64_noname_1(&u8g2, rotation, u8x8_byte_4wire_sw_spi, u8x8_gpio_and_delay_arduino,);
      u8x8_SetPin_4Wire_SW_SPI(getU8x8(), clock, data, cs, dc, reset);
    }
};
*/


void do_display_interface(int controller_idx, int display_idx, const char *postfix, int interface_idx)
{
  
  FILE *fp = u8g2_cpp_header_fp;
  
  printf("  %s %s", postfix, interface_list[interface_idx].interface_name);
  
  fprintf(fp, "class U8G2_");
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(fp, "%s", struppercase(postfix));
  if ( interface_list[interface_idx].interface_name[0] != '\0' )
    fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));  
  fprintf(fp, " : public U8G2 {\n");
  fprintf(fp, "  public: ");
  fprintf(fp, "U8G2_");
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
  fprintf(fp, "%s_", struppercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(fp, "%s", struppercase(postfix));
  if ( interface_list[interface_idx].interface_name[0] != '\0' )
    fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));  
  fprintf(fp, "(const u8g2_cb_t *rotation, ");
  fprintf(fp, "%s) : U8G2() {\n", interface_list[interface_idx].pins_with_type);
  fprintf(fp, "    ");
  /*
  fprintf(fp, "u8g2_Setup_");
  fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].name));
  fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(fp, "%s", postfix);
  */
  fprintf(fp, "%s", get_setup_function_name(controller_idx, display_idx, postfix));
  fprintf(fp, "(&u8g2, ");
  fprintf(fp, "rotation, ");
  fprintf(fp, "%s, ", interface_list[interface_idx].arduino_com_procedure);  
  fprintf(fp, "%s);\n", interface_list[interface_idx].arduino_gpio_procedure);  
  fprintf(fp, "    ");
  fprintf(fp, "%s(getU8x8(), ", interface_list[interface_idx].setpin_function);  
  fprintf(fp, "%s);\n", interface_list[interface_idx].pins_plain);
  fprintf(fp, "  }\n");
  fprintf(fp, "};\n");
  
  if ( strcmp(postfix, "1") == 0 )
    do_u8x8_header(controller_idx, display_idx, interface_idx);
  
}



/*===========================================*/

void do_display(int controller_idx, int display_idx, const char *postfix)
{
  
  do_setup_prototype(setup_header_fp, controller_idx, display_idx, postfix);
  fprintf(setup_header_fp, ";\n");
  
  do_setup_prototype(setup_code_fp, controller_idx, display_idx, postfix);
  fprintf(setup_code_fp, "\n");
  fprintf(setup_code_fp, "{\n");
  fprintf(setup_code_fp, "  uint8_t tile_buf_height;\n");
  fprintf(setup_code_fp, "  uint8_t *buf;\n");
  fprintf(setup_code_fp, "  u8g2_SetupDisplay(u8g2, u8x8_d_");
  fprintf(setup_code_fp, "%s_", strlowercase(controller_list[controller_idx].name));
  fprintf(setup_code_fp, "%s, ", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
  fprintf(setup_code_fp, "%s, ", controller_list[controller_idx].cad);
  fprintf(setup_code_fp, "byte_cb, gpio_and_delay_cb);\n");    
  fprintf(setup_code_fp, "  buf = ");
  //fprintf(setup_code_fp, "u8g2_m_%s_%d_%s(&tile_buf_height);\n", strlowercase(controller_list[controller_idx].name), controller_list[controller_idx].tile_width, postfix);
  fprintf(setup_code_fp, "u8g2_m_%d_%d_%s(&tile_buf_height);\n", controller_list[controller_idx].tile_width, controller_list[controller_idx].tile_height, postfix);
  fprintf(setup_code_fp, "  u8g2_SetupBuffer(u8g2, buf, tile_buf_height, %s, rotation);\n", controller_list[controller_idx].ll_hvline);
  fprintf(setup_code_fp, "}\n");
  
  /* generate interfaces for this display */
  if ( controller_list[controller_idx].com & COM_4WSPI )
  {
    do_display_interface(controller_idx, display_idx, postfix, 0);		/* SW SPI */
    do_display_interface(controller_idx, display_idx, postfix, 1);		/* HW SPI */
    do_display_interface(controller_idx, display_idx, postfix, 12);	/* 2nd HW SPI */
  }
  if ( controller_list[controller_idx].com & COM_3WSPI )
  {
    do_display_interface(controller_idx, display_idx, postfix, 4);		/* 3wire SW SPI */
    //do_display_interface(controller_idx, display_idx, postfix, 5);		/* 3wire HW SPI (not implemented) */
  }
  if ( controller_list[controller_idx].com & COM_6800 )
  {
    do_display_interface(controller_idx, display_idx, postfix, 2);		/* 6800 mode */    
  }
  if ( controller_list[controller_idx].com & COM_8080 )
  {
    do_display_interface(controller_idx, display_idx, postfix, 3);		/* 8080 mode */    
  }
  if ( controller_list[controller_idx].com & COM_I2C )
  {
    do_display_interface(controller_idx, display_idx, postfix, 6);		/* SW I2C */
    do_display_interface(controller_idx, display_idx, postfix, 7);		/* HW I2C */
    do_display_interface(controller_idx, display_idx, postfix, 10);	/* 2nd HW I2C */
  }
  if ( controller_list[controller_idx].com & COM_ST7920SPI )
  {
    do_display_interface(controller_idx, display_idx, postfix, 8);		/* ST7920 SW SPI */
    do_display_interface(controller_idx, display_idx, postfix, 9);		/* HW SPI (not yet implemented) */
  }
  if ( controller_list[controller_idx].com & COM_UART )
  {
    /* currently there is no Arduino C++ interface, instead the interface is created manually in the example */
  }
  if ( controller_list[controller_idx].com & COM_KS0108 )
  {
    do_display_interface(controller_idx, display_idx, postfix, 11);	/* KS0108 6800 parallel mode */
  }
  if ( controller_list[controller_idx].com & COM_SED1520 )
  {
    do_display_interface(controller_idx, display_idx, postfix, 13);
  }
  
}

void do_controller_buffer_code(int idx, const char *postfix, int buf_len, int rows)
{
  int display_idx;
  char s[1024];
  //sprintf(s, "u8g2_m_%s_%d_%d_%s", strlowercase(controller_list[idx].name), controller_list[idx].tile_width, controller_list[idx].tile_height, postfix);
  /* this should fix #370, additionally the controller is removed (hope that this will not break anything) */
  sprintf(s, "u8g2_m_%d_%d_%s", controller_list[idx].tile_width, controller_list[idx].tile_height, postfix);
  
  if ( str_exists(s) == 0 )
  {
    str_add(s);
    //FILE *fp = stdout;
    fprintf(buf_code_fp, "uint8_t *%s(uint8_t *page_cnt)\n", s);
    fprintf(buf_code_fp, "{\n");
    fprintf(buf_code_fp, "  static uint8_t buf[%d];\n", buf_len);
    fprintf(buf_code_fp, "  *page_cnt = %d;\n", rows);
    fprintf(buf_code_fp, "  return buf;\n");
    fprintf(buf_code_fp, "}\n");
    
    fprintf(buf_header_fp, "uint8_t *%s(uint8_t *page_cnt);\n", s);
  }
  
  display_idx = 0;
  fprintf(setup_code_fp, "/* %s %s */\n", controller_list[idx].name, postfix);
  while( controller_list[idx].display_list[display_idx].name != NULL )
  {
    do_display(idx, display_idx, postfix);
    display_idx++;
  }
  
}



void do_controller_list(void)
{
  int i;
  for( i = 0; i < sizeof(controller_list)/sizeof(*controller_list); i++ )
  {
    printf("%s: ",controller_list[i].name);
    fprintf(setup_code_fp, "/* %s */\n", controller_list[i].name);
    do_controller_buffer_code(i, "1", controller_list[i].tile_width*8, 1);
    do_controller_buffer_code(i, "2", controller_list[i].tile_width*8*2, 2);
    do_controller_buffer_code(i, "f", controller_list[i].tile_width*8*controller_list[i].tile_height, controller_list[i].tile_height);
    printf("\n");
  }
}


int is_arduino_cpp = 1;
int is_u8g2 = 1;
FILE *md_fp;


void do_md_display(int controller_idx, int display_idx)
{
  FILE *fp = md_fp;
  /*
  fprintf(fp, "%s:", controller_list[controller_idx].name);
  fprintf(fp, "%s\n", controller_list[controller_idx].display_list[display_idx].name);
  */

  
  if ( is_u8g2 )
  {
	fprintf(fp, "\n");    
	fprintf(fp, "## %s ", struppercase(controller_list[controller_idx].name));    
	fprintf(fp, "%s", struppercase(controller_list[controller_idx].display_list[display_idx].name));    
	fprintf(fp, "\n");    

    fprintf(fp, "| Controller \"%s\", ", controller_list[controller_idx].name);
    fprintf(fp, "Display \"%s\" | ", controller_list[controller_idx].display_list[display_idx].name);
    fprintf(fp, "Descirption |\n");
    fprintf(fp, "|---|---|\n");
  }
  else
  {
	if ( is_u8g2 != 0 || strcmp(controller_list[controller_idx].ll_hvline, U8X8_HVLINE ) == 0 )
	{

		fprintf(fp, "\n");    
		fprintf(fp, "## %s ", struppercase(controller_list[controller_idx].name));    
		fprintf(fp, "%s", struppercase(controller_list[controller_idx].display_list[display_idx].name));    
		fprintf(fp, "\n");    

		fprintf(fp, "| Controller \"%s\", ", controller_list[controller_idx].name);
		fprintf(fp, "Display \"%s\" |\n", controller_list[controller_idx].display_list[display_idx].name);
		fprintf(fp, "|---|\n");    
	}  
  }
}

void do_md_display_interface_buffer(int controller_idx, int display_idx, int interface_idx, char *postfix, int size, int rows)
{
  FILE *fp = md_fp;
  /*
  fprintf(fp, "%s:", controller_list[controller_idx].name);
  fprintf(fp, "%s:", controller_list[controller_idx].display_list[display_idx].name);
  fprintf(fp, "%s:", prefix);
  fprintf(fp, "%s\n", interface_list[interface_idx].interface_name);
  */
  if ( is_u8g2 ) 
  {
    if ( is_arduino_cpp )
    {
      fprintf(fp, "| U8G2_");
      fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
      fprintf(fp, "%s_", struppercase(controller_list[controller_idx].display_list[display_idx].name));
      fprintf(fp, "%s", struppercase(postfix));
      if ( interface_list[interface_idx].interface_name[0] != '\0' )
	fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));
      fprintf(fp, "(rotation, %s)", interface_list[interface_idx].pins_md_plain);
      if ( postfix[0] == 'f' )
      {
	fprintf(fp, " | full framebuffer, size = %d bytes |\n", size);
      }
      else
      {
	fprintf(fp, " | page buffer, size = %d bytes |\n", size);
      }
    }
    else
    {
      fprintf(fp, "| ");
      /*
      fprintf(fp, "u8g2_Setup_");
      fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].name));
      fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
      fprintf(fp, "%s", strlowercase(postfix));
      */
      fprintf(fp, "%s", get_setup_function_name(controller_idx, display_idx, postfix));
      fprintf(fp, "(u8g2, ");
      fprintf(fp, "rotation, ");
      fprintf(fp, "%s, ", interface_list[interface_idx].generic_com_procedure);  
      fprintf(fp, "%s)", "uC specific");  
      if ( postfix[0] == 'f' )
      {
	fprintf(fp, " | full framebuffer, size = %d bytes |\n", size);
      }
      else
      {
	fprintf(fp, " | page buffer, size = %d bytes |\n", size);
      }
    }
  }
  
}

void do_md_display_interface(int controller_idx, int display_idx, int interface_idx)
{
  FILE *fp = md_fp;
  
  if ( !is_u8g2 ) 
  {
    if ( strcmp(controller_list[controller_idx].ll_hvline, U8X8_HVLINE ) == 0 )
    {
	    if ( is_arduino_cpp )
	    {
	      fprintf(fp, "| U8X8_");
	      fprintf(fp, "%s_", struppercase(controller_list[controller_idx].name));
	      fprintf(fp, "%s", struppercase(controller_list[controller_idx].display_list[display_idx].name));
	      if ( interface_list[interface_idx].interface_name[0] != '\0' )
		fprintf(fp, "_%s", struppercase(interface_list[interface_idx].interface_name));
	      fprintf(fp, "(%s) |\n", interface_list[interface_idx].pins_md_plain);
	    }
	    else
	    {
	      
	      fprintf(fp, "| u8x8_Setup(u8x8_d_");
	      fprintf(fp, "%s_", strlowercase(controller_list[controller_idx].name));
	      fprintf(fp, "%s, ", strlowercase(controller_list[controller_idx].display_list[display_idx].name));
	      fprintf(fp, "%s, ", strlowercase(controller_list[controller_idx].cad));
	      fprintf(fp, "%s, ", strlowercase(interface_list[interface_idx].generic_com_procedure));
	      fprintf(fp, "uC specific) |\n");
	    }  
    }
  }
  do_md_display_interface_buffer(controller_idx, display_idx, interface_idx, "1", controller_list[controller_idx].tile_width*8, 1);
  do_md_display_interface_buffer(controller_idx, display_idx, interface_idx, "2", controller_list[controller_idx].tile_width*8*2, 2);
  do_md_display_interface_buffer(controller_idx, display_idx, interface_idx, "f", controller_list[controller_idx].tile_width*8*controller_list[controller_idx].tile_height, controller_list[controller_idx].tile_height);
  
}

void do_md_controller_list(void)
{
  int controller_idx, display_idx;
  
  for( controller_idx = 0; controller_idx < sizeof(controller_list)/sizeof(*controller_list); controller_idx++ )
  {

    display_idx = 0;
    while( controller_list[controller_idx].display_list[display_idx].name != NULL )
    {
      do_md_display(controller_idx, display_idx);
      /* generate interfaces for this display */
      if ( controller_list[controller_idx].com & COM_4WSPI )
      {
	do_md_display_interface(controller_idx, display_idx, 0);		/* SW SPI */
	if ( is_arduino_cpp )
	{
	  do_md_display_interface(controller_idx, display_idx, 1);		/* HW SPI */
	  do_md_display_interface(controller_idx, display_idx, 12);	/* 2nd HW SPI */
	}
      }
      if ( controller_list[controller_idx].com & COM_3WSPI )
      {
	do_md_display_interface(controller_idx, display_idx, 4);		/* 3wire SW SPI */
	//do_md_display_interface(controller_idx, display_idx, 5);		/* 3wire HW SPI (not implemented) */
      }
      if ( controller_list[controller_idx].com & COM_6800 )
      {
	do_md_display_interface(controller_idx, display_idx, 2);		/* 6800 mode */    
      }
      if ( controller_list[controller_idx].com & COM_8080 )
      {
	do_md_display_interface(controller_idx, display_idx, 3);		/* 8080 mode */    
      }
      if ( controller_list[controller_idx].com & COM_I2C )
      {
	do_md_display_interface(controller_idx, display_idx, 6);		/* SW I2C */
	do_md_display_interface(controller_idx, display_idx, 7);		/* HW I2C  */
	do_md_display_interface(controller_idx, display_idx, 10);		/* 2nd HW I2C  */
      }
      if ( controller_list[controller_idx].com & COM_ST7920SPI )
      {
	do_md_display_interface(controller_idx, display_idx, 8);		/* ST7920 SW SPI */
	do_md_display_interface(controller_idx, display_idx, 9);		/* HW SPI (not yet implemented) */
      }
      if ( controller_list[controller_idx].com & COM_KS0108 )
      {
	do_md_display_interface(controller_idx, display_idx, 11);		/* KS0108 */
      }
      if ( controller_list[controller_idx].com & COM_SED1520 )
      {
	do_md_display_interface(controller_idx, display_idx, 13);		/* SED1520 */
      }
      
      display_idx++;
    }    
  }
}


int main(void)
{
  buf_code_fp = fopen("u8g2_d_memory.c", "w");
  fprintf(buf_code_fp, "/* u8g2_d_memory.c */\n");
  fprintf(buf_code_fp, "/* generated code, codebuild, u8g2 project */\n");
  fprintf(buf_code_fp, "\n");
  fprintf(buf_code_fp, "#include \"u8g2.h\"\n");
  fprintf(buf_code_fp, "\n");
  
  buf_header_fp = fopen("u8g2_memory.h", "w");
  //fprintf(buf_header_fp, "/* start of generated code, codebuild, u8g2 project */\n");

  setup_code_fp = fopen("u8g2_d_setup.c", "w");
  fprintf(setup_code_fp, "/* u8g2_d_setup.c */\n");
  fprintf(setup_code_fp, "/* generated code, codebuild, u8g2 project */\n");
  fprintf(setup_code_fp, "\n");
  fprintf(setup_code_fp, "#include \"u8g2.h\"\n");
  fprintf(setup_code_fp, "\n");

  setup_header_fp = fopen("u8g2_setup.h", "w");
  //fprintf(setup_header_fp, "/* start of generated code, codebuild, u8g2 project */\n");
  
  u8g2_cpp_header_fp = fopen("U8g2lib.h", "w");
  fprintf(u8g2_cpp_header_fp, "/* generated code (codebuild), u8g2 project */\n");
  
  u8x8_cpp_header_fp = fopen("U8x8lib.h", "w");
  fprintf(u8x8_cpp_header_fp, "/* generated code (codebuild), u8g2 project */\n");


  u8x8_setup_c_md_fp = fopen("u8x8setupc.md", "w");
  u8x8_setup_cpp_md_fp = fopen("u8x8setupcpp.md", "w");
  u8g2_setup_c_md_fp = fopen("u8g2setupc.md", "w");
  u8g2_setup_cpp_md_fp = fopen("u8g2setupcpp.md", "w");




  do_controller_list();
  
  
  
  md_fp = u8g2_setup_cpp_md_fp;
  is_arduino_cpp = 1;
  is_u8g2 = 1;
  do_md_controller_list();
  
  md_fp = u8g2_setup_c_md_fp;
  is_arduino_cpp = 0;
  is_u8g2 = 1;
  do_md_controller_list();
  
  md_fp = u8x8_setup_cpp_md_fp;
  is_arduino_cpp = 1;
  is_u8g2 = 0;
  do_md_controller_list();
  
  md_fp = u8x8_setup_c_md_fp;
  is_arduino_cpp = 0;
  is_u8g2 = 0;
  do_md_controller_list();
  
  fprintf(buf_code_fp, "/* end of generated code */\n");
  fclose(buf_code_fp);
  
  //fprintf(buf_header_fp, "/* end of generated code */\n");
  fclose(buf_header_fp);

  fprintf(setup_code_fp, "/* end of generated code */\n");
  fclose(setup_code_fp);

  //fprintf(setup_header_fp, "/* end of generated code */\n");
  fclose(setup_header_fp);

  fclose(u8g2_cpp_header_fp);

  fclose(u8x8_cpp_header_fp);


  fclose(u8x8_setup_c_md_fp);
  fclose(u8x8_setup_cpp_md_fp);
  fclose(u8g2_setup_c_md_fp);
  fclose(u8g2_setup_cpp_md_fp);


  system("cp u8g2_d_memory.c ../../csrc/.");
  puts("generate u8g2_d_memory.c");
  system("cp u8g2_d_setup.c ../../csrc/.");
  puts("generate u8g2_d_setup.c");

  insert_into_file("../../csrc/u8g2.h", "u8g2_memory.h", "/* u8g2_d_memory.c generated code start */", "/* u8g2_d_memory.c generated code end */");
  insert_into_file("../../csrc/u8g2.h", "u8g2_setup.h", "/* u8g2_d_setup.c generated code start */", "/* u8g2_d_setup.c generated code end */");
  insert_into_file("../../cppsrc/U8g2lib.h", "U8g2lib.h", "/* Arduino constructor list start */", "/* Arduino constructor list end */");
  insert_into_file("../../cppsrc/U8x8lib.h", "U8x8lib.h", "// constructor list start", "// constructor list end");

  insert_into_file("../../../u8g2.wiki/u8g2setupc.md", "u8g2setupc.md", "# Setup Function Reference", "# Links");
  insert_into_file("../../../u8g2.wiki/u8g2setupcpp.md", "u8g2setupcpp.md", "# Constructor Reference", "# Links");
  insert_into_file("../../../u8g2.wiki/u8x8setupc.md", "u8x8setupc.md", "# Setup Function Reference", "# Links");
  insert_into_file("../../../u8g2.wiki/u8x8setupcpp.md", "u8x8setupcpp.md", "# Constructor Reference", "# Links");


  return 0;
}

