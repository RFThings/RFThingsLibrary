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
 * Author: mtnguyen
 */

#include <RFThings.h>
#include <radio/sx126x/rfthings_sx126x.h>


#define TX_INTERVAL 5 // seconds
const uint8_t device_uid[] = { 0x50, 0xFF, 0x1A, 0x01 };
const char ping_message[] = "PING";


rfthings_sx126x sx126x(E22_NSS, E22_NRST, E22_BUSY, E22_DIO1, E22_RXEN);
rft_status_t status;
uint32_t tx_cnt = 0;

byte payload[255];
uint32_t payload_len = 0;

void setup(void)
{
  pinMode(LS_LED, OUTPUT);
  digitalWrite(LS_LED, LOW);

  Serial.begin(115200);

  while (!Serial && (millis() < 3000))
    ;

  // Init LoRa Module
  Serial.println("#### LORA MODULE INITIALIZE ####");
  status = sx126x.init(RFT_REGION_EU863_870);
  Serial.print("SX126x Init: ");
  Serial.println(rft_status_to_str(status));

  // LoRa Packet Params
  sx126x_pkt_params_lora_t lora_packet_param;
  lora_packet_param.preamble_len_in_symb = 8;
  lora_packet_param.header_type = SX126X_LORA_PKT_EXPLICIT;
  lora_packet_param.pld_len_in_bytes = 255;
  lora_packet_param.crc_is_on = true;
  lora_packet_param.invert_iq_is_on = false;
  sx126x.set_lora_pkt_param(lora_packet_param);

  sx126x.set_tx_power(21);
  sx126x.set_frequency(868300000);
  sx126x.set_spreading_factor(RFT_LORA_SPREADING_FACTOR_9);
  sx126x.set_bandwidth(RFT_LORA_BANDWIDTH_250KHZ);
  sx126x.set_coding_rate(RFT_LORA_CODING_RATE_4_5);
  sx126x.set_syncword(RFT_LORA_SYNCWORD_PRIVATE);

  tx_cnt = 0;

  delay(500);
}

void loop(void)
{
  digitalWrite(LS_LED, HIGH);
  delay(100);
  digitalWrite(LS_LED, LOW);
  delay(100);
  digitalWrite(LS_LED, HIGH);
  delay(100);
  digitalWrite(LS_LED, LOW);
  delay(100);

  /*
   * [TEST] Build packet
   *
   * time_on_air = 2000 ms
   * altitude = 14m (x 10e2)
   * latitude = 43.6207616 (x 10e7)
   * longtitude = 7.0671429 (x 10e7)
   * tx_power = 14 dBm
   * app_payload_len = 4
   * ping_message = "PING"
   * tx_cnt
   * rx_cnt = 1
   */
  build_telnet_packet(2000, 1400, 436207616, 70671429, 21, 4, (uint8_t *)ping_message, tx_cnt++, 0);

  // Send packet
  Serial.print("Sending LoRa message: ");
  if (sx126x.send_lora(payload, payload_len, 2000, NULL) == RFT_STATUS_TX_TIMEOUT)
  {
    Serial.println("Sending LoRa message FAIL");
    Serial.println(rft_status_to_str(status));
  }
  else
  {
    // Print sending information
    Serial.println("Sending LoRa message DONE");
    Serial.print("Sent ");
    Serial.print(payload_len);
    Serial.print(" bytes: ");

    for (uint8_t i = 0; i < payload_len; i++)
    {
      if (payload[i] < 16)
      {
        Serial.print('0');
      }
      Serial.print(payload[i], HEX);
    }
    Serial.println();
  }

  // TODO: Low Power Delay
  delay(TX_INTERVAL * 1000);
}

void build_telnet_packet(uint32_t time_on_air, int32_t gps_altitude, int32_t gps_latitude, int32_t gps_longitude, int8_t tx_power, uint8_t app_payload_len, uint8_t *ping_message, uint32_t tx_cnt, uint32_t rx_cnt)
{
  /* Reset packet buffer to zeros */
  memset(payload, 0, 255);

  // Device Unique ID
  payload[0] = device_uid[3];
  payload[1] = device_uid[2];
  payload[2] = device_uid[1];
  payload[3] = device_uid[0];

  // Time On Air calculé du message LoRa (ms)
  payload[4] = (time_on_air >> 24) & 0xff;
  payload[5] = (time_on_air >> 16) & 0xff;
  payload[6] = (time_on_air >> 8) & 0xff;
  payload[7] = time_on_air & 0xff;

  // L'altitude du noeud emetteur
  payload[8] = (gps_altitude >> 24) & 0xff;
  payload[9] = (gps_altitude >> 16) & 0xff;
  payload[10] = (gps_altitude >> 8) & 0xff;
  payload[11] = gps_altitude & 0xff;

  // La latitude du noeud emetteur
  payload[12] = (gps_latitude >> 24) & 0xff;
  payload[13] = (gps_latitude >> 16) & 0xff;
  payload[14] = (gps_latitude >> 8) & 0xff;
  payload[15] = gps_latitude & 0xff;

  // La longitude du noeud emetteur
  payload[16] = (gps_longitude >> 24) & 0xff;
  payload[17] = (gps_longitude >> 16) & 0xff;
  payload[18] = (gps_longitude >> 8) & 0xff;
  payload[19] = gps_longitude & 0xff;

  // La puissance de transmission du noeud (dBm)
  payload[20] = tx_power;

  // La taille restante
  payload[21] = app_payload_len;

  // Message Ping
  if (ping_message != NULL)
  {
    payload[22] = ping_message[0];
    payload[23] = ping_message[1];
    payload[24] = ping_message[2];
    payload[25] = ping_message[3];
  }

  // Nombre de Messages LoRa envoyés
  payload[26] = (tx_cnt >> 24) & 0xff;
  payload[27] = (tx_cnt >> 16) & 0xff;
  payload[28] = (tx_cnt >> 8) & 0xff;
  payload[29] = tx_cnt & 0xff;

  // Nombre de Messages LoRa reçus
  payload[30] = (rx_cnt >> 24) & 0xff;
  payload[31] = (rx_cnt >> 16) & 0xff;
  payload[32] = (rx_cnt >> 8) & 0xff;
  payload[33] = rx_cnt & 0xff;

  payload_len = 34;
}