#include <SPI.h>
#include <Ethernet.h>
#include <stdio.h>

#include "PEDBfirmwareV5.hpp"

/*default ip address, only used if DHCP fails */
IPAddress ip (10,90,90,5);

/* Initialize the Ethernet server library */
EthernetServer server(80);

void setup()
{
    configurePowerSupplyLines();
    startSPI();
    configurePins();
    configureADC();
    // configureAveraging(); /* Disabled --- needs to be debugged */
    configureSerial(); 

    startEthernet();
    setMacAddress();
    startServer();
}

void loop()
{
    openClient();
    parseUrlAndExecuteCommand (readURL())
    readAndParseURL();
    closeClient();
}
