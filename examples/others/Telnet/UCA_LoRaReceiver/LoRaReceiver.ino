#include <SPI.h>
#include <LoRa.h>

void setup()
{
    Serial.begin(9600);
    while (!Serial)
        ;

    Serial.println("LoRa Receiver");

    LoRa.setPins(10, 8, 6);
    if (!LoRa.begin(868500000))
    {
        Serial.println("Starting LoRa failed!");
        while (1)
            ;
    }

    LoRa.setTxPower(14);
    LoRa.setSpreadingFactor(8);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setPreambleLength(8);
    LoRa.setSyncWord(0x12);
    LoRa.enableCrc();
    LoRa.disableInvertIQ();
}

void loop()
{
    // try to parse packet
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        // received a packet
        Serial.print("Received packet '");

        // read packet
        while (LoRa.available())
        {
            Serial.print((char)LoRa.read(), HEX);
        }

        // print RSSI of packet
        Serial.print("' with RSSI ");
        Serial.println(LoRa.packetRssi());
    }
}
