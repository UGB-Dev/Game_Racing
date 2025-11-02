/*
   GC9A01.h
 
 */

#ifndef __GC9A01_H__
#define __GC9A01_H__

/*
    SPI_MASTER.C 
    _______
   |        |--- MOSI (PIN 19)
   |        |--- SCK  (PIN 18)
   |        |--- RST  (PIN 21)
   | RP2040 |--- CS   (PIN 17)
   |        |--- DC   (PIN 20)
   |        | 
   |________|

*/

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

#define GC9A01_SPI_PORT spi0

/* PARAMETROS DE VENTANA */
#define GC9A01_HBACKPORCH 24 // EJE X
#define GC9A01_VBACKPORCH 2 // EJE Y
#define GC9A01_WIDTH 240
#define GC9A01_HEIGHT 240

/* COMANDOS TFT GC9A01 */
#define GC9A01_SWRESET 0x01 /**< Software Reset */
#define GC9A01_RDDID 0x04	/**< Read display identification information */
#define GC9A01_RDDST 0x09	/**< Read Display Status */

#define GC9A01_SLPIN 0x10	/**< Enter Sleep Mode */
#define GC9A01_SLPOUT 0x11	/**< Sleep Out */
#define GC9A01_PTLON 0x12	/**< Partial Mode ON */
#define GC9A01_NORON 0x13	/**< Normal Display Mode ON */
#define GC9A01_DISPOFF 0x28 /**< Display OFF */
#define GC9A01_DISPON 0x29	/**< Display ON */
#define GC9A01_IDLEOFF 0x38 /**< Idle mode OFF */
#define GC9A01_IDLEON 0x39	/**< Idle mode ON */

#define GC9A01_INVOFF 0x20	  /**< Display Inversion OFF */
#define GC9A01_INVON 0x21	  /**< Display Inversion ON */
#define GC9A01_CASET 0x2A	  /**< Column Address Set */
#define GC9A01_RASET 0x2B	  /**< Row Address Set */
#define GC9A01_RAMWR 0x2C	  /**< Memory Write */
#define GC9A01_PTLAR 0x30	  /**< Partial Area */
#define GC9A01_VSCRDEF 0x33	  /**< Vertical Scrolling Definition */
#define GC9A01_TEOFF 0x34	  /**< Tearing Effect Line OFF */
#define GC9A01_TEON 0x35	  /**< Tearing Effect Line ON */
#define GC9A01_MADCTL 0x36	  /**< Memory Access Control */
#define GC9A01_VSCRSADD 0x37  /**< Vertical Scrolling Start Address */
#define GC9A01_COLMOD 0x3A	  /**< Pixel Format Set */
#define GC9A01_CONTINUE 0x3C  /**< Write Memory Continue */
#define GC9A01_TEARSET 0x44	  /**< Set Tear Scanline */
#define GC9A01_GETLINE 0x45	  /**< Get Scanline */
#define GC9A01_SETBRIGHT 0x51 /**< Write Display Brightness */
#define GC9A01_SETCTRL 0x53	  /**< Write CTRL Display */
#define GC9A01_TEWC 0xBA			 /**< Tearing effect width control */
#define GC9A01_BLANK_PORCH_CTRL 0xB5 /**< Blanking Porch Control (B5h) */
#define GC9A01_FUNCTION_CTRL 0xB6	 /**< Display Function Control */
#define GC9A01_POWER1 0xC1 /**< Power Control 1 */
#define GC9A01_POWER2 0xC3 /**< Power Control 2 */
#define GC9A01_POWER3 0xC4 /**< Power Control 3 */
#define GC9A01_POWER4 0xC9 /**< Power Control 4 */
#define GC9A01_POWER7 0xA7 /**< Power Control 7 */
#define GC9A01_RDID1 0xDA /**< Read ID 1 */
#define GC9A01_RDID2 0xDB /**< Read ID 2 */
#define GC9A01_RDID3 0xDC /**< Read ID 3 */
#define GC9A01_FRAMERATE 0xE8 /**< Frame rate control */
#define GC9A01_SPI2DATA 0xE9  /**< SPI 2DATA control */
#define GC9A01_INREGEN2 0xEF  /**< Inter register enable 2 */
#define GC9A01_GAMMA1 0xF0	 /**< Set gamma 1 */
#define GC9A01_GAMMA2 0xF1	 /**< Set gamma 2 */
#define GC9A01_GAMMA3 0xF2	 /**< Set gamma 3 */
#define GC9A01_GAMMA4 0xF3	 /**< Set gamma 4 */
#define GC9A01_IFACE 0xF6	 /**< Interface control */
#define GC9A01_INREGEN1 0xFE /**< Inter register enable 1 */ 


