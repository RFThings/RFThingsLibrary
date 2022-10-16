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

/* Support REGION
    RFT_REGION_EU863_870
	RFT_REGION_US902_928
 	RFT_REGION_CN470_510
 	RFT_REGION_AU915_928
 	RFT_REGION_AS920_923
 	RFT_REGION_AS923_925
 	RFT_REGION_KR920_923
 	RFT_REGION_IN865_867
*/

#include <RFThings.h>
#include <radio/lr11xx/rfthings_lr11xx.h>

#define TX_INTERVAL 10

#define LNA_CTRL_MCU_PIN 41

// Keys and device address are MSB
static uint8_t nwkS_key[] = { 0x03, 0x1D, 0x45, 0x30, 0x8F, 0xAB, 0x76, 0x19, 0xAB, 0x34, 0x09, 0xE1, 0x41, 0xDC, 0xE5, 0xE8 };
static uint8_t appS_key[] = { 0xB8, 0x2A, 0x34, 0x8A, 0xF5, 0x9F, 0x82, 0x0E, 0xB1, 0x9F, 0x23, 0x30, 0x0F, 0xDA, 0xA3, 0xC7 };
static uint8_t dev_addr[] = { 0x26, 0x0D, 0xB1, 0x5C };
uint8_t* lr11xx_pin;
uint8_t* lr11xx_chip_eui;
uint8_t* lr11xx_join_eui;

// nss_pin, rst_pin, busy_pin, dio1_pin
// rfthings_lr11xx lr11xx(7, 9, 3, 2);
rfthings_lr11xx lr11xx(19, 14, 2, 39);
rft_status_t status;

char payload[255];
uint32_t payload_len;

String message = "hello world";

void setup() {
    Serial.begin(115200);

    while (!Serial && millis() < 3000);

    pinMode(LNA_CTRL_MCU_PIN, OUTPUT);
    digitalWrite(LNA_CTRL_MCU_PIN, HIGH);

    // Init LR11XX
    Serial.println("#### LR11XX INITIALIZE ####");
    status = lr11xx.init(RFT_REGION_KR920_923);
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
        Serial.print(lr11xx_pin[i], HEX);
    }
    Serial.println();

    // factory LR11XX chip eui
    Serial.print("CHIP EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_chip_eui[i], HEX);
    }
    Serial.println();

    // factory LR11XX join eui
    Serial.print("JOIN EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_join_eui[i], HEX);
    }
    Serial.println("\n");

    // set up LoRaWAN key (ABP by default)
    lr11xx.set_lorawan_activation_type(RFT_LORAWAN_ACTIVATION_TYPE_ABP);
    lr11xx.set_application_session_key(appS_key);
    lr11xx.set_network_session_key(nwkS_key);
    lr11xx.set_device_address(dev_addr);
    lr11xx.set_rx1_delay(1000);
}

void loop() {
    Serial.print("Sending LoRaWAN message: ");

    build_payload();
    status = lr11xx.send_uplink((byte*)payload, payload_len);

    if (status == RFT_STATUS_OK) {
        Serial.println("receive downlink packet");
        Serial.print("    RSSI: ");
        Serial.println(lr11xx.get_rssi());
        Serial.print("    SNR: ");
        Serial.println(lr11xx.get_snr());
        Serial.print("    Signal rssi: ");
        Serial.println(lr11xx.get_signal_rssi());

        Serial.print("Downlink payload: ");
        for (int i = 0; i < lr11xx.get_rx_length(); i++) {
            Serial.print(payload[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    } else {
        Serial.println(rft_status_to_str(status));
    }

    delay(TX_INTERVAL*1000);
}

void build_payload(void) {
    message.toCharArray(payload, 255); 
    payload_len = message.length();
}
