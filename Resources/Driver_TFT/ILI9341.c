#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "stdlib.h"
#include "GC9A01.h"
#include "ILI9341.h"


#define ILI9341_CS_H()	gpio_put(LCD_CS, 1)
#define ILI9341_CS_L()	gpio_put(LCD_CS, 0)
#define ILI9341_DC_D()	gpio_put(LCD_DC, 1)
#define ILI9341_DC_C()	gpio_put(LCD_DC, 0)
#define ILI9341_RES_H()	gpio_put(LCD_RST, 1)
#define ILI9341_RES_L()	gpio_put(LCD_RST, 0)

/*
   CADA COMANDO TIENE EL SIGUIENTE FORMATO:
   CUANDO EL COMANDO TIENE ARGUMENTOS:        COMANDO , #ARGUMENTOS, ARGUMENTOS .......
   CUANDO EL COMANDO NO TIENE ARGUMENTOS:     COMANDO , DELAY, TIEMPO EN ms

*/

const uint8_t ILI9341_INIT_CMD[]={
    20,                          // 20 commands
    ILI9341_PWCTRA, 5, 0x39,     // CMD 1
    0x2C, 0x00, 0x34, 0x02,  
    ILI9341_PWCTRB, 3, 0x00,     // CMD 2
    0xC1, 0x30,  
    ILI9341_DRVTIMCTRA, 3, 0x85, // CMD 3
    0x00, 0x78,
    ILI9341_DRVTIMCTRB, 2, 0x00, // CMD 4
    0x00,         
    ILI9341_PWONSEQCTR, 4, 0x64, // CMD 5
    0x03, 0x12, 0x81, 
    ILI9341_PMPRATIOCTR, 1, 0x20,// CMD 6
    ILI9341_PWCTR1, 1, 0x23,	 // CMD 7 Power control VRH[5:0]
    ILI9341_PWCTR2, 1, 0x10,	 // CMD 8 Power control SAP[2:0];BT[3:0]
    ILI9341_VMCTR1, 2, 0x3E,     // CMD 9 VCM control
    0x28, 
    ILI9341_VMCTR2, 1, 0x86,	 // CMD 10 VCM control2
	ILI9341_MADCTL, 1, 0x48,	 // CMD 11 Memory Access Control
    ILI9341_PIXFMT, 1, 0x55,     // CMD 12
	ILI9341_FRMCTR1, 2, 0x00,    // CMD 13 
    0x18, 
	ILI9341_DFUNCTR, 3, 0x08,    // CMD 14 Display Function Control
    0x82, 0x27,	
    ILI9341_GAMFUNTDISA, 1, 0x00,// CMD 15
    ILI9341_GAMMASET, 1, 0x01,	 // CMD 16 Gamma curve selected
    ILI9341_GMCTRP1, 15, 0x0F,   // CMD 17 Set Gamma
    0x31, 0x2B, 0x0C, 0x0E, 
    0x08, 0x4E, 0xF1, 0x37, 
    0x07, 0x10, 0x03, 0x0E, 
    0x09, 0x00,
	ILI9341_GMCTRN1, 15, 0x00,   // CMD 18 Set Gamma
    0x0E, 0x14, 0x03, 0x11, 
    0x07, 0x31, 0xC1, 0x48, 
    0x08, 0x0F, 0x0C, 0x31, 
    0x36, 0x0F,
    ILI9341_SLPOUT, DELAY, 120,  // CMD 19 Exit Sleep
	ILI9341_DISPON, DELAY, 150   // CMD 20 Display on
};

void ILI9341_Init(void){
    ILI9341_Reset();
    ILI9341_WriteCommand(ILI9341_SWRESET);
    sleep_ms(1000);
    ILI9341_ExecuteCommandList(ILI9341_INIT_CMD);
}

static void ILI9341_Reset(void){
	ILI9341_RES_L();
	sleep_ms(20);
	ILI9341_RES_H();
}

static void ILI9341_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1){
    ILI9341_WriteCommand(ILI9341_CASET);//
    uint8_t data[] = { (x0)>>8, (x0 ), (x1)>>8 , (x1 ) };
    ILI9341_WriteData(data, sizeof(data));

    // row address set
    ILI9341_WriteCommand(ILI9341_PASET);
	uint8_t data1[] = {(y0)>>8, (y0 ), (y1 )>>8 , (y1 ) };
    ILI9341_WriteData(data1, sizeof(data1));

    // write to RAM
    ILI9341_WriteCommand(ILI9341_RAMWR);
}

static void ILI9341_ExecuteCommandList(const uint8_t *addr){
    uint8_t numCommands, numArgs;
    uint16_t ms;

    numCommands = *addr++;
    while(numCommands--){
    	uint8_t cmd = *addr++;
        ILI9341_WriteCommand(cmd);

        numArgs = *addr++;
        // If high bit set, delay follows args
        ms = numArgs & DELAY;
        numArgs &= ~DELAY;
        if(numArgs){
            ILI9341_WriteData((uint8_t*)addr, numArgs);
            addr += numArgs;
        }

        if(ms){
            ms = *addr++;
            if(ms == 255) ms = 500;
            sleep_ms(ms);
        }
    }
}

static void ILI9341_WriteCommand(uint8_t cmd){
	ILI9341_DC_C();
	ILI9341_CS_L();
	spi_write_blocking(ILI9341_SPI_PORT, &cmd, sizeof(cmd));
	ILI9341_CS_H();

}

