#include "lib_pedbFirmware.h"
#include <SPI.h>
#include <Ethernet.h>
#include <String.h>

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
    parseUrlAndExecuteCommand (readURL()); 
    closeClient();
}
