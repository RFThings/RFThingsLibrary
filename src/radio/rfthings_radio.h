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

#include "Arduino.h"
#include "SPI.h"
#include "../RFThings.h"
#include "../aes/Encrypt.h"
#include "../aes/AES-128.h"

class rfthings_radio {
    public:
        // rfthings_radio(void);
        // ~rfthings_radio();
        
        // virtual rft_status_t init(void) = 0;
        
        // virtual rft_status_t send_lora(unsigned char *payload, uint8_t payload_len) = 0;
        // virtual rft_status_t send_lorawan(unsigned char *payload, uint8_t payload_len) = 0;
        // virtual rft_status_t check_hardware(void) = 0;

        // virtual void start_continuous_wave(void) = 0;
        // virtual void stop_continuous_wave(void) = 0;
        // virtual rft_status_t sweep_continuous_wave(uint32_t start_freq, uint32_t stop_freq, uint32_t step, uint16_t duration) = 0;
        
        // virtual void wait_for_busy(void) = 0;
        // virtual void sleep(void) = 0;
        // virtual void wake_up(void) = 0;

        // set LoRa tx parameters
        void set_tx_power(int8_t tx_power);
        void set_spreading_factor(rft_lora_spreading_factor_t spreading_factor);
        void set_coding_rate(rft_lora_coding_rate_t coding_rate);
        void set_bandwidth(rft_lora_bandwidth_t bandwidth);
        void set_syncword(rft_lora_syncword_t syncword);
        void set_frequency(uint32_t frequency);

        // get LoRa tx parameters
        int8_t get_tx_power(void);
        rft_lora_spreading_factor_t get_spreading_factor(void);
        rft_lora_coding_rate_t get_coding_rate(void);
        rft_lora_bandwidth_t get_bandwidth(void);
        rft_lora_syncword_t get_syncword(void);
        uint32_t get_frequency(void);
        
        // get LoRa rx parameters
        int8_t get_snr(void);
        int16_t get_rssi(void);
        int16_t get_signal_rssi(void);

        // LoRaWAN relay application
        void set_symbol_time(uint32_t symbol_time);
        void set_detect_symbol(uint8_t detect_symbol);

        // set LoRaWAN params
        void set_lorawan_activation_type(rft_lorawan_activation_type_t activation_type);
        void set_device_address(uint8_t dev_addr[4]);
        void set_network_session_key(uint8_t nwkS_key[16]);
        void set_application_session_key(uint8_t appS_key[16]);

        void set_devive_eui(uint8_t dev_eui[8]);
        void set_application_eui(uint8_t app_eui[8]);
        void set_application_key(uint8_t app_key[16]);

        void set_framecounter_size(rft_lorawan_framecounter_size_t framecounter_size);
        void set_tx_port(uint8_t tx_port);
        void set_rx1_delay(uint32_t rx1_delay);

        // get LoRaWAN params
        rft_lorawan_activation_type_t get_lorawan_activation_type(void);
        uint8_t* get_device_address(void);
        uint8_t* get_network_session_key(void);
        uint8_t* get_application_session_key(void);

        uint8_t* get_devive_eui(void);
        uint8_t* get_application_eui(void);
        uint8_t* get_application_key(void);

        rft_lorawan_framecounter_size_t get_framecounter_size(void);
        uint8_t get_tx_port(void);

        // get LoRaWAN params
        uint32_t get_framecounter_uplink(void);
        uint32_t get_framecounter_downlink(void);
        uint8_t get_rx_port(void);
        uint8_t get_rx_length(void);
    protected:
        rft_lora_params_t lora_params;
        rft_lorawan_params_t lorawan_params;

        rft_status_t create_params_by_region(rft_region_t region);
        uint8_t build_uplink_packet(unsigned char *payload, uint8_t payload_len, unsigned char *lorawan_packet);
        uint8_t build_join_request(unsigned char *lorawan_packet);

        rft_status_t parse_downlink(unsigned char *payload, uint8_t payload_len);
        rft_status_t parse_join_accept(unsigned char *payload, uint8_t payload_len);
};