// Color definitions    LITLE-ENDIAN    BIG-ENDIAN
#define	GC9A01_BLACK   0x0000    
#define	GC9A01_GRAY   /*0x8430*/              0x3084
#define	GC9A01_BLUE    /*0x001F */            0x1F00
#define	GC9A01_RED     /*0xF800*/             0x00F8 
#define	GC9A01_GREEN   /*0x07E0*/             0xE007
#define	GC9A01_DARK_GREEN   /*0x01C5*/    0xC501
#define GC9A01_CYAN   /* 0x07FF*/         0xFF07
#define GC9A01_MAGENTA /*0xF81F*/         0x1FF8  
#define GC9A01_YELLOW  /*0xFFE0*/         0xE0FF
#define GC9A01_WHITE   0xFFFE

#define DELAY 0x80
extern uint16_t BUFFER_[];
extern const uint8_t GC9A01_INIT_CMD[];

void GC9A01_Init(void);
static void GC9A01_Reset(void);
static void GC9A01_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void GC9A01_ExecuteCommandList(const uint8_t *addr);
static void GC9A01_WriteCommand(uint8_t cmd);
static void GC9A01_WriteData_(uint8_t Dat);
static void GC9A01_WriteData(uint8_t* buff, size_t buff_size);
static void GC9A01_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
static void GC9A01_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
 void GC9A01_Print_Pixel(uint8_t X, uint8_t Y, uint16_t Color);
 void GC9A01_Print_Buffer(void);
 void GC9A01_Clear_Buffer(const uint16_t* BG);
 void GC9A01_Clear_Display(uint16_t BG);
 void GC9A01_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color);
 void  GC9A01_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
 void GC9A01_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
 void GC9A01_Set_BG(const uint16_t* BG, uint16_t X_, uint16_t Y_, uint16_t Width, uint16_t Height);

#endif // __GC9A01_H__


