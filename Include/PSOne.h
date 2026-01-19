/*

    PSOne.h

*/

#ifndef __PSOne__
#define __PSOne__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "main.h"

#define PSOne_SPI_PORT spi1

/*          
     ---------- ------ --------- ------- ----- ----------- ----- ----- -----       
    |   PSOne  | Data | Command | Motor | GND | VCC(3.3V) | ATT | CLK | ACK |
     ---------- ---------------- ------- ----- ----------- ----- ----- -----
    | CH32V003 | MISO |   MOSI  |   NC  | GND | VCC(3.3V) | PC0 | CLK | PC1 |
     ---------- ------ ----------------- ----- ----------- ----- ----- -----  
*/

#define PSOne_ATT_H    gpio_put(PSOne_CS, 1)
#define PSOne_ATT_L    gpio_put(PSOne_CS, 0)

/* ETIQUETAS PARA IMPLEMENTACION POR SOFTWARE */
#define PSOne_SCK_H    gpio_put(PSOne_SCK, 1)
#define PSOne_SCK_L    gpio_put(PSOne_SCK, 0)
#define PSOne_DATA_H   gpio_put(PSOne_MISO, 1)
#define PSOne_DATA_L   gpio_put(PSOne_MISO, 0)
#define PSOne_CMD_H    gpio_put(PSOne_MOSI, 1)
#define PSOne_CMD_L    gpio_put(PSOne_MOSI, 0)

void PSOne_Init(void);
void PSOne_Software_Init(void);
uint8_t PSOne_Recibir_Char(uint8_t ADDR);
void PSOne_Capturar_Trama(uint8_t *Dat);
void PSOne_Enviar_MultiData(uint8_t* Dato, uint16_t Longitud);
void PSOne_Enviar(uint8_t Data);
uint8_t PSOne_Recibir(uint8_t Data);
uint8_t PSOne_Software_Enviar(uint8_t Data);
void PSOne_Software_Cap_Trama(uint8_t *Dat);
uint8_t Byte_Espejo(uint8_t Data);

#ifdef __cplusplus
 }
#endif //__PSOne__
#endif