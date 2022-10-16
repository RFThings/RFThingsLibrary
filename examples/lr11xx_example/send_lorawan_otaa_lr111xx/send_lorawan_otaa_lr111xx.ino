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

#define SE2341L_CPS 43
#define SE2341L_CTX 42
#define SE2341L_CSD 35
#define SE2341L_ANT 34

#define LNA_CTRL_MCU_PIN 41

// Keys and device address are MSB
static uint8_t app_eui[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t app_key[] =  { 0x45, 0x47, 0x8C, 0xB2, 0x98, 0xA1, 0x4F, 0xBC, 0xBC, 0x08, 0xDB, 0xD2, 0x96, 0x9D, 0x10, 0x83 };
static uint8_t dev_eui[] =  { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x05, 0x00, 0xB4 };
uint8_t* dev_addr;
uint8_t* nwkS_key;
uint8_t* appS_key;
uint8_t* lr11xx_pin;
uint8_t* lr11xx_chip_eui;
uint8_t* lr11xx_join_eui;

// nss_pin, rst_pin, busy_pin, dio1_pin
rfthings_lr11xx lr11xx(19, 14, 2, 39);
rft_status_t status;
bool join_accept = false;

char payload[255];
uint32_t payload_len;

String message = "hello world";

void setup() {
    Serial.begin(115200);

    while (!Serial && millis() < 3000);

    pinMode(SE2341L_CPS, OUTPUT);
    pinMode(SE2341L_CTX, OUTPUT);
    pinMode(SE2341L_CSD, OUTPUT);
    pinMode(SE2341L_ANT, OUTPUT);

    pinMode(LNA_CTRL_MCU_PIN, OUTPUT);
    digitalWrite(LNA_CTRL_MCU_PIN, HIGH);

    se2341l_off();

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
    Serial.println(lr11xx.get_bootloader_type() == 0x01 ? "10" : lr11xx.get_bootloader_type() == 0x02 ? "20" : "XX");
    Serial.println();

    lr11xx_pin = lr11xx.get_pin();
    lr11xx_chip_eui = lr11xx.get_chip_eui();
    lr11xx_join_eui = lr11xx.get_join_eui();

    // LR11XX PIN
    Serial.println("#### LR11XX IDENTITY ####");
    Serial.print("PIN     : ");
    for (int i = 0; i < 4; i++) {
        Serial.print(lr11xx_pin[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // factory LR11XX chip eui
    Serial.print("CHIP EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_chip_eui[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // factory LR11XX join eui
    Serial.print("JOIN EUI: ");
    for (int i = 0; i < 8; i++) {
        Serial.print(lr11xx_join_eui[i], HEX);
        Serial.print(" ");
    }
    Serial.println("\n");

    // set up LoRaWAN key (ABP by default)
    lr11xx.set_lorawan_activation_type(RFT_LORAWAN_ACTIVATION_TYPE_OTAA);
    lr11xx.set_devive_eui(dev_eui);
    lr11xx.set_application_eui(app_eui);
    lr11xx.set_application_key(app_key);
    lr11xx.set_rx1_delay(5000);
}

void loop() {
    if (!join_accept) {
        Serial.print("Sending join request: ");
        status = lr11xx.send_join_request();

        if (status == RFT_STATUS_OK) {
            dev_addr = lr11xx.get_device_address();
            nwkS_key = lr11xx.get_network_session_key();
            appS_key = lr11xx.get_application_session_key();

            Serial.println("Join accept!");
            join_accept = true;

            Serial.print("Device address: ");
            for (int i = 0; i < 4; i++) {
                Serial.print(dev_addr[i]);
            }
            Serial.println();

            Serial.print("Network session key: ");
            for (int i = 0; i < 16; i++) {
                Serial.print(nwkS_key[i]);
            }
            Serial.println();

            Serial.print("Application session address: ");
            for (int i = 0; i < 16; i++) {
                Serial.print(appS_key[i]);
            }
            Serial.println();
        } else {
            Serial.println(rft_status_to_str(status));
        }
    } else {
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
    }

    delay(TX_INTERVAL*1000);
}

void build_payload(void) {
    message.toCharArray(payload, 255); 
    payload_len = message.length();
}

void se2341l_tx() {
    digitalWrite(SE2341L_CSD, HIGH);
    digitalWrite(SE2341L_ANT, HIGH);
    digitalWrite(SE2341L_CTX, HIGH);
    digitalWrite(SE2341L_CPS, LOW);
}

void se2341l_rx() {
    digitalWrite(SE2341L_CSD, HIGH);
    digitalWrite(SE2341L_ANT, HIGH);
    digitalWrite(SE2341L_CTX, LOW);
    digitalWrite(SE2341L_CPS, HIGH);
}

void se2341l_bypass() {
    digitalWrite(SE2341L_CSD, HIGH);
    digitalWrite(SE2341L_ANT, HIGH);
    digitalWrite(SE2341L_CTX, LOW);
    digitalWrite(SE2341L_CPS, LOW);
}

void se2341l_off() {
    digitalWrite(SE2341L_CSD, LOW);
    digitalWrite(SE2341L_ANT, HIGH);
    digitalWrite(SE2341L_CTX, LOW);
    digitalWrite(SE2341L_CPS, LOW);
}