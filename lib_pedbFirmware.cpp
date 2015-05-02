#include "lib_pedbFirmware.h"
#include "pedbFirmware_constants.h"

#include <SPI.h>
#include <Ethernet.h>

/*default ip address, only used if DHCP fails */
IPAddress ip (10,90,90,5);

/* Initialize the Ethernet server library */
EthernetServer server(80);

EthernetClient client;

int psline[18];
int chipSelectPin[4];

/* define 1 to put in DEBUG mode, 0 to turn off DEBUG mode.. */
#define DEBUG 1

#define debug_println(text) \
        do { if (DEBUG) Serial.println(text); } while (0)
#define debug_print(text) \
        do { if (DEBUG) Serial.print(text); } while (0)

void executeCommand (uint8_t cmd, uint16_t data)
{
    switch (cmd)
    {
        case CMD_READ_ADC:
            {
                int chip    = (0xFF & data) << 8;
                int channel = (0xFF & data);
                client.print(readValue(chip, channel));
                break;
            }
        case CMD_CHECK_DOOR:
            {
                client.print(checkDoor());
                break;
            }
        case CMD_POWER_ON:
            {
                int channel = data;
                powerOn (channel);
                break;
            }
        case CMD_POWER_OFF:
            {
                int channel = data;
                powerOff (channel);
                break;
            }
        case CMD_RESET:
            {
                resetAllADCs();
                break;
            }
        default:
            {
                break;
            }
    }
}

void closeEthernetClient()
{
    /* give the web browser time to receive the data */
    delay(1);
    /* close the connection: */
    client.stop();

    debug_println("client disconnected");
}

void startEthernetServer()
{
    /*start the web server */
    server.begin();

    debug_println("server is at %i", Ethernet.localIP());

    if (isPrimary)
        debug_println("primary box selected");
    else
        debug_println("secondary box selected");
}

void configurePowerSupplyLines ()
{
    /*if this is the primary box, set the enable lines and the chip select lines properly */
    for(int i =0; i<18;i++){
        if (isPrimary)
            psline[i] = pslinePri[i];
        else
            psline[i] = pslineSec[i];
    }
}

void startSPI ()
{
    /* start the SPI library: */
    SPI.begin();
    SPI.setClockDivider( SPI_CLOCK_DIV2 ); /* slow the SPI bus down */
    SPI.setBitOrder(MSBFIRST);  /*make sure bit order is right */
    // SPI.setDataMode(SPI_MODE0);
}

void configurePins()
{
    for(int i =0; i<4;i++){
        if (isPrimary)
            chipSelectPin[i] = chipSelectPinPri[i];
        else
            chipSelectPin[i] = chipSelectPinSec[i];
    }

    /*set all power supply enable pins to output and to HIGH (power supply inhibit) */
    for(int i=0;i<18;i++){
        if(psline[i]>0){
            pinMode(psline[i],OUTPUT);
            digitalWrite(psline[i],HIGH);
        }
    }
    /* initalize the chip select pins: */
    pinMode(chipSelectPin[0], OUTPUT);
    pinMode(chipSelectPin[1], OUTPUT);
    pinMode(chipSelectPin[2], OUTPUT);
    pinMode(chipSelectPin[3], OUTPUT);

    /*initalize the box door sensor and enable the pull-up resistor */
    pinMode(doorStatePin, INPUT_PULLUP);
}

void configureAveraging()
{
    /*set up ADC averaging */
    writeRegister(0,averageB);
    writeRegister(1,averageB);
    writeRegister(2,averageB);
    writeRegister(3,averageB);
}

void setMacAddress()
{
    if (Ethernet.begin(mac) == 0) {
        debug_println("Failed to configure Ethernet using DHCP");
        /* initialize the ethernet device not using DHCP: */
        Ethernet.begin(mac, ip);
    }
}

void configureADC()
{
    /*write the setup byte */
    writeRegister(0,setupB);
    writeRegister(1,setupB);
    writeRegister(2,setupB);
    writeRegister(3,setupB);
    /* give the sensors time to set up: */
    delay(100);
}

