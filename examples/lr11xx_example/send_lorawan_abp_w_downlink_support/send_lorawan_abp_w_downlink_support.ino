/**
 * @file send_lorawan_abp_w_downlink_support.ino
 * @author nguyenmanhthao996tn (nguyenmanhthao996tn@gmail.com)
 * 
 * @brief This sketch demonstrates the Activation-by-personalization downlink support by
 *        answer MAC Commands from Network Server.
 * 
 * Tested configuration:
 *      - Network Server: The Things Network Community Server
 *      - Hardware: RFThings Libra (STM32WB & LR1120)
 *      - Platform: stm32duino (https://github.com/stm32duino/Arduino_Core_STM32)
 *           + Board: Generic STM32WB Series
 *           + Board part number: Generic WB55REVx
 * 
 * @version 1.0
 * @date 2023-09-12
 * 
 * @copyright Copyright (c) 2023
 * 
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
#include <LoRaMacCommands.h>
#include <radio/lr11xx/rfthings_lr11xx.h>

#define TX_INTERVAL 15

#define LNA_CTRL_MCU_PIN PA_8
#define LED_PIN PC10
#define BUTTON_PIN PB11

// Keys and device address are MSB
static uint8_t nwkS_key[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appS_key[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t dev_addr[] = {0x00, 0x00, 0x00, 0x00};
uint8_t *lr11xx_pin;
uint8_t *lr11xx_chip_eui;
uint8_t *lr11xx_join_eui;

// nss_pin, rst_pin, busy_pin, dio1_pin
rfthings_lr11xx lr11xx(PA_4, PA_0, PB_0, PB_3);
rft_status_t status;

char payload[255];
uint8_t payload_len;

String message = "hello world";

typedef enum
{
    UPLINK_TYPE_NORMAL = 0,
    UPLINK_TYPE_MAC_CMD_RESP = 1
} UplinkType_t;
UplinkType_t next_uplink_type = UPLINK_TYPE_NORMAL;

bool flag = false;

/************************** Main Program **************************/
void setup()
{
    Serial.begin(115200);

    while (!Serial && millis() < 3000)
        ;

    pinMode(LNA_CTRL_MCU_PIN, OUTPUT);
    digitalWrite(LNA_CTRL_MCU_PIN, HIGH);

    // Init LR11XX
    Serial.println("#### LR11XX INITIALIZE ####");
    status = lr11xx.init(RFT_REGION_EU863_870);
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
    for (int i = 0; i < 4; i++)
    {
        Serial.print(lr11xx_pin[i], HEX);
    }
    Serial.println();

    // factory LR11XX chip eui
    Serial.print("CHIP EUI: ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(lr11xx_chip_eui[i], HEX);
    }
    Serial.println();

    // factory LR11XX join eui
    Serial.print("JOIN EUI: ");
    for (int i = 0; i < 8; i++)
    {
        Serial.print(lr11xx_join_eui[i], HEX);
    }
    Serial.println("\n");

    // set up LoRaWAN key (ABP by default)
    lr11xx.set_lorawan_activation_type(RFT_LORAWAN_ACTIVATION_TYPE_ABP);
    lr11xx.set_application_session_key(appS_key);
    lr11xx.set_network_session_key(nwkS_key);
    lr11xx.set_device_address(dev_addr);
    lr11xx.set_rx1_delay(1000);

    UplinkType_t next_uplink_type = UPLINK_TYPE_NORMAL;
}

void loop()
{
    send_uplink();
    check_and_process_mac_cmd();
    delay(TX_INTERVAL * 1000);
}

