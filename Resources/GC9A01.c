#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "stdlib.h"
#include "GC9A01.h"

uint16_t BUFFER_[GC9A01_WIDTH * GC9A01_HEIGHT];

#define GC9A01_CS_H()	gpio_put(LCD_CS, 1)
#define GC9A01_CS_L()	gpio_put(LCD_CS, 0)
#define GC9A01_DC_D()	gpio_put(LCD_DC, 1)
#define GC9A01_DC_C()	gpio_put(LCD_DC, 0)
#define GC9A01_RES_H()	gpio_put(LCD_RST, 1)
#define GC9A01_RES_L()	gpio_put(LCD_RST, 0)

/*
   CADA COMANDO TIENE EL SIGUIENTE FORMATO:
   CUANDO EL COMANDO TIENE ARGUMENTOS:        COMANDO , #ARGUMENTOS, ARGUMENTOS .......
   CUANDO EL COMANDO NO TIENE ARGUMENTOS:     COMANDO , DELAY, TIEMPO EN ms

*/
const uint8_t GC9A01_INIT_CMD[]={
	50,  // 50 Comandos
	GC9A01_INREGEN2, DELAY, 1,  // CMD 1
    0xEB, 1, 0x14,              // CMD 2
	GC9A01_INREGEN1, DELAY, 1,  // CMD 3
	GC9A01_INREGEN2, DELAY, 1,  // CMD 4
	0xEB, 1, 0x14,              // CMD 5
	0x84, 1, 0x40,              // CMD 6
	0x85, 1, 0xFF,              // CMD 7
    0x86, 1, 0xFF,              // CMD 8  
    0x87, 1, 0xFF,              // CMD 9
	0x88, 1, 0x0A,              // CMD 10
	0x89, 1, 0x21,              // CMD 11
	0x8A, 1, 0x00,              // CMD 12
	0x8B, 1, 0x80,              // CMD 13
	0x8C, 1, 0x01,              // CMD 14
	0x8D, 1, 0x01,              // CMD 15
	0x8E, 1, 0xFF,              // CMD 16
	0x8F, 1, 0xFF,              // CMD 17
	GC9A01_FUNCTION_CTRL,       // CMD 18
	2, 0x00, 0x20,
	GC9A01_MADCTL, 1, 0x08,     // CMD 19 
	GC9A01_COLMOD, 1, 0x05,     // CMD 20 DBI (MODO BUS)
	0x90, 4, 0x08, 0x08,        // CMD 21 (no existe el comando en hoja de datos)
	0x08, 0x08,
    0xBD, 1, 0x06,              // CMD 22
    0xBC, 1, 0x00,              // CMD 23
	0xFF, 3, 0x60, 0x01, 0x04,  // CMD 24
	GC9A01_POWER2, 1, 0x13,     // CMD 25
	GC9A01_POWER3, 1, 0x13,     // CMD 26
	GC9A01_POWER4, 1, 0x22,     // CMD 27
	0xBE, 1, 0x11,              // CMD 28
	0xE1, 2, 0x10, 0x0E,        // CMD 29
	0xDF, 3, 0x21, 0x0C, 0x02,  // CMD 30
	GC9A01_GAMMA1, 6, 0x45,     // CMD 31
	0x09, 0x08, 0x08, 0x26,
	0x2A,
	GC9A01_GAMMA2, 6, 0x43,     // CMD 32
	0x70, 0x72, 0x36, 0x37,
	0x6F,
	GC9A01_GAMMA3, 6, 0x45,     // CMD 33
	0x09, 0x08, 0x08, 0x26,
	0x2A,
	GC9A01_GAMMA4, 6, 0x43,     // CMD 34
	0x70, 0x72, 0x36, 0x37,
	0x6F,
	0xED, 2, 0x1B, 0x0B,        // CMD 35
	0xAE, 1, 0x77,              // CMD 36
    0xCD, 1, 0x63,              // CMD 37
	0x70, 9, 0x07, 0x07, 0x04,  // CMD 38
	0x0E, 0x0F, 0x09, 0x07,
	0x08, 0x03,
	GC9A01_FRAMERATE, 1, 0x34,  // CMD 39
	0x62, 12, 0x18, 0x0D, 0x71, // CMD 40
	0xED, 0x70, 0x70, 0x18, 
	0x0F, 0x71, 0xEF, 0x70, 
	0x70,
    0x63, 12, 0x18, 0x11, 0x71, // CMD 41
	0xF1, 0x70, 0x70, 0x18,
	0x13, 0x71, 0xF3, 0x70, 
	0x70,
    0x64, 7, 0x28 ,0x29, 0xF1,  // CMD 42
	0x01, 0xF1, 0x00, 0x07,
	0x66, 10, 0x3C, 0x00, 0xCD, // CMD 43
	0x67, 0x45, 0x45, 0x10,
	0x00, 0x00, 0x00,
	0x67, 10, 0x00, 0x3C, 0x00, // CMD 44
	0x00, 0x00, 0x01, 0x54,
	0x10, 0x32, 0x98,
	0x74, 7, 0x10, 0x85, 0x80,  // CMD 45
	0x00, 0x00, 0x4E, 0x00,
	0x98, 2, 0x3E, 0x07,        // CMD 46
    GC9A01_TEON, DELAY, 1,      // CMD 47
	GC9A01_INVON, DELAY, 1,     // CMD 48
	GC9A01_SLPOUT, DELAY, 120,   // CMD 49
	GC9A01_DISPON, DELAY, 20    // CMD 50 Display ON
};

void GC9A01_Init(void){
    GC9A01_Reset();
    GC9A01_ExecuteCommandList(GC9A01_INIT_CMD);
}

