/*
   ILI9341.h
 
*/

#ifndef ILI9341_H
#define ILI9341_H

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

#define DELAY 0x80

#define ILI9341_SPI_PORT spi0

// Dimensiones
#define ILI9341_WIDTH  240
#define ILI9341_HEIGHT 320

// Colores RGB565
#define	ILI9341_BLACK   0x0000    
#define	ILI9341_GRAY   /*0x8430*/              0x3084
#define	ILI9341_BLUE    /*0x001F */            0x1F00
#define	ILI9341_RED     /*0xF800*/             0x00F8 
#define	ILI9341_GREEN   /*0x07E0*/             0xE007
#define	ILI9341_DARK_GREEN   /*0x01C5*/    0xC501
#define ILI9341_CYAN   /* 0x07FF*/         0xFF07
#define ILI9341_MAGENTA /*0xF81F*/         0x1FF8  
#define ILI9341_YELLOW  /*0xFFE0*/         0xE0FF
#define ILI9341_WHITE   0xFFFE

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left


#define ILI9341_TFTWIDTH 240  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT 320 ///< ILI9341 max TFT height

#define ILI9341_NOP 0x00     ///< No-op register
#define ILI9341_SWRESET 0x01 ///< Software reset register
#define ILI9341_RDDID 0x04   ///< Read display identification information
#define ILI9341_RDDST 0x09   ///< Read Display Status

#define ILI9341_SLPIN 0x10  ///< Enter Sleep Mode
#define ILI9341_SLPOUT 0x11 ///< Sleep Out
#define ILI9341_PTLON 0x12  ///< Partial Mode ON
#define ILI9341_NORON 0x13  ///< Normal Display Mode ON

#define ILI9341_RDMODE 0x0A     ///< Read Display Power Mode
#define ILI9341_RDMADCTL 0x0B   ///< Read Display MADCTL
#define ILI9341_RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT 0x0D   ///< Read Display Image Format
#define ILI9341_RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF 0x20   ///< Display Inversion OFF
#define ILI9341_INVON 0x21    ///< Display Inversion ON
#define ILI9341_GAMMASET 0x26 ///< Gamma Set
#define ILI9341_DISPOFF 0x28  ///< Display OFF
#define ILI9341_DISPON 0x29   ///< Display ON

#define ILI9341_CASET 0x2A ///< Column Address Set
#define ILI9341_PASET 0x2B ///< Page Address Set
#define ILI9341_RAMWR 0x2C ///< Memory Write
#define ILI9341_RAMRD 0x2E ///< Memory Read

#define ILI9341_PTLAR 0x30    ///< Partial Area
#define ILI9341_VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define ILI9341_MADCTL 0x36   ///< Memory Access Control
#define ILI9341_VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1 0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3 0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR 0xB4  ///< Display Inversion Control
#define ILI9341_DFUNCTR 0xB6 ///< Display Function Control

#define ILI9341_PWCTR1 0xC0 ///< Power Control 1
#define ILI9341_PWCTR2 0xC1 ///< Power Control 2
#define ILI9341_PWCTR3 0xC2 ///< Power Control 3
#define ILI9341_PWCTR4 0xC3 ///< Power Control 4
#define ILI9341_PWCTR5 0xC4 ///< Power Control 5
#define ILI9341_VMCTR1 0xC5 ///< VCOM Control 1
#define ILI9341_VMCTR2 0xC7 ///< VCOM Control 2

#define ILI9341_RDID1 0xDA ///< Read ID 1
#define ILI9341_RDID2 0xDB ///< Read ID 2
#define ILI9341_RDID3 0xDC ///< Read ID 3
#define ILI9341_RDID4 0xDD ///< Read ID 4

#define ILI9341_GMCTRP1 0xE0 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1 0xE1 ///< Negative Gamma Correction
//   **************************************************
#define ILI9341_PWCTRA 0xCB ///< Power Control A
#define ILI9341_PWCTRB 0xCF ///< Power Control B
#define ILI9341_DRVTIMCTRA 0xE8 ///< Driver Timing Control A
#define ILI9341_DRVTIMCTRB 0xEA ///< Driver Timing Control B
#define ILI9341_PWONSEQCTR 0xED ///< Power On sequence Control
#define ILI9341_PMPRATIOCTR 0xF7 ///< Pump Ratio Control
#define ILI9341_GAMFUNTDISA 0xF2 ///< Gamma Function Disable

// Funciones principales
void ILI9341_Init(void);
static void ILI9341_Reset(void);
static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
static void ILI9341_ExecuteCommandList(const uint8_t *addr);
static void ILI9341_WriteCommand(uint8_t cmd);
static void ILI9341_WriteData_(uint8_t Dat);
static void ILI9341_WriteData(uint8_t* buff, size_t buff_size);
static void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
static void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data);
void ILI9341_Print_Pixel(uint16_t X, uint16_t Y, uint16_t Color);
void ILI9341_Print_Buffer(void);
void ILI9341_Clear_Buffer(const uint16_t* BG);
void ILI9341_Clear_Display(uint16_t BG);
void ILI9341_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color);
void ILI9341_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
void ILI9341_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color);
void ILI9341_Set_BG(const uint16_t* BG, uint16_t X_, uint16_t Y_, uint16_t Width, uint16_t Height);  
void ILI9341_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint16_t X_Pos_Destino, uint16_t Y_Pos_Destino, const uint16_t* Sprite);


#endif // __ILI9341_H__