/************************** Functions **************************/
void send_uplink(void)
{
    switch (next_uplink_type)
    {
    case UPLINK_TYPE_NORMAL:
    {
        Serial.print("Sending LoRaWAN message: ");
        build_payload();
        lr11xx.set_tx_port(1);
        next_uplink_type = UPLINK_TYPE_NORMAL;
    }
    break;
    case UPLINK_TYPE_MAC_CMD_RESP:
    {
        Serial.print("Sending MAC Command response: ");
        lr11xx.set_tx_port(0);
        next_uplink_type = UPLINK_TYPE_NORMAL;
    }
    break;
    default:
        next_uplink_type = UPLINK_TYPE_NORMAL;
        break;
    }

    status = lr11xx.send_uplink((byte *)payload, payload_len, NULL, NULL);

    if (status == RFT_STATUS_OK)
    {
        Serial.println("receive downlink packet");
        Serial.print("    Payload length: ");
        Serial.println(lr11xx.get_rx_length());
        Serial.print("    RX Port: ");
        Serial.println(lr11xx.get_rx_port());
        Serial.print("    RSSI: ");
        Serial.println(lr11xx.get_rssi());
        Serial.print("    SNR: ");
        Serial.println(lr11xx.get_snr());
        Serial.print("    Signal rssi: ");
        Serial.println(lr11xx.get_signal_rssi());

        if (lr11xx.get_rx_length() > 0)
        {
            Serial.print("Downlink payload: ");
            for (int i = 0; i < lr11xx.get_rx_length(); i++)
            {
                Serial.print(payload[i], HEX);
                Serial.print(" ");
            }
            Serial.println();
        }
    }
    else
    {
        Serial.println(rft_status_to_str(status));
    }
}

void check_and_process_mac_cmd(void)
{
    uint8_t resp_cmd_len = 0;
    if (lr11xx.get_rx_fopts_len() > 0)
    {
        Serial.println("MAC Command downlink detected in piggyback!");
        memcpy(payload, lr11xx.get_rx_fopts(), lr11xx.get_rx_fopts_len());
        resp_cmd_len = process_mac_command((byte *)payload, lr11xx.get_rx_fopts_len(), lr11xx.get_snr());
    }
    else if ((lr11xx.get_rx_port() == 0) && (lr11xx.get_rx_length() > 0))
    {
        Serial.println("MAC Command downlink detected in packet's payload!");
        resp_cmd_len = process_mac_command((byte *)payload, lr11xx.get_rx_length(), lr11xx.get_snr());
    }

    if (resp_cmd_len > 0)
    {
        Serial.println("Schedule an MAC command repsonse for the next uplink");
        payload_len = resp_cmd_len;
        next_uplink_type = UPLINK_TYPE_MAC_CMD_RESP;
    }
}

void build_payload(void)
{
    message.toCharArray(payload, 255);
    payload_len = message.length();
}

/************************** MAC Command event callbacks **************************/
/*
 * NOTE: It is NOT required to declare all of these callbacks.
 *       Just declare what you need.
 */

void on_mac_cmd_rx_timing_setup_req(uint8_t delay_in_sec)
{
    Serial.print("on_mac_cmd_rx_timing_setup_req | delay = ");
    Serial.println(delay_in_sec);

    lr11xx.set_rx1_delay(delay_in_sec * 1000);
}

void on_mac_cmd_device_time_ans(uint32_t epoch, uint8_t fraction_of_1_to_256_sec)
{
    Serial.print("on_mac_cmd_device_time_ans | GPS epoch = ");
    Serial.println(epoch);
}

void on_mac_cmd_new_channel_req(NewChannelReqParams_t channel_params)
{
    Serial.print("on_mac_cmd_new_channel_req | Channel Index = ");
    Serial.print(channel_params.ChannelId);
    Serial.print(" | Frequency = ");
    Serial.print(channel_params.Frequency);
    Serial.print(" | MinDR = ");
    Serial.print(channel_params.MinDR);
    Serial.print(" | MaxDR = ");
    Serial.println(channel_params.MaxDR);
}

void on_mac_cmd_link_check_ans(uint8_t margin, uint8_t gateway_count)
{
    Serial.print("on_mac_cmd_link_check_ans | margin = ");
    Serial.print(margin);
    Serial.print(" | gateway_count = ");
    Serial.println(gateway_count);
}