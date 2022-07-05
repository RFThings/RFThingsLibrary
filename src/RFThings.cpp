#include "RFThings.h"

const char* rft_status_to_str(rft_status_t status) {
	switch (status) {
		case RFT_STATUS_OK:
			return "ok";
			break;
		case RFT_STATUS_TX_DONE:
			return "tx done";
			break;
		case RFT_STATUS_RX_DONE:
			return "rx done";
			break;
		case RFT_STATUS_WRONG_MIC:
			return "wrong MIC";
			break;
		case RFT_STATUS_TX_TIMEOUT:
			return "tx timeout";
			break;
		case RFT_STATUS_RX_TIMEOUT:
			return "rx timeout";
			break;
		case RFT_STATUS_ERROR_HARDWARE:
			return "hardware error";
			break;
		case RFT_STATUS_ERROR_INVALID_PARAM:
			return "invalid parameter";
			break;
		case RFT_STATUS_ERROR_INVALID_REGION:
			return "invalid region";
			break;
		case RFT_STATUS_PREAMBLE_DETECT_FAIL:
			return "detect preamble fail";
			break;
		case RFT_STATUS_ERROR_WRONG_LR11XX_FIRMWARE_VERSION:
			return "wrong lr11xx firmware versions";
			break;
		case RFT_STATUS_ERROR_INAVLID_LORAWAN_ACTIVATION_TYPE:
			return "invalid parameter";
			break;
		default:
			return "invalid status";
			break;
	}
}

void worship(void) {
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@@@@@@@@@@@@@,@@..@@,@@@@@@@@@@@@@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@)))@@@@@@@@@,@@..@@,@@@@@@@@@(((@@");
	Serial.println(" @@@@@@........................@@@@@@");
	Serial.println(" @..................................@");
	Serial.println(" @@................................@@");
	Serial.println(" @@@..............................@@@");
	Serial.println(" @@@@............................@@@@");
	Serial.println(" @@@@@@........................@@@@@@");
	Serial.println(" @@@@@@@@....................@@@@@@@@");
	Serial.println(" @@@@@@@@@..................@@@@@@@@@");
	Serial.println(" @@@@@@@@@..................@@@@@@@@@");
	Serial.println(" @@@@@@@,,,,,@@@@@@@@@@@@,,,,,@@@@@@@");
}
