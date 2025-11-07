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
#define GC9A01_HEIGHT 320

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
extern uint16_t Buf[];

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
 void GC9A01_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint8_t X_Pos_Destino, uint8_t Y_Pos_Destino, const uint16_t* Sprite);

#endif // __GC9A01_H__