static void GC9A01_Reset(void){
	GC9A01_RES_L();
	sleep_ms(20);
	GC9A01_RES_H();
}

static void GC9A01_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    // column address set
    GC9A01_WriteCommand(GC9A01_CASET);//
    uint8_t data[] = { (x0-1)>>8, (x0-1 ), (x1-1)>>8 , (x1 -1 ) };
    GC9A01_WriteData(data, sizeof(data));

    // row address set
    GC9A01_WriteCommand(GC9A01_RASET);
	uint8_t data1[] = {(y0-1)>>8, (y0-1 ), (y1-1 )>>8 , (y1-1 ) };
    GC9A01_WriteData(data1, sizeof(data1));

    // write to RAM
    GC9A01_WriteCommand(GC9A01_RAMWR);
}

static void GC9A01_ExecuteCommandList(const uint8_t *addr){
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--){
    	uint8_t cmd = *addr++;
        GC9A01_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs){
            GC9A01_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms){
            ms = *addr++;
            if(ms == 255) ms = 500;
            sleep_ms(ms);
        }
    }
}

static void GC9A01_WriteCommand(uint8_t cmd){
	GC9A01_DC_C();
	GC9A01_CS_L();
	spi_write_blocking(GC9A01_SPI_PORT, &cmd, sizeof(cmd));
	GC9A01_CS_H();
}

static void GC9A01_WriteData(uint8_t* buff, size_t buff_size){
	GC9A01_DC_D();
    GC9A01_CS_L();
	spi_write_blocking(GC9A01_SPI_PORT, buff, buff_size);
    GC9A01_CS_H();
}

static void GC9A01_WriteData_(uint8_t Dat){
	GC9A01_DC_D();
    GC9A01_CS_L();
	spi_write_blocking(GC9A01_SPI_PORT, &Dat, sizeof(Dat));
    GC9A01_CS_H();
}

static void GC9A01_DrawPixel(uint16_t x, uint16_t y, uint16_t color){
    if((x >= GC9A01_WIDTH) || (y >= GC9A01_HEIGHT)){
        return;}
    
    GC9A01_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    GC9A01_WriteData(data, sizeof(data));
}

static void GC9A01_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data){
    if((x >= GC9A01_WIDTH) || (y >= GC9A01_HEIGHT)) return;
    if((x + w - 1) >=GC9A01_WIDTH) return;
    if((y + h - 1) >= GC9A01_HEIGHT) return;
     
    GC9A01_SetAddressWindow(x, y, x+w-1, y+h-1);
    GC9A01_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
}

void GC9A01_Print_Pixel(uint8_t X, uint8_t Y, uint16_t Color){
    if ((X>=0 && X <GC9A01_WIDTH) && (Y>=0 && Y<GC9A01_HEIGHT)){
        if(Color != 0xFFFF){ // para la "transparencia" 
            BUFFER_[X+(Y*GC9A01_WIDTH)] = Color;
        }
    }
}

 void GC9A01_Print_Buffer(void){
     GC9A01_DrawImage(0, 0, GC9A01_WIDTH, GC9A01_HEIGHT, BUFFER_);
}

 void GC9A01_Clear_Buffer(const uint16_t* BG){
    for(uint32_t Y=0; Y<GC9A01_WIDTH; Y++){
        for(uint32_t X=0; X<GC9A01_HEIGHT; X++){
            BUFFER_[Y+(X*GC9A01_WIDTH)] = BG[Y+(X*GC9A01_WIDTH)]; 
        }
    }
}

void GC9A01_Set_BG(const uint16_t* BG, uint16_t X_, uint16_t Y_, uint16_t Width, uint16_t Height){
    for(uint32_t Y=0; Y<Height; Y++){
        for(uint32_t X=0; X<Width; X++){
			if(BG[X+(Y*Width)] != 0xFFFF){
				BUFFER_[(X_+X)+((Y+Y_)*GC9A01_WIDTH)] = BG[X+(Y*Width)];
			}     
        }
    }
}

 void GC9A01_Clear_Display(uint16_t BG){
    for(uint32_t Y=0; Y<GC9A01_WIDTH; Y++){
        for(uint32_t X=0; X<GC9A01_HEIGHT; X++){
            BUFFER_[Y+(X*GC9A01_WIDTH)] = BG; 
        }
    }
}

 void GC9A01_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color){
    int dx = abs(X1-X0);
    int dy = -abs(Y1-Y0);
    int sx = (X0 < X1)? 1 : -1;
    int sy = (Y0 < Y1)? 1 : -1;
    int err = dx + dy;

    while (1) {
        GC9A01_Print_Pixel(X0, Y0, Color);
        if (X0 == X1  && Y0 == Y1) break;
        int e2 = 2 * err;
        if(e2 >= dy){ err += dy; X0 += sx; }
        if(e2 <= dx){ err += dx; Y0 += sy; }
    }
}

void  GC9A01_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
     GC9A01_Linea(X, Y, X+Width, Y, Color);
     GC9A01_Linea(X, Y, X, Y+Height, Color);
     GC9A01_Linea(X+Width, Y, X+Width, Y+Height, Color);
     GC9A01_Linea(X, Y+Height, X+Width, Y+Height, Color);
}

void  GC9A01_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
    for(uint32_t Y_Pos = 0; Y_Pos < Height; Y_Pos++){
        for(uint32_t X_Pos = 0; X_Pos < Width; X_Pos++){
             GC9A01_Print_Pixel(X+X_Pos, Y+Y_Pos, Color); 
        }
    }
}
