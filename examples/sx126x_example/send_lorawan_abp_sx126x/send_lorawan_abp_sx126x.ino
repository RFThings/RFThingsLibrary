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
#include <radio/sx126x/rfthings_sx126x.h>

#define TX_INTERVAL 10

#define LED 7

// Keys and device address are MSB
static uint8_t nwkS_key[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t appS_key[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t dev_addr[] = { 0x00, 0x00, 0x00, 0x00 };

// nss_pin, rst_pin, busy_pin, dio1_pin, antenna_switch_pin
rfthings_sx126x sx126x(10, 14, 2, 39, 1);
rft_status_t status;

char payload[255];
uint32_t payload_len;

String message = "hello world";

void setup() {
    Serial.begin(115200);

    while (!Serial && millis() < 3000);

    pinMode(LED, OUTPUT);
    digitalWrite(LED, LOW);

    // Init sx126x
    Serial.println("#### SX126X INITIALIZE ####");
    status = sx126x.init(RFT_REGION_KR920_923);
    Serial.print("sx126x init: ");
    Serial.println(rft_status_to_str(status));

    // set up LoRaWAN key (ABP by default)
    sx126x.set_lorawan_activation_type(RFT_LORAWAN_ACTIVATION_TYPE_ABP);
    sx126x.set_application_session_key(appS_key);
    sx126x.set_network_session_key(nwkS_key);
    sx126x.set_device_address(dev_addr);
    sx126x.set_rx1_delay(1000);
}

void loop() {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);

    Serial.print("Sending LoRaWAN message: ");

    build_payload();
    status = sx126x.send_uplink((byte*)payload, payload_len, NULL, NULL);

    if (status == RFT_STATUS_OK) {
        Serial.println("receive downlink packet");
        Serial.print("    RSSI: ");
        Serial.println(sx126x.get_rssi());
        Serial.print("    SNR: ");
        Serial.println(sx126x.get_snr());
        Serial.print("    Signal rssi: ");
        Serial.println(sx126x.get_signal_rssi());

        Serial.print("Downlink payload: ");
        for (int i = 0; i < sx126x.get_rx_length(); i++) {
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