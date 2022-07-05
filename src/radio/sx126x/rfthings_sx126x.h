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

#include "../rfthings_radio.h"
#include "sx126x_driver/sx126x.h"
#include "sx126x_driver/sx126x_hal.h"
#include "sx126x_driver/sx126x_regs.h"

#include <RTC.h>

class rfthings_sx126x : public rfthings_radio {
        public:
        rfthings_sx126x(byte nss_pin, byte rst_pin, byte busy_pin, byte dio1_pin, byte rxen_pin);
        ~rfthings_sx126x(void);

        rft_status_t init(rft_region_t region);

        rft_status_t send_lora(byte *payload, uint32_t payload_len, uint32_t timeout, void (*tx_func)());
        rft_status_t receive_lora(byte *payload, uint32_t payload_len, uint32_t timeout, void(*rx_func)());
        rft_status_t send_uplink(byte *payload, uint32_t payload_len, void (*tx_func)(), void(*rx_func)());
        rft_status_t send_join_request(void (*tx_func)(), void(*rx_func)());
        rft_status_t check_hardware(void);

        void start_continuous_wave(void);
        void stop_continuous_wave(void);
        rft_status_t sweep_continuous_wave(uint32_t start_freq, uint32_t stop_freq, uint32_t step, uint16_t duration);

        void sleep(void);
        void wake_up(void);

        // LoRaWAN relay application
        rft_status_t relay(byte *payload, uint32_t payload_len, void (*rx_func)(), void (*sleep_func)());
        static void irq_relay(void);

        void set_lora_pkt_param(sx126x_pkt_params_lora_t param);

    private:
        sx126x_hal_t sx126x_hal;

        void calibrate_image(uint32_t frequency);

        static sx126x_lora_sf_t map_spreading_factor(rft_lora_spreading_factor_t spreading_factor);
        static sx126x_lora_bw_t map_bandwidth(rft_lora_bandwidth_t bandwidth);
        static sx126x_lora_cr_t map_coding_rate(rft_lora_coding_rate_t coding_rate);

        uint8_t compute_lora_ldro(void);

        // LoRaWAN relay application
        // volatile bool detect_preamble;
};
