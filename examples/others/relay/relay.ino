#include <RFThings.h>
#include <radio/sx126x/rfthings_sx126x.h>

// Device information (For uploading relay status)
static uint8_t nwkS_key[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t appS_key[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t dev_addr[] = {0x00, 0x00, 0x00, 0x00};

// nss_pin, rst_pin, busy_pin, dio1_pin, antenna_switch_pin
rfthings_sx126x sx126x(10, 14, 2, 39, 1);
rft_status_t status;

// Relay params (Listen to all devices meet this LoRaPHY Params, NO FILTER)
rft_lora_params_t relay_lora_params;
byte payload[255];
uint32_t payload_len;

void setup(void)
{
  pinMode(LS_LED_BLUE, OUTPUT);
  digitalWrite(LS_LED_BLUE, HIGH);
  delay(200);
  digitalWrite(LS_LED_BLUE, LOW);

  Serial.begin(9600);
  while (!Serial && millis() < 3000)
    ;
  Serial.println("Initializing - Relay example for DKPlatium");

  analogReadResolution(12);

  pinMode(LS_GPS_ENABLE, OUTPUT);
  digitalWrite(LS_GPS_ENABLE, LOW);
  pinMode(LS_GPS_V_BCKP, OUTPUT);
  digitalWrite(LS_GPS_V_BCKP, LOW);

  digitalWrite(LS_VERSION_ENABLE, LOW);

  pinMode(LS_INT_MAG, OUTPUT); // make INT_MAG LOW for low-power
  digitalWrite(LS_INT_MAG, LOW);

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

  // Setting relay LoRa Params
  relay_lora_params.frequency = 866600000;
  relay_lora_params.spreading_factor = RFT_LORA_SPREADING_FACTOR_7;
  relay_lora_params.bandwidth = RFT_LORA_BANDWIDTH_125KHZ;
  relay_lora_params.coding_rate = RFT_LORA_CODING_RATE_4_5;
  relay_lora_params.syncword = RFT_LORA_SYNCWORD_PUBLIC;

  delay(2000); // For DEBUG
}

void loop(void)
{
  switch (sx126x.relay(&relay_lora_params, payload, payload_len, NULL, NULL))
  {
  case RFT_STATUS_OK:
    if (payload_len > 0)
    {
      // TODO: Filter packet and verify packet integrity

      Serial.print("[Relay] OK: Received ");
      Serial.print(payload_len);
      Serial.println(" bytes");

      Serial.print("Payload: ");
      for (uint8_t i = 0; i < payload_len; i++)
      {
        Serial.print(payload[i]);
        Serial.print(" ");
      }
      Serial.println();

      Serial.print("    Received RSSI: ");
      Serial.println(relay_lora_params.rssi);
      Serial.print("    Received SNR: ");
      Serial.println(relay_lora_params.snr);
      Serial.print("    Received Signal RSSI: ");
      Serial.println(relay_lora_params.signal_rssi);

      // TODO: Your processing (Forward packet, update relay status variable, etc.)
      digitalWrite(LS_LED_BLUE, HIGH);
      delay(50);
      digitalWrite(LS_LED_BLUE, LOW);
      delay(50);
      digitalWrite(LS_LED_BLUE, HIGH);
      delay(50);
      digitalWrite(LS_LED_BLUE, LOW);
      delay(50);
      digitalWrite(LS_LED_BLUE, HIGH);
      delay(50);
      digitalWrite(LS_LED_BLUE, LOW);
    }
    else
    {
      Serial.println("[Relay] ERROR: Unknown ERROR!");
    }
    break;
  case RFT_STATUS_PREAMBLE_DETECT_FAIL:
    Serial.println("[Relay] ERROR: Fail to detect preamble!");
    break;
  case RFT_STATUS_RX_TIMEOUT:
    Serial.println("[Relay] ERROR: RX Timeout!");
    break;
  default:
    break;
  }
}