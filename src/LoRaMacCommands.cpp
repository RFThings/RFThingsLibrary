#include "LoRaMacCommands.h"
#include <Arduino.h>

/**** MAC Command Callbacks Implementation ****/
static LoRaMacCommandCallbacks_t mac_cmd_callbacks = {
  on_mac_cmd_rx_timing_setup_req : on_mac_cmd_rx_timing_setup_req,
  on_mac_cmd_device_time_ans : on_mac_cmd_device_time_ans,
  on_mac_cmd_new_channel_req : on_mac_cmd_new_channel_req,
  on_mac_cmd_link_check_ans : on_mac_cmd_link_check_ans

  /**
   * TODO: Add implementation on these commands
   *     - LinkADRReq
   *     - DutyCycleReq
   *     - RXParamSetupReq
   *     - TXParamSetupReq
   *     - DlChannelReq
   */
};

uint8_t process_mac_command(uint8_t *payload, uint32_t payload_len, int8_t snr)
{
  uint8_t payload_index = 0;

  uint8_t mac_command_respond_payload[255];
  uint8_t mac_command_respond_payload_len = 0;

  mac_command_respond_payload_len = 0;
  memset(mac_command_respond_payload, 0, 255);

  while (payload_index < payload_len)
  {
    /**
     * TODO: Make sure to parse only complete MAC commands
     */
    if (0)
    {
      return 0;
    }

    switch (payload[payload_index])
    {
    case SRV_MAC_RESET_CONF:
      // Serial.println("SRV_MAC_RESET_CONF");
      break;
    case SRV_MAC_LINK_CHECK_ANS:
    {
      uint8_t margin = payload[payload_index + 1];
      uint8_t gw_cnt = payload[payload_index + 2];

      if (mac_cmd_callbacks.on_mac_cmd_link_check_ans != NULL)
      {
        mac_cmd_callbacks.on_mac_cmd_link_check_ans(margin, gw_cnt);
      }
    }
    break;
    case SRV_MAC_LINK_ADR_REQ:
    {
      // Serial.println("SRV_MAC_LINK_ADR_REQ");

      /**
       * TODO: Add LinkADRReq
       *
       */
    }
    break;
    case SRV_MAC_DUTY_CYCLE_REQ:
    {
      // Serial.println("SRV_MAC_DUTY_CYCLE_REQ");

      /**
       * TODO: Add DutyCycleReq
       *
       */
    }
    break;
    case SRV_MAC_RX_PARAM_SETUP_REQ:
    {
      // Serial.println("SRV_MAC_RX_PARAM_SETUP_REQ");

      /**
       * TODO: Add RXParamSetupReq
       *
       */
    }
    break;
    case SRV_MAC_DEV_STATUS_REQ:
    {
      /**** COMMAND RESPONSE ****/
      // CID = DevStatusAns
      mac_command_respond_payload[mac_command_respond_payload_len++] = MOTE_MAC_DEV_STATUS_ANS;

      // No measurement on battery
      mac_command_respond_payload[mac_command_respond_payload_len++] = BAT_LEVEL_NO_MEASURE;

      // Radio status
      mac_command_respond_payload[mac_command_respond_payload_len++] = snr;
    }
    break;
    case SRV_MAC_NEW_CHANNEL_REQ:
    {
      uint8_t ChIndex = payload[payload_index + 1];

      uint32_t Frequency = payload[payload_index + 4];
      Frequency = (Frequency << 8) | payload[payload_index + 3];
      Frequency = (Frequency << 8) | payload[payload_index + 2];

      uint8_t MaxDR = (payload[payload_index + 5] & 0xF0) >> 4;
      uint8_t MinDR = (payload[payload_index + 5] & 0x0F);

      if (mac_cmd_callbacks.on_mac_cmd_new_channel_req != NULL)
      {
        mac_cmd_callbacks.on_mac_cmd_new_channel_req({
          ChannelId : ChIndex,
          Frequency : Frequency,
          MinDR : MinDR,
          MaxDR : MaxDR
        });
      }

      /**** COMMAND RESPONSE ****/
      // CID = DevStatusAns
      mac_command_respond_payload[mac_command_respond_payload_len++] = MOTE_MAC_NEW_CHANNEL_ANS;

      // Status
      mac_command_respond_payload[mac_command_respond_payload_len++] = (1 << 0) /* Channel frequency ok */ | (1 << 1) /* Data-rate range ok */;
    }
    break;
    case SRV_MAC_RX_TIMING_SETUP_REQ:
    {
      uint8_t RxTimingDelay = payload[payload_index + 1] & 0x0F;

      if (mac_cmd_callbacks.on_mac_cmd_rx_timing_setup_req != NULL)
      {
        mac_cmd_callbacks.on_mac_cmd_rx_timing_setup_req(RxTimingDelay);
      }
      /**** COMMAND RESPONSE ****/
      // CID = RXTimingSetupAns
      mac_command_respond_payload[mac_command_respond_payload_len++] = MOTE_MAC_RX_TIMING_SETUP_ANS;
    }
    break;
    case SRV_MAC_TX_PARAM_SETUP_REQ:
    {
      // Serial.println("SRV_MAC_TX_PARAM_SETUP_REQ");

      /**
       * TODO: Add TXParamSetupReq
       *
       */
    }
    break;
    case SRV_MAC_DL_CHANNEL_REQ:
    {
      // Serial.println("SRV_MAC_DL_CHANNEL_REQ");

      /**
       * TODO: Add DlChannelReq
       *
       */
    }
    break;
    case SRV_MAC_REKEY_CONF:
      // Serial.println("SRV_MAC_REKEY_CONF");
      break;
    case SRV_MAC_ADR_PARAM_SETUP_REQ:
      // Serial.println("SRV_MAC_ADR_PARAM_SETUP_REQ");
      break;
    case SRV_MAC_FORCE_REJOIN_REQ:
      // Serial.println("SRV_MAC_FORCE_REJOIN_REQ");
      break;
    case SRV_MAC_REJOIN_PARAM_REQ:
      // Serial.println("SRV_MAC_REJOIN_PARAM_REQ");
      break;
    case SRV_MAC_DEVICE_MODE_CONF:
      // Serial.println("SRV_MAC_DEVICE_MODE_CONF");
      break;
    case SRV_MAC_DEVICE_TIME_ANS:
    {
      uint32_t epoch = payload[payload_index + 1];
      epoch |= payload[payload_index + 2] << 8;
      epoch |= payload[payload_index + 3] << 16;
      epoch |= payload[payload_index + 4] << 24;

      uint8_t fraction_of_1_to_256_sec = payload[payload_index + 5];

      if (mac_cmd_callbacks.on_mac_cmd_device_time_ans != NULL)
      {
        mac_cmd_callbacks.on_mac_cmd_device_time_ans(epoch, fraction_of_1_to_256_sec);
      }
    }
    break;
    case SRV_MAC_PING_SLOT_INFO_ANS:
      // Serial.println("SRV_MAC_PING_SLOT_INFO_ANS");
      break;
    case SRV_MAC_PING_SLOT_CHANNEL_REQ:
      // Serial.println("SRV_MAC_PING_SLOT_CHANNEL_REQ");
      break;
    case SRV_MAC_BEACON_TIMING_ANS:
      // Serial.println("SRV_MAC_BEACON_TIMING_ANS");
      break;
    case SRV_MAC_BEACON_FREQ_REQ:
      // Serial.println("SRV_MAC_BEACON_FREQ_REQ");
      break;

    default:
      // Unknown command. ABORT MAC commands processing
      return 0;
    }

    payload_index += LoRaMacCommandsGetCmdSize(payload[payload_index]);
  }

  if (mac_command_respond_payload_len > 0)
  {
    memcpy(payload, mac_command_respond_payload, mac_command_respond_payload_len); // TODO: Check max length of the payload
  }

  return mac_command_respond_payload_len;
}