void startEthernet ()
{
    /* start the Ethernet connection and the server: */
    /*remove if using DHCP below */
    //if (isPrimary)
    //    Ethernet.begin(macPri, ip);
    //else
    //    Ethernet.begin(macSec, ip);
    /* start the Ethernet connection: **************Edit for primary/secondary************ */
    for (int j=0;j<10;j++){
        if (Ethernet.begin(mac)) {
            break;
        }
        debug_println("Waiting for DHCP assigned address\n");
        delay(5000);
    }
}

void checkDoor()
{
    /* Debug */
    debug_println("box door check command BSread seen from curl");

    return (digitalRead(doorStatePin));
}

void resetAllADCs ()
{
    /*write the setup byte */
    for (int i=0; i<4; i++) {
        writeRegister(i,setupB);
        delay(100);
        writeRegister(0,resetB);
    }

    /* Debug */
    debug_println("ADC reset command resetA seen from curl");
}

void powerOn (int channel)
{
    /* Debug */
    debug_println("power on command received from curl:");

    /* set power supply line LOW (enable) */
    digitalWrite(psline[channel], LOW);
}

void powerOff (int channel)
{
    /* Debug */
    debug_println("power off command received from curl: ");

    /* set power supply line High (disable) */
    digitalWrite(psline[channel], HIGH);
}

uint16_t readValue(int chip, int channel)
{
    /* take the chip select low to select the device: */
    digitalWrite(chipSelectPin[chip], LOW);

    /*add convert one channel opetation to the adc number to produce scan command */
    uint8_t b0 = SPI.transfer(scanOneChan+adcAddress[adcNum]);
    uint8_t b1 = SPI.transfer(0x00);
    uint8_t b2 = SPI.transfer(0x00);
    /*debug_println("chip ="); */
    /*debug_println(chip, DEC); */
    /*debug_println("    "); */
    /*debug_println(b0,BIN); */
    /*debug_println("    "); */
    /*debug_println(b1,BIN); */
    /*debug_println("    "); */
    /*debug_println(b2,BIN); */

    uint16_t result |= b1 << 8;
             result |= b2;

    /* take the chip select high to de-select: */
    digitalWrite(chipSelectPin[chip], HIGH);

    resetADCFIFO(chip);

    return(result);
}

void resetADCFIFO (int chip)
{
    writeRegister(chip,resetB);/*reset the FIFO only of the ADC */
}

void writeRegister(int chipNum, uint8_t thisRegister)
{
    /* take the chip select low to select the device: */
    digitalWrite(chipSelectPin[chipNum], LOW);

    SPI.transfer(thisRegister); /*Send register location */
    //SPI.transfer(thisValue);  //Send value to record into register */

    /* take the chip select high to de-select: */
    digitalWrite(chipSelectPin[chipNum], HIGH);
}

String readURL()
{
    String readString;
    while (client.connected() && client.available()) {
        char c = client.read();
        /* read char by char HTTP request */
        if (readString.length() < 100) {
            readString += c; /* store characters to string */
        }

        /* if you've gotten to the end of the line (received a newline
         * character) and the line is blank, the http request has ended,
         * so you can send a reply */
        if (c == '\n') {
            debug_println(readString);
            return readString;
        }
    }
}

void parseUrlAndExecuteCommand (String url)
{
    int start=0;
    if (url.indexOf('?') > 0) {
        start = url.indexOf('?');
        debug_println("starting position of %i", start);
    }

    int length = url.length();

    uint8_t cmd = 0xF;
    uint16_t data = 0x16;

    if (length>=start+1)
        cmd   = strtol (url.substring(start+1,start+1).c_str(),  NULL, 16);
    if (length>=start+3)
        data  = strtol (url.substring(start+2, start+3).c_str(), NULL, 16);

    executeCommand (cmd, data);
}


void openEthernetClient()
{
    /* listen for incoming clients */
    client = server.available();
    if (!client) {
        debug_println("error: ...");
        return;
    }
    debug_println("new client");
}

void configureSerial()
{
#ifdef DEBUG
        Serial.begin(9600); /* Open serial communications and wait for port to open: */
#endif
        debug_println("Trying to get an IP address using DHCP");
}
