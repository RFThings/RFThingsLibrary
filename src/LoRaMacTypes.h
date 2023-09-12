/*!
 * \file      LoRaMacTypes.h
 *
 * \brief     LoRa MAC layer internal types definition. Please do not include in application sources.
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013 Semtech
 *
 *               ___ _____ _   ___ _  _____ ___  ___  ___ ___
 *              / __|_   _/_\ / __| |/ / __/ _ \| _ \/ __| __|
 *              \__ \ | |/ _ \ (__| ' <| _| (_) |   / (__| _|
 *              |___/ |_/_/ \_\___|_|\_\_| \___/|_|_\\___|___|
 *              embedded.connectivity.solutions===============
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Daniel Jaeckle ( STACKFORCE )
 *
 * \author    Johannes Bruder ( STACKFORCE )
 *
 * addtogroup LORAMAC
 * \{
 *
 */
#ifndef __LORAMAC_TYPES_H__
#define __LORAMAC_TYPES_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

    /*!
     * LoRaMAC mote MAC commands
     *
     * LoRaWAN Specification V1.1.0, chapter 5, table 4
     */
    typedef enum eLoRaMacMoteCmd
    {
        /*!
         * ResetInd
         */
        MOTE_MAC_RESET_IND = 0x01,
        /*!
         * LinkCheckReq
         */
        MOTE_MAC_LINK_CHECK_REQ = 0x02,
        /*!
         * LinkADRAns
         */
        MOTE_MAC_LINK_ADR_ANS = 0x03,
        /*!
         * DutyCycleAns
         */
        MOTE_MAC_DUTY_CYCLE_ANS = 0x04,
        /*!
         * RXParamSetupAns
         */
        MOTE_MAC_RX_PARAM_SETUP_ANS = 0x05,
        /*!
         * DevStatusAns
         */
        MOTE_MAC_DEV_STATUS_ANS = 0x06,
        /*!
         * NewChannelAns
         */
        MOTE_MAC_NEW_CHANNEL_ANS = 0x07,
        /*!
         * RXTimingSetupAns
         */
        MOTE_MAC_RX_TIMING_SETUP_ANS = 0x08,
        /*!
         * TXParamSetupAns
         */
        MOTE_MAC_TX_PARAM_SETUP_ANS = 0x09,
        /*!
         * DlChannelAns
         */
        MOTE_MAC_DL_CHANNEL_ANS = 0x0A,
        /*!
         * RekeyInd
         */
        MOTE_MAC_REKEY_IND = 0x0B,
        /*!
         * DeviceTimeReq
         */
        MOTE_MAC_DEVICE_TIME_REQ = 0x0D,
        /*!
         * ADRParamSetupAns
         */
        MOTE_MAC_ADR_PARAM_SETUP_ANS = 0x0C,
        /*!
         * RejoinParamSetupAns
         */
        MOTE_MAC_REJOIN_PARAM_ANS = 0x0F,
        /*!
         * DeviceModeInd ( Class C only )
         */
        MOTE_MAC_DEVICE_MODE_IND = 0x20,
        /*!
         * PingSlotInfoReq
         */
        MOTE_MAC_PING_SLOT_INFO_REQ = 0x10,
        /*!
         * PingSlotFreqAns
         */
        MOTE_MAC_PING_SLOT_CHANNEL_ANS = 0x11,
        /*!
         * BeaconTimingReq
         */
        MOTE_MAC_BEACON_TIMING_REQ = 0x12,
        /*!
         * BeaconFreqAns
         */
        MOTE_MAC_BEACON_FREQ_ANS = 0x13,
    } LoRaMacMoteCmd_t;

    /*!
     * LoRaMAC server MAC commands
     *
     * LoRaWAN Specification V1.1.0 chapter 5, table 4
     */
    typedef enum eLoRaMacSrvCmd
    {
        /*!
         * ResetInd
         */
        SRV_MAC_RESET_CONF = 0x01,
        /*!
         * LinkCheckAns
         */
        SRV_MAC_LINK_CHECK_ANS = 0x02,
        /*!
         * LinkADRReq
         */
        SRV_MAC_LINK_ADR_REQ = 0x03,
        /*!
         * DutyCycleReq
         */
        SRV_MAC_DUTY_CYCLE_REQ = 0x04,
        /*!
         * RXParamSetupReq
         */
        SRV_MAC_RX_PARAM_SETUP_REQ = 0x05,
        /*!
         * DevStatusReq
         */
        SRV_MAC_DEV_STATUS_REQ = 0x06,
        /*!
         * NewChannelReq
         */
        SRV_MAC_NEW_CHANNEL_REQ = 0x07,
        /*!
         * RXTimingSetupReq
         */
        SRV_MAC_RX_TIMING_SETUP_REQ = 0x08,
        /*!
         * NewChannelReq
         */
        SRV_MAC_TX_PARAM_SETUP_REQ = 0x09,
        /*!
         * DlChannelReq
         */
        SRV_MAC_DL_CHANNEL_REQ = 0x0A,
        /*!
         * RekeyConf
         */
        SRV_MAC_REKEY_CONF = 0x0B,
        /*!
         * ADRParamSetupReq
         */
        SRV_MAC_ADR_PARAM_SETUP_REQ = 0x0C,
        /*!
         * ForceRejoinReq
         */
        SRV_MAC_FORCE_REJOIN_REQ = 0x0E,
        /*!
         * RejoinParamSetupReq
         */
        SRV_MAC_REJOIN_PARAM_REQ = 0x0F,
        /*!
         * DeviceModeConf ( Class C only )
         */
        SRV_MAC_DEVICE_MODE_CONF = 0x20,
        /*!
         * DeviceTimeAns
         */
        SRV_MAC_DEVICE_TIME_ANS = 0x0D,
        /*!
         * PingSlotInfoAns
         */
        SRV_MAC_PING_SLOT_INFO_ANS = 0x10,
        /*!
         * PingSlotChannelReq
         */
        SRV_MAC_PING_SLOT_CHANNEL_REQ = 0x11,
        /*!
         * BeaconTimingAns
         */
        SRV_MAC_BEACON_TIMING_ANS = 0x12,
        /*!
         * BeaconFreqReq
         */
        SRV_MAC_BEACON_FREQ_REQ = 0x13,
    } LoRaMacSrvCmd_t;

    /*!
     * LoRaMAC Battery level indicator
     */
    typedef enum eLoRaMacBatteryLevel
    {
        /*!
         * External power source
         */
        BAT_LEVEL_EXT_SRC = 0x00,
        /*!
         * Battery level empty
         */
        BAT_LEVEL_EMPTY = 0x01,
        /*!
         * Battery level full
         */
        BAT_LEVEL_FULL = 0xFE,
        /*!
         * Battery level - no measurement available
         */
        BAT_LEVEL_NO_MEASURE = 0xFF,
    } LoRaMacBatteryLevel_t;

    typedef struct
    {
        /*!
         * Channel id.
         */
        uint8_t ChannelId;

        /*!
         * Frequency in Hz
         */
        uint32_t Frequency;

        /*!
         * Minimum data rate
         *
         * LoRaWAN Regional Parameters V1.0.2rB
         *
         * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
         */
        uint8_t MinDR : 4;
        /*!
         * Maximum data rate
         *
         * LoRaWAN Regional Parameters V1.0.2rB
         *
         * The allowed ranges are region specific. Please refer to \ref DR_0 to \ref DR_15 for details.
         */
        uint8_t MaxDR : 4;
    } NewChannelReqParams_t;

    typedef struct
    {
        void (*on_mac_cmd_rx_timing_setup_req)(uint8_t delay_in_sec);
        void (*on_mac_cmd_device_time_ans)(uint32_t epoch, uint8_t fraction_of_1_to_256_sec);
        void (*on_mac_cmd_new_channel_req)(NewChannelReqParams_t channel_params);
        void (*on_mac_cmd_link_check_ans)(uint8_t margin, uint8_t gateway_count);

        /**
         * TODO: Add define function pointers of following MAC Commands
         *     - LinkADRReq
         *     - DutyCycleReq
         *     - RXParamSetupReq
         *     - TXParamSetupReq
         *     - DlChannelReq
         */
    } LoRaMacCommandCallbacks_t;

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_TYPES_H__
