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
#include "sx126x_hal.h"

sx126x_hal_status_t sx126x_hal_write( const void* context, const uint8_t* command, const uint16_t command_length,
                                      const uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(digitalRead(sx126x_hal->busy)) { }
    
    digitalWrite(sx126x_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    uint8_t buffer[255];
    memcpy(buffer, command, command_length);
    memcpy(buffer + command_length, data, data_length);

    SPI.transfer(buffer, (uint32_t)(command_length + data_length));

    digitalWrite(sx126x_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_read( const void* context, const uint8_t* command, const uint16_t command_length,
                                     uint8_t* data, const uint16_t data_length ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;

    while(digitalRead(sx126x_hal->busy)) { }

    digitalWrite(sx126x_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    uint8_t buffer[255];
    memcpy(buffer, command, command_length);
    memset(buffer + command_length, SX126X_NOP, data_length);

    SPI.transfer(buffer, (uint32_t)(command_length + data_length));
    memcpy(data, buffer + command_length, data_length);

    digitalWrite(sx126x_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_reset( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    delay(20);
    digitalWrite(sx126x_hal->reset, LOW);
    delay(50);
    digitalWrite(sx126x_hal->reset, HIGH);;
    delay(50);

    return SX126X_HAL_STATUS_OK;
}

sx126x_hal_status_t sx126x_hal_wakeup( const void* context ) {
    sx126x_hal_t* sx126x_hal = ( sx126x_hal_t* ) context;
    
    digitalWrite(sx126x_hal->nss, LOW);
#ifdef SPI_HAS_TRANSACTION
    SPI.beginTransaction(SPISettings(100000, MSBFIRST, SPI_MODE0));
#endif

    SPI.transfer(SX126X_NOP, 1);

	digitalWrite(sx126x_hal->nss, HIGH);
#ifdef SPI_HAS_TRANSACTION
    SPI.endTransaction();
#endif

    while (digitalRead(sx126x_hal->busy)) { }
    
	return SX126X_HAL_STATUS_OK;
}