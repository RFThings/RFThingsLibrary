/*
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
* @@@@@@             ,@@@@@@@@@@@                 @@@@@@
* @@@@@@                  @@@@@@@                 @@@@@@  @@@@@@@@@@@@@@@@@  @@@@              @@@
* @@@@@@                    @@@@@                 @@@@@@        @@@@@        @@@@
* @@@@@@       @@@@@@        @@@@       @@@@@@@@@@@@@@@@        @@@@@        @@@@  @@@@@@      @@@     @@@  @@@@@@        @@@@@@   @@@      @@@@@@@
* @@@@@@       @@@@@@@       @@@@       @@@@@@@@@@@@@@@@        @@@@@        @@@@@@   @@@@     @@@    @@@@@@   @@@@@    @@@@@   @@@@@@   @@@@    @@@@
* @@@@@@                    @@@@@                @@@@@@@        @@@@@        @@@@      @@@@    @@@    @@@@      @@@@   @@@@       @@@@   @@@@@@
* @@@@@@                  @@@@@@@                @@@@@@@        @@@@@        @@@@      @@@@    @@@    @@@@      @@@@   @@@@       @@@@       @@@@@@@@
* @@@@@@       @@        @@@@@@@@       @@@@@@@@@@@@@@@@        @@@@@        @@@@      @@@@    @@@    @@@@      @@@@   @@@@@     @@@@@   @@@      @@@@
* @@@@@@       @@@@       @@@@@@@       @@@@@@@@@@@@@@@@        @@@@@        @@@@      @@@@    @@@    @@@@      @@@@     @@@@@@@@@@@@@    @@@@@@@@@@@
* @@@@@@       @@@@@        @@@@@       @@@@@@@@@@@@@@@@                                                                          @@@@
* @@@@@@       @@@@@@@       @@@@       @@@@@@@@@@@@@@@@                                                                @@@@    #@@@@
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@                                                                  .@@@@@@@
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
*
* Author: m1nhle
*/

#include "Arduino.h"
#include "SPI.h"
#include "lr11xx_hal.h"

lr11xx_hal_status_t lr11xx_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {
    lr11xx_hal_t* lr11xx_hal = ( lr11xx_hal_t* ) context;

    while(digitalRead(lr11xx_hal->busy)) { }

    digitalWrite(lr11xx_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    uint8_t newbuffer[300];
    memcpy(newbuffer, command, command_length);
    memcpy(newbuffer + command_length, data, data_length);

    SPI.transfer(newbuffer, (uint32_t)(command_length + data_length));

    digitalWrite(lr11xx_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
    lr11xx_hal_t* lr11xx_hal = ( lr11xx_hal_t* ) context;

    while(digitalRead(lr11xx_hal->busy)) { }

    digitalWrite(lr11xx_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    /* 1st SPI transaction */
    uint8_t buffer[300];
    memcpy(buffer, command, command_length);
    SPI.transfer(buffer, (uint32_t)command_length);
    
    digitalWrite(lr11xx_hal->nss, HIGH);

    while(digitalRead(lr11xx_hal->busy)) { }

    /* 2nd SPI transaction */
    digitalWrite(lr11xx_hal->nss, LOW);
    SPI.transfer(LR11XX_NOP, 1);
    
    memset(data, LR11XX_NOP, data_length);
    SPI.transfer(data, (uint32_t)data_length);

    digitalWrite(lr11xx_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_direct_read( const void* context, uint8_t* data, const uint16_t data_length ) {
    lr11xx_hal_t* lr11xx_hal = ( lr11xx_hal_t* ) context;

    while(digitalRead(lr11xx_hal->busy)) { }

    digitalWrite(lr11xx_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    memset(data, LR11XX_NOP, data_length);
    SPI.transfer(data, (uint32_t)data_length);

    digitalWrite(lr11xx_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_reset( const void* context ) {
    lr11xx_hal_t* lr11xx_hal = ( lr11xx_hal_t* ) context;

    digitalWrite(lr11xx_hal->reset, LOW);
    delay(5);
    digitalWrite(lr11xx_hal->reset, HIGH);
    delay(5);

    delay(200);

    return LR11XX_HAL_STATUS_OK;
}

lr11xx_hal_status_t lr11xx_hal_wakeup( const void* context ) {
    lr11xx_hal_t* lr11xx_hal = ( lr11xx_hal_t* ) context;

    digitalWrite(lr11xx_hal->nss, HIGH);
    digitalWrite(lr11xx_hal->nss, HIGH);

    while(digitalRead(lr11xx_hal->busy)) { }

    return LR11XX_HAL_STATUS_OK;
}