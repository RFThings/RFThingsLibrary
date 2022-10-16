#include <RFThings.h>
#include <SPI.h>
#include <radio/lr11xx/rfthings_lr11xx.h>
#include "radio/lr11xx/lr11xx_driver/lr11xx_system_types.h"
#include "radio/lr11xx/lr11xx_driver/lr11xx_hal.h"
#include "radio/lr11xx/lr11xx_driver/lr11xx_system.h"

// #define BOARD_NUCLEO64_L476RG_LR1110_EVK
#if defined(BOARD_NUCLEO64_L476RG_LR1110_EVK)
#define LED_TX 20 // PC1
#define LED_RX 21 // PC0

#define LR1110_NSS 7  // PA8
#define LR1110_RST 16 // PA0
#define LR1110_BUSY 3 // PB3
#define LR1110_DIO1 5 // PB4
#endif

#define RFT_GEMINI_V21
#if defined(RFT_GEMINI_V21)
#define LED_TX 2 // PB5
#define LED_RX 2 // PB5

#define LR1110_NSS 4   // PA4
#define LR1110_RST 16  // PA0
#define LR1110_BUSY 27 // PB0
#define LR1110_DIO1 3  // PB4

#define MODULE_ON_OFF_PIN 7 // PB1

#define LNA_CTRL_MCU 20 // PA8
#endif

// #define RFT_LIBRA
#if defined(RFT_LIBRA)
#define LED_TX -1
#define LED_RX -1

#define LR1110_NSS 19
#define LR1110_RST 14
#define LR1110_BUSY 2
#define LR1110_DIO1 39
#endif

// nss_pin, rst_pin, busy_pin, dio1_pin
rfthings_lr11xx lr11xx(LR1110_NSS, LR1110_RST, LR1110_BUSY, LR1110_DIO1);
rft_status_t status;

uint8_t *lr11xx_pin;
uint8_t *lr11xx_chip_eui;
uint8_t *lr11xx_join_eui;

void setup(void)
{
#if defined(RFT_GEMINI_V21)
  pinMode(MODULE_ON_OFF_PIN, OUTPUT);
  digitalWrite(MODULE_ON_OFF_PIN, HIGH);

  pinMode(LNA_CTRL_MCU, OUTPUT);
  digitalWrite(LNA_CTRL_MCU, LOW);
#endif

  Serial.begin(115200);
  pinMode(LED_RX, OUTPUT);
  pinMode(LED_TX, OUTPUT);

  while (!Serial && (millis() < 5000))
    ;

  // Init LR11XX
  Serial.println("#### LR11XX INITIALIZE ####");
  status = lr11xx.init(RFT_REGION_EU863_870);
  Serial.print("LR11XX init: ");
  Serial.println(rft_status_to_str(status));
  Serial.print("    LR11XX system hardware: 0x");
  Serial.println(lr11xx.get_system_hardware(), HEX);
  Serial.print("    LR11XX system firmware: 0x");
  Serial.println(lr11xx.get_system_firmware(), HEX);
  Serial.print("    LR11XX system type    : LR11");
  Serial.println(lr11xx.get_system_type() == 0x01 ? "10" : "20");
  Serial.print("    LR11XX GNSS firmware  : 0x");
  Serial.println(lr11xx.get_gnss_firmware(), HEX);
  Serial.print("    LR11XX GNSS almanac   : 0x");
  Serial.println(lr11xx.get_gnss_firmware(), HEX);
  Serial.println();

  // LR11XX PIN
  lr11xx_pin = lr11xx.get_pin();
  lr11xx_chip_eui = lr11xx.get_chip_eui();
  lr11xx_join_eui = lr11xx.get_join_eui();
  Serial.println("#### LR11XX IDENTITY ####");
  Serial.print("    PIN: ");
  for (int i = 0; i < 4; i++)
  {
    Serial.print(lr11xx_pin[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // factory LR11XX chip eui
  Serial.print("    CHIP EUI: ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(lr11xx_chip_eui[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // factory LR11XX join eui
  Serial.print("    JOIN EUI: ");
  for (int i = 0; i < 8; i++)
  {
    Serial.print(lr11xx_join_eui[i], HEX);
    Serial.print(" ");
  }
  Serial.println("\n");

  delay(500);
  Serial.println("Config CW");
  lr11xx.config_continous_wave();

  Serial.println("Start CW");
  lr11xx.start_continuous_wave();
}

void loop(void)
{
  digitalWrite(LED_RX, HIGH);
  digitalWrite(LED_TX, LOW);
  delay(1000);

  digitalWrite(LED_RX, LOW);
  digitalWrite(LED_TX, HIGH);
  delay(1000);
}
