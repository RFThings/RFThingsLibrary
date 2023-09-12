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

#include "rfthings_sx126X.h"

volatile bool detect_preamble;

rfthings_sx126x::rfthings_sx126x(byte nss_pin, byte rst_pin, byte busy_pin, byte dio1_pin, byte antenna_switch_pin)
{
	sx126x_hal.nss = nss_pin;
	sx126x_hal.reset = rst_pin;
	sx126x_hal.busy = busy_pin;
	sx126x_hal.irq = dio1_pin;
	sx126x_hal.antenna_switch = antenna_switch_pin;
}

rfthings_sx126x::~rfthings_sx126x(void)
{
}

void rfthings_sx126x::sleep(void)
{
	sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);
}

void rfthings_sx126x::wake_up(void)
{
	sx126x_hal_wakeup(&sx126x_hal);
}

rft_status_t rfthings_sx126x::init(rft_region_t region)
{
	if (create_params_by_region(region) != RFT_STATUS_OK)
	{
		return RFT_STATUS_ERROR_INVALID_REGION;
	}

	pinMode(sx126x_hal.reset, OUTPUT);
	pinMode(sx126x_hal.busy, INPUT);
	pinMode(sx126x_hal.irq, INPUT_PULLUP);
	pinMode(sx126x_hal.nss, OUTPUT);
	pinMode(sx126x_hal.antenna_switch, OUTPUT);

	SPI.begin();
	digitalWrite(sx126x_hal.nss, HIGH);

	digitalWrite(sx126x_hal.antenna_switch, LOW);

	sx126x_hal_reset(&sx126x_hal);

	if (check_hardware() != RFT_STATUS_OK)
	{
		return RFT_STATUS_ERROR_HARDWARE;
	}

	sx126x_set_reg_mode(&sx126x_hal, SX126X_REG_MODE_DCDC);

	sx126x_cfg_tx_clamp(&sx126x_hal);

	sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

	sx126x_set_dio2_as_rf_sw_ctrl(&sx126x_hal, true);

	sx126x_set_dio3_as_tcxo_ctrl(&sx126x_hal, SX126X_TCXO_CTRL_1_7V, 320);

	sx126x_cal(&sx126x_hal, 0x7f);

	calibrate_image(lora_params.frequency);

	sx126x_set_lora_sync_word(&sx126x_hal, lora_params.syncword);

	return RFT_STATUS_OK;
}

rft_status_t rfthings_sx126x::check_hardware(void)
{
	uint16_t syncword;
	uint8_t buffer[2] = {0x00, 0x00};

	sx126x_read_register(&sx126x_hal, SX126X_REG_LR_SYNCWORD, buffer, 2);

	syncword = ((uint16_t)buffer[0] << 8) | buffer[1];

	if (syncword != 0x1424)
	{
		return RFT_STATUS_ERROR_HARDWARE;
	}

	return RFT_STATUS_OK;
}

