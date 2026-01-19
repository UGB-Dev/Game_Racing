/*

    PSOne.c

*/
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "PSOne.h"
#include "Comandos.h"

void PSOne_Init(void){
    spi_deinit(PSOne_SPI_PORT);
    spi_init(PSOne_SPI_PORT, 300000); // SPI1 a 250 Kb/s para control PSOne
    spi_set_format(PSOne_SPI_PORT, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_LSB_FIRST);
    gpio_set_function(PSOne_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PSOne_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PSOne_MOSI, GPIO_FUNC_SPI);
    
    gpio_init(PSOne_CS);
    gpio_set_dir(PSOne_CS, GPIO_OUT);

    PSOne_ATT_H;
}

void PSOne_Software_Init(void){

    /*  SET PSOne CONTROLLER */
    gpio_init(PSOne_SCK);
    gpio_set_dir(PSOne_SCK, GPIO_OUT);
    gpio_init(PSOne_MISO);
    gpio_set_dir(PSOne_MISO, GPIO_IN);
    gpio_init(PSOne_MOSI);
    gpio_set_dir(PSOne_MOSI, GPIO_OUT);
    gpio_init(PSOne_CS);
    gpio_set_dir(PSOne_CS, GPIO_OUT);
    
    PSOne_ATT_H;
    PSOne_SCK_H;
    PSOne_CMD_H;
    PSOne_ATT_H;
}


void PSOne_Capturar_Trama(uint8_t *Dat){
    //uint8_t Aux=0;
    PSOne_ATT_L;
    sleep_us(10);
    PSOne_Enviar(Preambulo_SPI);
    sleep_us(10);
    *(++Dat) = Byte_Espejo(PSOne_Recibir(Capture_Mod_SPI)); // Captura el modo actual del control PS1
    sleep_us(10);
    switch (Dat[0]) {
        case PSOne_Mode_Digital:
            for (uint8_t i=0; i<PSOne_NByte_Digital_SPI; i++) {
                *(Dat+i) = Byte_Espejo(PSOne_Recibir(0x00));
                sleep_us(10);
            }
            break;
        
        case PSOne_Mode_Analog:
            for (uint8_t i=0; i<PSOne_NByte_Analog_SPI; i++) {
                *(Dat+i) = Byte_Espejo(PSOne_Recibir(0x00));

                sleep_us(10);
            }
            break;
    }
    
    PSOne_ATT_H;
}

uint8_t Byte_Espejo(uint8_t Data){  // Funcion si no se puede configurar el envio SPI1 en modo LSB
    uint8_t Aux=0;
    for(uint8_t k=0; k<8; k++){
        if(Data & (1<<k)){
            Aux |= 1<<(7-k);
        }
    }
    return Aux;
}

void PSOne_Enviar(uint8_t Data){
    spi_write_blocking(PSOne_SPI_PORT, &Data, 1);
    //sleep_us(10);
}

uint8_t PSOne_Recibir(uint8_t Data){
    uint8_t Buffer_Rx=0;
    spi_read_blocking(PSOne_SPI_PORT, Data, &Buffer_Rx, 1);
    return Buffer_Rx; // Guarda el contenido de DATAR y borra DATAR
}

uint8_t PSOne_Software_Enviar(uint8_t Data){
    uint8_t PSOne_Data_Input = 0;
    for(uint8_t k=0; k<8; k++){
        PSOne_SCK_L;
        (Data & 0x01)? PSOne_CMD_H : PSOne_CMD_L;
        Data>>=1;
        sleep_us(4);
        PSOne_SCK_H;
        sleep_us(1);
        PSOne_Data_Input |= gpio_get(PSOne_MISO) << k;
        sleep_us(3);
    }
    return PSOne_Data_Input;
    //sleep_us(10);
}

void PSOne_Software_Cap_Trama(uint8_t *Dat){
    PSOne_ATT_L;
    sleep_us(10);
    PSOne_Software_Enviar(Preambulo);
    sleep_us(10);
    *(++Dat) = PSOne_Software_Enviar(Capture_Mod);
    sleep_us(10);
    switch (Dat[0]) {
        case PSOne_Mode_Digital:
            for (uint8_t i=0; i<PSOne_NByte_Digital; i++) {
                *(Dat+i) = PSOne_Software_Enviar(0x00);
                sleep_us(10);
            }
            break;
        
        case PSOne_Mode_Analog:
            for (uint8_t i=0; i<PSOne_NByte_Analog; i++) {
                *(Dat+i) = PSOne_Software_Enviar(0x00);
                sleep_us(10);
            }
            break;
    }
    PSOne_ATT_H;
}