uint8_t LoRaMacCommandsGetCmdSize(uint8_t cid)
{
  uint8_t cidSize = 0;

  // Decode Frame MAC commands
  switch (cid)
  {
  case SRV_MAC_RESET_CONF:
  {
    // cid + Serv_LoRaWAN_version
    cidSize = 2;
    break;
  }
  case SRV_MAC_LINK_CHECK_ANS:
  {
    // cid + Margin + GwCnt
    cidSize = 3;
    break;
  }
  case SRV_MAC_LINK_ADR_REQ:
  {
    // cid + DataRate_TXPower + ChMask (2) + Redundancy
    cidSize = 5;
    break;
  }
  case SRV_MAC_DUTY_CYCLE_REQ:
  {
    // cid + DutyCyclePL
    cidSize = 2;
    break;
  }
  case SRV_MAC_RX_PARAM_SETUP_REQ:
  {
    // cid + DLsettings + Frequency (3)
    cidSize = 5;
    break;
  }
  case SRV_MAC_DEV_STATUS_REQ:
  {
    // cid
    cidSize = 1;
    break;
  }
  case SRV_MAC_NEW_CHANNEL_REQ:
  {
    // cid + ChIndex + Frequency (3) + DrRange
    cidSize = 6;
    break;
  }
  case SRV_MAC_RX_TIMING_SETUP_REQ:
  {
    // cid + Settings
    cidSize = 2;
    break;
  }
  case SRV_MAC_TX_PARAM_SETUP_REQ:
  {
    // cid + EIRP_DwellTime
    cidSize = 2;
    break;
  }
  case SRV_MAC_DL_CHANNEL_REQ:
  {
    // cid + ChIndex + Frequency (3)
    cidSize = 5;
    break;
  }
  case SRV_MAC_REKEY_CONF:
  {
    // cid + Serv_LoRaWAN_version
    cidSize = 2;
    break;
  }
  case SRV_MAC_ADR_PARAM_SETUP_REQ:
  {
    // cid + ADRparam
    cidSize = 2;
    break;
  }
  case SRV_MAC_FORCE_REJOIN_REQ:
  {
    // cid + Payload (2)
    cidSize = 3;
    break;
  }
  case SRV_MAC_REJOIN_PARAM_REQ:
  {
    // cid + Payload (1)
    cidSize = 2;
    break;
  }
  case SRV_MAC_DEVICE_MODE_CONF:
  {
    // cid + Class
    cidSize = 2;
    break;
  }
  case SRV_MAC_DEVICE_TIME_ANS:
  {
    // cid + Seconds (4) + Fractional seconds (1)
    cidSize = 6;
    break;
  }
  case SRV_MAC_PING_SLOT_INFO_ANS:
  {
    // cid
    cidSize = 1;
    break;
  }
  case SRV_MAC_PING_SLOT_CHANNEL_REQ:
  {
    // cid + Frequency (3) + DR
    cidSize = 5;
    break;
  }
  case SRV_MAC_BEACON_TIMING_ANS:
  {
    // cid + TimingDelay (2) + Channel
    cidSize = 4;
    break;
  }
  case SRV_MAC_BEACON_FREQ_REQ:
  {
    // cid + Frequency (3)
    cidSize = 4;
    break;
  }
  default:
  {
    // Unknown command. ABORT MAC commands processing
    break;
  }
  }
  return cidSize;
}
