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

/*
* Available firmware:
*   LR1110:     "lr1110/lr1110_transceiver_0303.h"
*               "lr1110/lr1110_transceiver_0304.h"
*               "lr1110/lr1110_transceiver_0305.h"
*               "lr1110/lr1110_transceiver_0306.h"
*               "lr1110/lr1110_transceiver_0307.h"
*   LR1120:     "lr1120/lr1110_transceiver_0101.h"
*/

#include <RFThings.h>
#include <radio/lr11xx/rfthings_lr11xx.h>
#include <radio/lr11xx/lr11xx_firmware_images/lr1110/lr1110_transceiver_0307.h>

uint8_t* lr11xx_pin;
uint8_t* lr11xx_chip_eui;
uint8_t* lr11xx_join_eui;

// nss_pin, rst_pin, busy_pin, dio1_pin
rfthings_lr11xx lr11xx(19, 14, 2, 39);
rft_status_t status;

void setup() {
    Serial.begin(115200);

    while (!Serial && millis() < 3000);

    // Init LR11XX
    Serial.println("#### LR11XX INITIALIZE ####");
    status = lr11xx.init(RFT_REGION_AS923_925);
    Serial.print("LR11XX init: ");
    Serial.println(rft_status_to_str(status));
    Serial.print("LR11XX system hardware: 0x");
    Serial.println(lr11xx.get_system_hardware(), HEX);
    Serial.print("LR11XX system firmware: 0x");
    Serial.println(lr11xx.get_system_firmware(), HEX);
    Serial.print("LR11XX system type    : LR11");
    Serial.println(lr11xx.get_system_type() == 0x01 ? "10" : "20");
    Serial.println();

    lr11xx_pin = lr11xx.get_pin();
    lr11xx_chip_eui = lr11xx.get_chip_eui();
    lr11xx_join_eui = lr11xx.get_join_eui();

    // LR11XX PIN
    Serial.println("#### LR11XX IDENTITY ####");
    Serial.print("PIN: ");
    for (int i = 0; i < 4; i++) {
        Serial.print(lr11xx_pin[i]);
    }
    Serial.println();

    // factory LR11XX chip eui
    Serial.print("CHIP EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_chip_eui[i]);
    }
    Serial.println();

    // factory LR11XX join eui
    Serial.print("JOIN EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_join_eui[i]);
    }
    Serial.println("\n");
}

void loop() {
    Serial.println("#### LR11XX BOOTLOADER ####");
    Serial.print("LR11XX bootloader hardware: 0x");
    Serial.println(lr11xx.get_bootloader_hardware(), HEX);
    Serial.print("LR11XX bootloader firmware: 0x");
    Serial.println(lr11xx.get_bootloader_firmware(), HEX);
    Serial.print("LR11XX bootloader type    : LR11");
    Serial.println(lr11xx.get_bootloader_type() == 0x01 ? "10" : lr11xx.get_bootloader_type() == 0x02 ? "20" : "XX");
    Serial.println();

    status = lr11xx.update_firmware(LR11XX_FIRMWARE_UPDATE_TO, lr11xx_firmware_image, (uint32_t) LR11XX_FIRMWARE_IMAGE_SIZE);

    if (status == RFT_STATUS_OK) {
        Serial.print("LR11XX system hardware: 0x");
        Serial.println(lr11xx.get_system_hardware(), HEX);
        Serial.print("LR11XX system firmware: 0x");
        Serial.println(lr11xx.get_system_firmware(), HEX);
        Serial.print("LR11XX system type    : LR11");
        Serial.println(lr11xx.get_system_type() == 0x01 ? "10" : "20");
        Serial.println();
    }

    while (1) {}
}
