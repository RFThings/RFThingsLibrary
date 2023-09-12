#ifndef __LORAMAC_COMMANDS_H__
#define __LORAMAC_COMMANDS_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>
#include "LoRaMacTypes.h"

#if  defined ( __GNUC__ )
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

uint8_t process_mac_command(uint8_t *payload, uint32_t payload_len, int8_t snr);

/*!
 * \brief Get the MAC command size with corresponding CID.
 *
 * \param[IN]   cid                - MAC command identifier
 *
 * \retval Size of the command.
 */
uint8_t LoRaMacCommandsGetCmdSize( uint8_t cid );

/**** MAC Command Callbacks ****/
__weak void on_mac_cmd_link_check_ans(uint8_t margin, uint8_t gateway_count);
__weak void on_mac_cmd_new_channel_req(NewChannelReqParams_t channel_params);
__weak void on_mac_cmd_rx_timing_setup_req(uint8_t delay_in_sec);
__weak void on_mac_cmd_device_time_ans(uint32_t epoch, uint8_t fraction_of_1_to_256_sec);

/**
 * TODO: Add define __weak declaration of following MAC Commands
 *     - LinkADRReq
 *     - DutyCycleReq
 *     - RXParamSetupReq
 *     - TXParamSetupReq
 *     - DlChannelReq
 */

#ifdef __cplusplus
}
#endif

#endif // __LORAMAC_COMMANDS_H__