static void ILI9341_WriteData(uint8_t* buff, size_t buff_size){
	ILI9341_DC_D();
    ILI9341_CS_L();
	spi_write_blocking(ILI9341_SPI_PORT, buff, buff_size);
    ILI9341_CS_H();
}

static void ILI9341_WriteData_(uint8_t Dat){
	ILI9341_DC_D();
    ILI9341_CS_L();
	spi_write_blocking(ILI9341_SPI_PORT, &Dat, sizeof(Dat));
    ILI9341_CS_H();
}

static void ILI9341_DrawPixel(uint16_t x, uint16_t y, uint16_t color){
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)){ return; }
    
    ILI9341_SetAddressWindow(x, y, x+1, y+1);
    uint8_t data[] = { color >> 8, color & 0xFF };
    ILI9341_WriteData(data, sizeof(data));
}

static void ILI9341_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t* data){
    if((x >= ILI9341_WIDTH) || (y >= ILI9341_HEIGHT)) return;
    if((x + w - 1) >=ILI9341_WIDTH) return;
    if((y + h - 1) >= ILI9341_HEIGHT) return;
    
    ILI9341_SetAddressWindow(x, y, x+w-1, y+h-1);
    ILI9341_WriteData((uint8_t*)data, sizeof(uint16_t)*w*h);
}

void ILI9341_Print_Pixel(uint16_t X, uint16_t Y, uint16_t Color){
    if ((X>=0 && X <ILI9341_WIDTH) && (Y>=0 && Y<ILI9341_HEIGHT)){
        if(Color != 0xFFFF){ // para la "transparencia" 
            BUFFER_[X+(Y*ILI9341_WIDTH)] = Color;
        }
    }
}

 void ILI9341_Print_Buffer(void){
     ILI9341_DrawImage(0, 0, ILI9341_WIDTH, ILI9341_HEIGHT, BUFFER_);
}

 void ILI9341_Clear_Buffer(const uint16_t* BG){
    for(uint32_t Y=0; Y<ILI9341_WIDTH; Y++){
        for(uint32_t X=0; X<ILI9341_HEIGHT; X++){
            BUFFER_[Y+(X*ILI9341_WIDTH)] = BG[Y+(X*ILI9341_WIDTH)]; 
        }
    }
}

void ILI9341_Set_BG(const uint16_t* BG, uint16_t X_, uint16_t Y_, uint16_t Width, uint16_t Height){
    for(uint32_t Y=0; Y<Height; Y++){
        for(uint32_t X=0; X<Width; X++){
			if(BG[X+(Y*Width)] != 0xFFFF){
				BUFFER_[(X_+X)+((Y+Y_)*ILI9341_WIDTH)] = BG[X+(Y*Width)];
			}
        }
    }
}

 void ILI9341_Clear_Display(uint16_t BG){
    for(uint32_t Y=0; Y<ILI9341_WIDTH; Y++){
        for(uint32_t X=0; X<ILI9341_HEIGHT; X++){
            BUFFER_[Y+(X*ILI9341_WIDTH)] = BG; 
        }
    }
}

 void ILI9341_Linea(uint16_t X0, uint16_t Y0, uint16_t X1, uint16_t Y1, uint16_t Color){
    int dx = abs(X1-X0);
    int dy = -abs(Y1-Y0);
    int sx = (X0 < X1)? 1 : -1;
    int sy = (Y0 < Y1)? 1 : -1;
    int err = dx + dy;

    while (1) {
        ILI9341_Print_Pixel(X0, Y0, Color);
        if (X0 == X1  && Y0 == Y1) break;
        int e2 = 2 * err;
        if(e2 >= dy){ err += dy; X0 += sx; }
        if(e2 <= dx){ err += dx; Y0 += sy; }
    }
}

void ILI9341_Cuadro(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
     ILI9341_Linea(X, Y, X+Width, Y, Color);
     ILI9341_Linea(X, Y, X, Y+Height, Color);
     ILI9341_Linea(X+Width, Y, X+Width, Y+Height, Color);
     ILI9341_Linea(X, Y+Height, X+Width, Y+Height, Color);
}

void  ILI9341_Cuadro_Solido(uint16_t X, uint16_t Y, uint16_t Width, uint16_t Height, uint16_t Color){
    for(uint32_t Y_Pos = 0; Y_Pos < Height; Y_Pos++){
        for(uint32_t X_Pos = 0; X_Pos < Width; X_Pos++){
             ILI9341_Print_Pixel(X+X_Pos, Y+Y_Pos, Color); 
        }
    }
}

void ILI9341_Print_Sprite_Trans(uint8_t X_Pos, uint8_t Y_Pos, uint16_t X_Width, uint16_t Y_Height, uint16_t X_Pos_Destino, uint16_t Y_Pos_Destino, const uint16_t* Sprite){
    for (uint16_t Y=0; Y < Y_Height; Y++) {
        for (uint16_t X=0; X < X_Width; X++) {
            Buf[X+(Y*X_Width)] = Sprite[(X_Pos+X)+((Y_Pos+Y)*143)]; //Se almacena la ubicacion del sprite
        }
    }

    for(uint32_t Y=0; Y<X_Width; Y++){
        for(uint32_t X=0; X<Y_Height; X++){
            if(Buf[Y+(X*X_Width)] != 0xFFFF){
                ILI9341_Print_Pixel(X_Pos_Destino+Y, Y_Pos_Destino+X, Buf[Y+(X*X_Width)]);
            }
        }
    }
}