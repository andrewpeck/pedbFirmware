#include "lib_pedbFirmware.h"
#include <SPI.h>
#include <Ethernet.h>
#include <String.h>

/* USAGE INFO: 
 * the pedb Firmware will generates an Ethernet Client that will respond to a three digit 
 * command sequence of the form: 
 *      http://xx.xx.xx.xx?123
 *
 *  where 123 are three hexdecimal characters, converted to their respective numerical values,  defining the command.
 *  
 * The first byte in the sequence represents the command, according to the enum thus defined: 
 *  Consider the enum thus defined: 
 *  enum commands {
 *     CMD_READ_ADC,      // 0 
 *     CMD_POWER_ON,      // 1
 *     CMD_POWER_OFF,     // 2
 *     CMD_CHECK_DOOR,    // 4
 *     CMD_RESET          // 5
 * };
 * 
 * The second and third characters are context dependent: 
 *
 * For cmd==CMD_READ_ADC: 
 *      the second character represents the adc chip, and the third character represents the adc channel. 
 *      for example, if we wish to read chip 1, channel 12, we want a command of 01C
 *      if we wish to read chip 0, channel 15, we want a command of 01F 
 *
 * For cmd==CMD_POWER_ON or cmd==CMD_POWER_OFF 
 *      The second and third characters are taken together to represent a channel.. 
 *      
 *      for example, if we want to power on channel 14, we can issue 10E
 *      or if we wish to power on channel 18, we can issue 13F
 *
 * For other commands (These are without arguments): 
 *      we just issue the command (first character), regardless of the remaining characters
 *      , e.g. 400 to check door status, which will return ????1 or 0??? if the door is closed, and ???0 or 1??? if the
 *      door is opened. 
 *      
 *      or 500 to reset the ADCs. 
 */ 

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