rft_status_t rfthings_sx126x::send_lora(byte *payload, uint32_t payload_len, uint32_t timeout, void (*tx_func)())
{
	sx126x_wakeup(&sx126x_hal);
	sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

	sx126x_irq_mask_t irq_status;
	uint16_t symbol_len;

	symbol_len = 8;
	if (lora_params.send_to_relay)
	{
		symbol_len = 8 + (sx126x_get_lora_bw_in_hz(map_bandwidth(lora_params.bandwidth)) / (1 << map_spreading_factor(lora_params.spreading_factor)));
	}

	sx126x_set_pkt_type(&sx126x_hal, SX126X_PKT_TYPE_LORA);

	sx126x_mod_params_lora_t lora_mod_params;
	lora_mod_params.sf = map_spreading_factor(lora_params.spreading_factor);
	lora_mod_params.bw = map_bandwidth(lora_params.bandwidth);
	lora_mod_params.cr = map_coding_rate(lora_params.coding_rate);
	// lora_mod_params.ldro = 0x01;
	lora_mod_params.ldro = compute_lora_ldro();
	sx126x_set_lora_mod_params(&sx126x_hal, &lora_mod_params);

	sx126x_pkt_params_lora_t lora_pkt_params;
	lora_pkt_params.preamble_len_in_symb = symbol_len;
	lora_pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT;
	lora_pkt_params.pld_len_in_bytes = payload_len;
	lora_pkt_params.crc_is_on = true;
	lora_pkt_params.invert_iq_is_on = false;
	sx126x_set_lora_pkt_params(&sx126x_hal, &lora_pkt_params);

	sx126x_pa_cfg_params_t pa_config;
	pa_config.pa_duty_cycle = 0x04;
	pa_config.hp_max = 0x07;
	pa_config.device_sel = 0x00;
	pa_config.pa_lut = 0x01;
	sx126x_set_pa_cfg(&sx126x_hal, &pa_config);

	sx126x_set_ocp_value(&sx126x_hal, 0x38);

	sx126x_set_tx_params(&sx126x_hal, lora_params.tx_power, SX126X_RAMP_10_US);
	sx126x_set_rf_freq(&sx126x_hal, lora_params.frequency);

	sx126x_set_dio_irq_params(&sx126x_hal, 0x01, 0x01, 0x00, 0x00);
	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

	sx126x_set_buffer_base_address(&sx126x_hal, 0x00, 0x00);
	sx126x_write_buffer(&sx126x_hal, 0x00, payload, payload_len);

	if (tx_func != NULL)
	{
		tx_func();
	}
	sx126x_set_tx(&sx126x_hal, timeout);

	while (1)
	{
		sx126x_get_irq_status(&sx126x_hal, &irq_status);
		if (irq_status & SX126X_IRQ_TX_DONE)
		{
			break;
		}
		if (irq_status & SX126X_IRQ_TIMEOUT)
		{
			return RFT_STATUS_TX_TIMEOUT;
		}
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

	// sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);

	return RFT_STATUS_TX_DONE;
}

rft_status_t rfthings_sx126x::receive_lora(byte *payload, uint32_t payload_len, uint32_t timeout, void (*rx_func)())
{
	sx126x_wakeup(&sx126x_hal);
	sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

	sx126x_irq_mask_t irq_status;

	sx126x_set_pkt_type(&sx126x_hal, SX126X_PKT_TYPE_LORA);

	digitalWrite(sx126x_hal.antenna_switch, HIGH);

	sx126x_mod_params_lora_t lora_mod_params;
	lora_mod_params.sf = map_spreading_factor(lora_params.spreading_factor);
	lora_mod_params.bw = map_bandwidth(lora_params.bandwidth);
	lora_mod_params.cr = map_coding_rate(lora_params.coding_rate);
	lora_mod_params.ldro = compute_lora_ldro();
	sx126x_set_lora_mod_params(&sx126x_hal, &lora_mod_params);

	sx126x_pkt_params_lora_t lora_pkt_params;
	lora_pkt_params.preamble_len_in_symb = 8;
	lora_pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT;
	lora_pkt_params.pld_len_in_bytes = 255;
	lora_pkt_params.crc_is_on = true;
	lora_pkt_params.invert_iq_is_on = true;
	sx126x_set_lora_pkt_params(&sx126x_hal, &lora_pkt_params);

	sx126x_cfg_rx_boosted(&sx126x_hal, true);

	sx126x_set_rf_freq(&sx126x_hal, lora_params.frequency);

	sx126x_set_dio_irq_params(&sx126x_hal, 0x0fff, 0x0fff, 0x00, 0x00);

	sx126x_set_buffer_base_address(&sx126x_hal, 0x00, 0x00);

	if (rx_func != NULL)
	{
		rx_func();
	}
	sx126x_set_rx(&sx126x_hal, timeout);

	while (1)
	{
		sx126x_get_irq_status(&sx126x_hal, &irq_status);
		if (irq_status & SX126X_IRQ_RX_DONE)
		{
			break;
		}
		if (irq_status & SX126X_IRQ_TIMEOUT)
		{
			break;
		}
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

	if (irq_status & SX126X_IRQ_TIMEOUT)
	{
		return RFT_STATUS_RX_TIMEOUT;
	}

	sx126x_pkt_status_lora_t pkt_status;
	sx126x_get_lora_pkt_status(&sx126x_hal, &pkt_status);

	lora_params.rssi = pkt_status.rssi_pkt_in_dbm;
	lora_params.snr = pkt_status.snr_pkt_in_db;
	lora_params.signal_rssi = pkt_status.signal_rssi_pkt_in_dbm;

	sx126x_rx_buffer_status_t rx_buffer_status;
	sx126x_get_rx_buffer_status(&sx126x_hal, &rx_buffer_status);

	payload_len = rx_buffer_status.pld_len_in_bytes;
	sx126x_read_buffer(&sx126x_hal, rx_buffer_status.buffer_start_pointer, payload, payload_len);

	return RFT_STATUS_OK;
}

void rfthings_sx126x::set_lora_pkt_param(sx126x_pkt_params_lora_t param)
{
	sx126x_set_lora_pkt_params(&sx126x_hal, &param);
}

rft_status_t rfthings_sx126x::relay(byte *payload, uint32_t &payload_len, void (*rx_func)(), void (*sleep_func)())
{
	return (this->relay(&(this->lora_params), payload, payload_len, rx_func, sleep_func));
}

rft_status_t rfthings_sx126x::relay(rft_lora_params_t *relay_lora_params, byte *payload, uint32_t &payload_len, void (*rx_func)(), void (*sleep_func)())
{
	/**
	 * TODO: Parameters validation!
	 * relay_lora_params: Nom NULL Pointer, also need to validate following values:
	 *    +	relay_sleep_interval_us
	 *    + relay_rx_symbol
	 *    + relay_max_rx_packet_length
	 * 
	 * payload: Non NULL pointer
	 */

	rft_status_t return_value = RFT_STATUS_OK;

	sx126x_wakeup(&sx126x_hal);
	sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

	// Preable length for Relay application
	uint16_t preamble_length = 8 + (sx126x_get_lora_bw_in_hz(map_bandwidth(relay_lora_params->bandwidth)) / (1 << map_spreading_factor(relay_lora_params->spreading_factor)));

	sx126x_set_pkt_type(&sx126x_hal, SX126X_PKT_TYPE_LORA);

	digitalWrite(sx126x_hal.antenna_switch, HIGH);

	sx126x_mod_params_lora_t lora_mod_params;
	lora_mod_params.sf = map_spreading_factor(relay_lora_params->spreading_factor);
	lora_mod_params.bw = map_bandwidth(relay_lora_params->bandwidth);
	lora_mod_params.cr = map_coding_rate(relay_lora_params->coding_rate);
	lora_mod_params.ldro = compute_lora_ldro(*relay_lora_params);
	sx126x_set_lora_mod_params(&sx126x_hal, &lora_mod_params);

	sx126x_pkt_params_lora_t lora_pkt_params;
	lora_pkt_params.preamble_len_in_symb = preamble_length; // Calculated
	lora_pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT;
	lora_pkt_params.pld_len_in_bytes = relay_lora_params->relay_max_rx_packet_length;
	lora_pkt_params.crc_is_on = true;
	lora_pkt_params.invert_iq_is_on = false;
	sx126x_set_lora_pkt_params(&sx126x_hal, &lora_pkt_params);

	sx126x_set_lora_sync_word(&sx126x_hal, relay_lora_params->syncword);

	sx126x_cfg_rx_boosted(&sx126x_hal, true);

	sx126x_set_rf_freq(&sx126x_hal, relay_lora_params->frequency);

	sx126x_set_dio_irq_params(&sx126x_hal, 0x0fff, 0x0fff, 0x00, 0x00);

	sx126x_set_buffer_base_address(&sx126x_hal, 0x00, 0x00);

	sx126x_set_reg_mode(&sx126x_hal, SX126X_REG_MODE_DCDC);

	sx126x_set_lora_symb_nb_timeout(&sx126x_hal, 0);

	if (rx_func != NULL)
	{
		rx_func();
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);
	detect_preamble = false;
	attachInterrupt(digitalPinToInterrupt(sx126x_hal.irq), (voidFuncPtr)(rfthings_sx126x::irq_relay), RISING);

	uint32_t rxTime = (1e6 * relay_lora_params->relay_rx_symbol * (1 << map_spreading_factor(relay_lora_params->spreading_factor)) / sx126x_get_lora_bw_in_hz(map_bandwidth(relay_lora_params->bandwidth))) / T_STEP;
	uint32_t sleepTime = (relay_lora_params->relay_sleep_interval_us) / T_STEP - rxTime;
	sx126x_set_rx_duty_cycle_with_timings_in_rtc_step(&sx126x_hal, rxTime, sleepTime);

	if (sleep_func != NULL)
	{
		sleep_func();
	}
	else
	{
		while (!detect_preamble)
		{
		};
		detect_preamble = false;
	}
	detachInterrupt(digitalPinToInterrupt(sx126x_hal.irq));

	sx126x_irq_mask_t irq_status;
	sx126x_get_irq_status(&sx126x_hal, &irq_status);
	if (irq_status & SX126X_IRQ_PREAMBLE_DETECTED)
	{
		sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

		sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

		sx126x_set_rx(&sx126x_hal, 2000);

		while (1)
		{
			sx126x_get_irq_status(&sx126x_hal, &irq_status);
			if (irq_status & SX126X_IRQ_RX_DONE)
			{
				break;
			}
			if (irq_status & SX126X_IRQ_TIMEOUT)
			{
				break;
			}
			delay(10);
		}

		if (irq_status & SX126X_IRQ_TIMEOUT)
		{
			// Timeout
			payload_len = 0;
			return_value = RFT_STATUS_RX_TIMEOUT;
		}
		else
		{
			// Rx Done
			sx126x_set_standby(&sx126x_hal, SX126X_STANDBY_CFG_RC);

			sx126x_pkt_status_lora_t pkt_status;
			sx126x_get_lora_pkt_status(&sx126x_hal, &pkt_status);

			relay_lora_params->rssi = pkt_status.rssi_pkt_in_dbm;
			relay_lora_params->snr = pkt_status.snr_pkt_in_db;
			relay_lora_params->signal_rssi = pkt_status.signal_rssi_pkt_in_dbm;

			sx126x_rx_buffer_status_t rx_buffer_status;
			sx126x_get_rx_buffer_status(&sx126x_hal, &rx_buffer_status);

			payload_len = rx_buffer_status.pld_len_in_bytes;
			sx126x_read_buffer(&sx126x_hal, rx_buffer_status.buffer_start_pointer, payload, payload_len);

			return_value = RFT_STATUS_OK;
		}
	}
	else
	{
		payload_len = 0;
		return_value = RFT_STATUS_PREAMBLE_DETECT_FAIL;
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);
	digitalWrite(sx126x_hal.antenna_switch, 0);

	sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);

	return return_value;
}

rft_status_t rfthings_sx126x::send_uplink(byte *payload, uint8_t& payload_len, void (*tx_func)(), void(*rx_func)(), bool send_to_relay)
{
	// buld LoRaWAN packet
	unsigned char lorawan_packet[9 + 255 + 4];

	uint8_t packet_len = build_uplink_packet(payload, payload_len, lorawan_packet);

	// send LoRa

	if (send_lora(lorawan_packet, packet_len, 2000, tx_func) == RFT_STATUS_TX_TIMEOUT)
	{
		return RFT_STATUS_TX_TIMEOUT;
	}

	lorawan_params.framecounter_uplink++;

	if (this->lora_params.send_to_relay)
	{
		// No need downlink for this mode
		return RFT_STATUS_OK;
	}

	// receive downlink
	bool receive_downlink = false;

	sx126x_irq_mask_t irq_status;

	digitalWrite(sx126x_hal.antenna_switch, HIGH);
	if (rx_func != NULL)
	{
		rx_func();
	}

	// RX1 window
	sx126x_pkt_params_lora_t lora_pkt_params;
	lora_pkt_params.preamble_len_in_symb = 8;
	lora_pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT;
	lora_pkt_params.pld_len_in_bytes = 255;
	lora_pkt_params.crc_is_on = true;
	lora_pkt_params.invert_iq_is_on = true;
	sx126x_set_lora_pkt_params(&sx126x_hal, &lora_pkt_params);

	sx126x_cfg_rx_boosted(&sx126x_hal, true);

	sx126x_set_rf_freq(&sx126x_hal, lorawan_params.rx1_frequency);

	sx126x_set_dio_irq_params(&sx126x_hal, 0x0fff, 0x0fff, 0x00, 0x00);

	sx126x_set_buffer_base_address(&sx126x_hal, 0x00, 0x00);

	delay(lorawan_params.rx1_delay - 200);

	sx126x_set_rx(&sx126x_hal, 500);

	while (1)
	{
		sx126x_get_irq_status(&sx126x_hal, &irq_status);
		if (irq_status & SX126X_IRQ_RX_DONE)
		{
			break;
		}
		if (irq_status & SX126X_IRQ_TIMEOUT)
		{
			break;
		}
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

	if (irq_status & SX126X_IRQ_RX_DONE)
	{
		receive_downlink = true;
	}

	if (!receive_downlink)
	{
		// RX2 window
		sx126x_mod_params_lora_t lora_mod_params;
		lora_mod_params.sf = map_spreading_factor(lorawan_params.rx2_spreading_factor);
		lora_mod_params.bw = map_bandwidth(lorawan_params.rx2_bandwidth);
		lora_mod_params.cr = map_coding_rate(lora_params.coding_rate);
		lora_mod_params.ldro = compute_lora_ldro();
		sx126x_set_lora_mod_params(&sx126x_hal, &lora_mod_params);

		sx126x_set_rf_freq(&sx126x_hal, lorawan_params.rx2_frequency);

		delay(800);

		sx126x_set_rx(&sx126x_hal, 500);

		while (1)
		{
			sx126x_get_irq_status(&sx126x_hal, &irq_status);
			if (irq_status & SX126X_IRQ_RX_DONE)
			{
				break;
			}
			if (irq_status & SX126X_IRQ_TIMEOUT)
			{
				break;
			}
		}

		sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

		if (irq_status & SX126X_IRQ_RX_DONE)
		{
			receive_downlink = true;
		}
	}

	digitalWrite(sx126x_hal.antenna_switch, LOW);

	// parse downlink
	if (!receive_downlink)
	{
		// sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);
		lorawan_params.rx_length = 0;
		return RFT_STATUS_RX_TIMEOUT;
	}

	sx126x_pkt_status_lora_t pkt_status;
	sx126x_get_lora_pkt_status(&sx126x_hal, &pkt_status);

	lora_params.rssi = pkt_status.rssi_pkt_in_dbm;
	lora_params.snr = pkt_status.snr_pkt_in_db;
	lora_params.signal_rssi = pkt_status.signal_rssi_pkt_in_dbm;

	sx126x_rx_buffer_status_t rx_buffer_status;
	sx126x_get_rx_buffer_status(&sx126x_hal, &rx_buffer_status);

	payload_len = rx_buffer_status.pld_len_in_bytes;
	sx126x_read_buffer(&sx126x_hal, rx_buffer_status.buffer_start_pointer, payload, payload_len);

	// sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);

	return parse_downlink(payload, payload_len);
}

rft_status_t rfthings_sx126x::send_join_request(void (*tx_func)(), void (*rx_func)())
{
	// buld join request
	unsigned char lorawan_packet[9 + 255 + 4];

	uint8_t packet_len = build_join_request(lorawan_packet);

	// send lora
	if (send_lora(lorawan_packet, packet_len, 2000, tx_func) == RFT_STATUS_TX_TIMEOUT)
	{
		return RFT_STATUS_TX_TIMEOUT;
	}

	// receive downlink
	bool receive_downlink = false;

	uint16_t irq_status;

	digitalWrite(sx126x_hal.antenna_switch, HIGH);
	if (rx_func != NULL)
	{
		rx_func();
	}

	// RX1 window
	sx126x_pkt_params_lora_t lora_pkt_params;
	lora_pkt_params.preamble_len_in_symb = 8;
	lora_pkt_params.header_type = SX126X_LORA_PKT_EXPLICIT;
	lora_pkt_params.pld_len_in_bytes = 255;
	lora_pkt_params.crc_is_on = true;
	lora_pkt_params.invert_iq_is_on = true;
	sx126x_set_lora_pkt_params(&sx126x_hal, &lora_pkt_params);

	sx126x_cfg_rx_boosted(&sx126x_hal, true);

	sx126x_set_rf_freq(&sx126x_hal, lorawan_params.rx1_frequency);

	sx126x_set_dio_irq_params(&sx126x_hal, 0x0fff, 0x0fff, 0x00, 0x00);

	sx126x_set_buffer_base_address(&sx126x_hal, 0x00, 0x00);

	delay(lorawan_params.rx1_delay - 200);

	sx126x_set_rx(&sx126x_hal, 500);

	while (1)
	{
		sx126x_get_irq_status(&sx126x_hal, &irq_status);
		if (irq_status & SX126X_IRQ_RX_DONE)
		{
			break;
		}
		if (irq_status & SX126X_IRQ_TIMEOUT)
		{
			break;
		}
	}

	sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

	if (irq_status & SX126X_IRQ_RX_DONE)
	{
		receive_downlink = true;
	}

	if (!receive_downlink)
	{
		// RX2 window
		sx126x_mod_params_lora_t lora_mod_params;
		lora_mod_params.sf = map_spreading_factor(lorawan_params.rx2_spreading_factor);
		lora_mod_params.bw = map_bandwidth(lorawan_params.rx2_bandwidth);
		lora_mod_params.cr = map_coding_rate(lora_params.coding_rate);
		lora_mod_params.ldro = compute_lora_ldro();
		sx126x_set_lora_mod_params(&sx126x_hal, &lora_mod_params);

		sx126x_set_rf_freq(&sx126x_hal, lorawan_params.rx2_frequency);

		delay(800);

		sx126x_set_rx(&sx126x_hal, 500);

		while (1)
		{
			sx126x_get_irq_status(&sx126x_hal, &irq_status);
			if (irq_status & SX126X_IRQ_RX_DONE)
			{
				break;
			}
			if (irq_status & SX126X_IRQ_TIMEOUT)
			{
				break;
			}
		}

		sx126x_clear_irq_status(&sx126x_hal, SX126X_IRQ_ALL);

		if (irq_status & SX126X_IRQ_RX_DONE)
		{
			receive_downlink = true;
		}
	}

	digitalWrite(sx126x_hal.antenna_switch, LOW);

	// parse downlink
	if (!receive_downlink)
	{
		// sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);
		lorawan_params.rx_length = 0;
		return RFT_STATUS_RX_TIMEOUT;
	}

	sx126x_pkt_status_lora_t pkt_status;
	sx126x_get_lora_pkt_status(&sx126x_hal, &pkt_status);

	lora_params.rssi = pkt_status.rssi_pkt_in_dbm;
	lora_params.snr = pkt_status.snr_pkt_in_db;
	lora_params.signal_rssi = pkt_status.signal_rssi_pkt_in_dbm;

	sx126x_rx_buffer_status_t rx_buffer_status;
	sx126x_get_rx_buffer_status(&sx126x_hal, &rx_buffer_status);

	packet_len = rx_buffer_status.pld_len_in_bytes;
	sx126x_read_buffer(&sx126x_hal, rx_buffer_status.buffer_start_pointer, lorawan_packet, packet_len);

	// sx126x_set_sleep(&sx126x_hal, SX126X_SLEEP_CFG_WARM_START);

	return parse_join_accept(lorawan_packet, packet_len);
}

void rfthings_sx126x::start_continuous_wave(void)
{
}

void rfthings_sx126x::stop_continuous_wave(void)
{
}

rft_status_t rfthings_sx126x::sweep_continuous_wave(uint32_t start_freq, uint32_t stop_freq, uint32_t step, uint16_t duration)
{
	return RFT_STATUS_OK;
}

void rfthings_sx126x::calibrate_image(uint32_t frequency)
{
	uint8_t calFreq[2];
	calFreq[0] = 0xD7;
	calFreq[1] = 0xDB;

	if (lora_params.frequency > 900000000)
	{
		calFreq[0] = 0xE1;
		calFreq[1] = 0xE9;
	}
	else if (lora_params.frequency > 850000000)
	{
		calFreq[0] = 0xD7;
		calFreq[1] = 0xDB;
	}
	else if (lora_params.frequency > 770000000)
	{
		calFreq[0] = 0xC1;
		calFreq[1] = 0xC5;
	}
	else if (lora_params.frequency > 460000000)
	{
		calFreq[0] = 0x75;
		calFreq[1] = 0x81;
	}
	else if (lora_params.frequency > 425000000)
	{
		calFreq[0] = 0x6B;
		calFreq[1] = 0x6F;
	}

	sx126x_cal_img(&sx126x_hal, calFreq[0], calFreq[1]);
}

sx126x_lora_sf_t rfthings_sx126x::map_spreading_factor(rft_lora_spreading_factor_t spreading_factor)
{
	switch (spreading_factor)
	{
	case RFT_LORA_SPREADING_FACTOR_5:
		return SX126X_LORA_SF5;
		break;
	case RFT_LORA_SPREADING_FACTOR_6:
		return SX126X_LORA_SF6;
		break;
	case RFT_LORA_SPREADING_FACTOR_7:
		return SX126X_LORA_SF7;
		break;
	case RFT_LORA_SPREADING_FACTOR_8:
		return SX126X_LORA_SF8;
		break;
	case RFT_LORA_SPREADING_FACTOR_9:
		return SX126X_LORA_SF9;
		break;
	case RFT_LORA_SPREADING_FACTOR_10:
		return SX126X_LORA_SF10;
		break;
	case RFT_LORA_SPREADING_FACTOR_11:
		return SX126X_LORA_SF11;
		break;
	case RFT_LORA_SPREADING_FACTOR_12:
		return SX126X_LORA_SF12;
		break;
	default:
		return SX126X_LORA_SF7;
		break;
	}
}

sx126x_lora_bw_t rfthings_sx126x::map_bandwidth(rft_lora_bandwidth_t bandwidth)
{
	switch (bandwidth)
	{
	case RFT_LORA_BANDWIDTH_10KHZ:
		return SX126X_LORA_BW_010;
		break;
	case RFT_LORA_BANDWIDTH_15KHZ:
		return SX126X_LORA_BW_015;
		break;
	case RFT_LORA_BANDWIDTH_20KHZ:
		return SX126X_LORA_BW_020;
		break;
	case RFT_LORA_BANDWIDTH_31KHZ:
		return SX126X_LORA_BW_031;
		break;
	case RFT_LORA_BANDWIDTH_41KHZ:
		return SX126X_LORA_BW_041;
		break;
	case RFT_LORA_BANDWIDTH_62KHZ:
		return SX126X_LORA_BW_062;
		break;
	case RFT_LORA_BANDWIDTH_125KHZ:
		return SX126X_LORA_BW_125;
		break;
	case RFT_LORA_BANDWIDTH_250KHZ:
		return SX126X_LORA_BW_250;
		break;
	case RFT_LORA_BANDWIDTH_500KHZ:
		return SX126X_LORA_BW_500;
		break;
	default:
		return SX126X_LORA_BW_125;
		break;
	}
}

sx126x_lora_cr_t rfthings_sx126x::map_coding_rate(rft_lora_coding_rate_t coding_rate)
{
	switch (coding_rate)
	{
	case RFT_LORA_CODING_RATE_4_5:
		return SX126X_LORA_CR_4_5;
		break;
	case RFT_LORA_CODING_RATE_4_6:
		return SX126X_LORA_CR_4_6;
		break;
	case RFT_LORA_CODING_RATE_4_7:
		return SX126X_LORA_CR_4_7;
		break;
	case RFT_LORA_CODING_RATE_4_8:
		return SX126X_LORA_CR_4_8;
		break;
	default:
		return SX126X_LORA_CR_4_5;
		break;
	}
}

uint8_t rfthings_sx126x::compute_lora_ldro(void)
{
	switch (lora_params.bandwidth)
	{
	case RFT_LORA_BANDWIDTH_500KHZ:
		return 0x00;
		break;
	case RFT_LORA_BANDWIDTH_250KHZ:
		if (lora_params.spreading_factor == RFT_LORA_SPREADING_FACTOR_12)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_125KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_11)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_62KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_10)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_41KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_9)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_31KHZ:
	case RFT_LORA_BANDWIDTH_20KHZ:
	case RFT_LORA_BANDWIDTH_15KHZ:
	case RFT_LORA_BANDWIDTH_10KHZ:
		return 0x01;
		break;
	default:
		return 0x00;
		break;
	}
}

uint8_t rfthings_sx126x::compute_lora_ldro(rft_lora_params_t lora_params)
{
	switch (lora_params.bandwidth)
	{
	case RFT_LORA_BANDWIDTH_500KHZ:
		return 0x00;
		break;
	case RFT_LORA_BANDWIDTH_250KHZ:
		if (lora_params.spreading_factor == RFT_LORA_SPREADING_FACTOR_12)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_125KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_11)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_62KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_10)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_41KHZ:
		if (lora_params.spreading_factor >= RFT_LORA_SPREADING_FACTOR_9)
		{
			return 0x01;
		}
		else
		{
			return 0x00;
		}
		break;
	case RFT_LORA_BANDWIDTH_31KHZ:
	case RFT_LORA_BANDWIDTH_20KHZ:
	case RFT_LORA_BANDWIDTH_15KHZ:
	case RFT_LORA_BANDWIDTH_10KHZ:
		return 0x01;
		break;
	default:
		return 0x00;
		break;
	}
}
