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
static uint8_t app_eui[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t app_key[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static uint8_t dev_eui[] =  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t* dev_addr;
uint8_t* nwkS_key;
uint8_t* appS_key;

// nss_pin, rst_pin, busy_pin, dio1_pin, antenna_switch_pin
// rfthings_sx126x sx126x(7, 3, 8, 9, 10);
rfthings_sx126x sx126x(10, 14, 2, 39, 1);
rft_status_t status;
bool join_accept = false;

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
    sx126x.set_lorawan_activation_type(RFT_LORAWAN_ACTIVATION_TYPE_OTAA);
    sx126x.set_devive_eui(dev_eui);
    sx126x.set_application_eui(app_eui);
    sx126x.set_application_key(app_key);
    sx126x.set_rx1_delay(5000);
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

    if (!join_accept) {
        Serial.print("Sending join request: ");
        status = sx126x.send_join_request();

        if (status == RFT_STATUS_OK) {
            dev_addr = sx126x.get_device_address();
            nwkS_key = sx126x.get_network_session_key();
            appS_key = sx126x.get_application_session_key();

            Serial.println("Join accept!");
            join_accept = true;

            Serial.print("Device address: ");
            for (int i = 0; i < 4; i++) {
                Serial.print(dev_addr[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            Serial.print("Network session key: ");
            for (int i = 0; i < 16; i++) {
                Serial.print(nwkS_key[i], HEX);
                Serial.print(" ");
            }
            Serial.println();

            Serial.print("Application session address: ");
            for (int i = 0; i < 16; i++) {
                Serial.print(appS_key[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        } else {
            Serial.println(rft_status_to_str(status));
        }
    } else {
        Serial.print("Sending LoRaWAN message: ");

        build_payload();
        status = sx126x.send_uplink((byte*)payload, payload_len);

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
    }

    delay(TX_INTERVAL*1000);
}

void build_payload(void) {
    message.toCharArray(payload, 255); 
    payload_len = message.length();
}