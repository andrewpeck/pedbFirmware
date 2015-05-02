#ifndef PEDB_FIRMWARE_H
#define PEDB_FIRMWARE_H

#include <Arduino.h>
#include <String.h>

/* */
enum commands {
    CMD_READ_ADC,
    CMD_POWER_ON,
    CMD_POWER_OFF,
    CMD_CHECK_DOOR,
    CMD_RESET
};

/* */
String readURL();

/* */
void parseUrlAndExecuteCommand (String url);

/* */
void openEthernetClient();

/* */
void closeEthernetClient();

/* */
void powerOn(int channel);

/*Read from the off-board ADC chips */
int  readValue(int chip, int channel);

/* */
void startEthernet();

/* */
void configureADC();

/* */
void setMacAddress();

/*Sends a command to the ADC */
void writeRegister(int chipNum, uint8_t thisRegister);

/* */
void configureAveraging();

/* */
void configurePins();

/* */
void startSPI();

/* */
void configurePowerSupplyLines();

/* */
void startEthernetServer();


/* */
void resetAllADCs();

/* */
void resetADCFIFO(int chip);

/* */
void checkDoor();

/* */
void powerOff(int channel);

/* */
void executeCommand(int cmd,int channel,int chip);

/* */
void configureSerial();

#endif