/*

const uint8_t GC9A01_INIT_CMD[]={
	53,  // 50 Comandos
	GC9A01_INREGEN1, DELAY, 1,  // CMD 1
	GC9A01_INREGEN2, DELAY, 1,  // CMD 2
    0xEB, 1, 0x14,              // CMD 3
	//GC9A01_INREGEN1, DELAY, 1,  // CMD 3
	//GC9A01_INREGEN2, DELAY, 1,  // CMD 4
	//0xEB, 1, 0x14,              // CMD 5
	0x84, 1, 0x60,              // CMD 4
	0x85, 1, 0xF7,              // CMD 5
    0x86, 1, 0xFC,              // CMD 6  
    0x87, 1, 0x28,              // CMD 7
	0x88, 1, 0x0A,              // CMD 8
	0x89, 1, 0x21,              // CMD 9
	0x8A, 1, 0x00,              // CMD 10
	0x8B, 1, 0x80,              // CMD 11
	0x8C, 1, 0x01,              // CMD 12
	0x8D, 1, 0x03,              // CMD 13
	0x8E, 1, 0x0F,              // CMD 14
	0x8F, 1, 0xFC,              // CMD 15
	GC9A01_FUNCTION_CTRL,       // CMD 16
	2, 0x00, 0x20,
	GC9A01_MADCTL, 1, 0x48,     // CMD 17 
	GC9A01_COLMOD, 1, 0x05,     // CMD 18 DBI (MODO BUS)
	0x90, 4, 0x08, 0x08,        // CMD 19 (no existe el comando en hoja de datos)
	0x08, 0x08,
	GC9A01_TEWC, 1, 0x01,       // CMD 20
    0xBD, 1, 0x06,              // CMD 21
    0xBC, 1, 0x00,              // CMD 22
	0xFF, 3, 0x60, 0x01, 0x04,  // CMD 23
	GC9A01_POWER2, 1, 0x48,     // CMD 24
	GC9A01_POWER3, 1, 0x48,     // CMD 25
	GC9A01_POWER4, 1, 0x25,     // CMD 26
	0xBE, 1, 0x11,              // CMD 27
	0xE1, 2, 0x10, 0x0E,        // CMD 28
	0xDF, 3, 0x21, 0x10, 0x02,  // CMD 29
	GC9A01_GAMMA1, 6, 0x4B,     // CMD 30
	0x0F, 0x0A, 0x0B, 0x15,
	0x30,
	GC9A01_GAMMA2, 6, 0x43,     // CMD 31
	0x70, 0x72, 0x36, 0x37,
	0x6F,
	GC9A01_GAMMA3, 6, 0x4B,     // CMD 32
	0x0F, 0x0A, 0x0B, 0x15,
	0x30,
	GC9A01_GAMMA4, 6, 0x43,     // CMD 33
	0x70, 0x72, 0x36, 0x37,
	0x6F,
	0xED, 2, 0x1B, 0x0B,        // CMD 34
	0xAC, 1, 0x47,              // CMD 35
	0xAE, 1, 0x77,              // CMD 36
    0xCD, 1, 0x63,              // CMD 37
	0x70, 9, 0x07, 0x09, 0x04,  // CMD 38
	0x0C, 0x0D, 0x09, 0x07,
	0x08, 0x03,
	GC9A01_FRAMERATE, 1, 0x34,  // CMD 39
	0x60, 8, 0x38, 0x0B, 0x76,  // CMD 40
	0x62,0x39, 0xF0, 0x76, 
	0x62,
	0x61, 8, 0x38, 0xF6, 0x76,  // CMD 41
	0x62, 0x38, 0xF7, 0x76, 
	0x62,
	0x62, 12, 0x38, 0x0D, 0x71, // CMD 42
	0xED, 0x76, 0x62, 0x38, 
	0x0F, 0x71, 0xEF, 0x76, 
	0x62,
    0x63, 12, 0x38, 0x11, 0x71, // CMD 43
	0xF1, 0x76, 0x62, 0x38, 
	0x13, 0x71, 0xF3, 0x76, 
	0x62,
    0x64, 7, 0x3B ,0x29, 0xF1,  // CMD 44
	0x01, 0xF1, 0x00, 0x0A,
	0x66, 10, 0x3C, 0x00, 0xCD, // CMD 45
	0x67, 0x45, 0x45, 0x10,
	0x00, 0x00, 0x00,
	0x67, 10, 0x00, 0x3C, 0x00, // CMD 46
	0x00, 0x00, 0x01, 0x54,
	0x10, 0x32, 0x98,
	GC9A01_BLANK_PORCH_CTRL, 4, //CMD 47
	0x08, 0x09, 0x14, 0x08,
	0x74, 7, 0x10, 0x85, 0x80,  // CMD 48
	0x00, 0x00, 0x4E, 0x00,
	0x98, 2, 0x3E, 0x07,        // CMD 49
    GC9A01_TEON, 1, 0x00,      // CMD 50
	GC9A01_INVON, DELAY, 1,     // CMD 51
	GC9A01_SLPOUT, DELAY, 120,   // CMD 52
	GC9A01_DISPON, DELAY, 20    // CMD 53 Display ON
};
*/