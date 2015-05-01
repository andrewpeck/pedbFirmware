#include "PEDBfirmwareV5.hpp"

#define debug_println(text) \
        do { if (DEBUG) Serial.println(text); } while (0)
#define debug_print(text) \
        do { if (DEBUG) Serial.print(text); } while (0)

void executeCommand (int cmd, int channel, int chip)
{
    switch (cmd):
    {
        case CMD_READ_VOLTAGE:
            readVoltage(chip, channel);
            break;
        case CMD_POWER_ON:
            break;
        case CMD_POWER_OFF:
            powerOff (channel);
            break;
        case CMD_READ_CURRENT:
            readCurrent(.....);
            break;
        case CMD_CHECK_DOOR:
            checkDoor();
            break;
        case CMD_RESET:
            resetADC();
            break;
        default:
            break;
    }
}

void closeClient()
{
    /* give the web browser time to receive the data */
    delay(1);
    /* close the connection: */
    client.stop();

    debug_println("client disconnected");
}

void startServer()
{
    /*start the web server */
    server.begin();

    debug_print("server is at ");
    debug_println(Ethernet.localIP());

    if (isPrimary)
        debug_print("primary box selected");
    else
        debug_print("secondary box selected");
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
    if (isPrimary)
        Ethernet.begin(macPri, ip);
    else
        Ethernet.begin(macSec, ip);
    /* start the Ethernet connection: **************Edit for primary/secondary************ */
    for (int j=0;j<10;j++){
        if (Ethernet.begin(mac)) {
            break;
        }
        debug_println("Waiting for DHCP assigned address");
        delay(5000);
    }
}

void readVoltage(int chip, int channel)
{
    int adc_value = readValue(chip, channel);
    client.print(...);
}

void readCurrent(......)
{
    debug_print("current read command seen from curl");
    debug_print('\n');
}

void checkDoor()
{
    debug_print("box door check command BSread seen from curl");
    debug_print('\n');
    client.print("\n");  /*there is no logical reason for this to be here, but this will not work without it */
    if(digitalRead(doorStatePin)){
        client.print("1");
    }
    else{
        client.print("0");
    }
    client.print("\n");
}

void resetADC ()
{
    client.print("\n");  /*there is no logical reason for this to be here, but this will not work without it */
    /*write the setup byte */
    writeRegister(0,setupB);
    writeRegister(1,setupB);
    writeRegister(2,setupB);
    writeRegister(3,setupB);
    /* give the sensors time to set up: */
    delay(100);
    writeRegister(0,resetB);/*reset the FIFO only of the ADC */
    writeRegister(1,resetB);
    writeRegister(2,resetB);
    writeRegister(3,resetB);
    debug_print("ADC reset command resetA seen from curl");
    debug_print('\n');
}

void powerOn (int channel)
{
    debug_print('power on command received from curl: ');
    debug_print(cmdCandidate);
    debug_print('\n');
    digitalWrite(psline[i], LOW);    /* set power supply line LOW (enable) */
}

void powerOff (int channel)
{
    debug_print('power off command received from curl: ');
    debug_print(cmdCandidate);
    debug_print('\n');

    digitalWrite(psline[i], HIGH);    /* set power supply line HIGH (disable) */
}

int readValue(int chipNum,int adcNum)
{
    /*value to return */
    int result=0;
    uint8_t b0=0;
    uint8_t b1=0;
    uint8_t b2=0;


    /* take the chip select low to select the device: */
    digitalWrite(chipSelectPin[chipNum], LOW);

    /*add convert one channel opetation to the adc number to produce scan command */
    b0 = SPI.transfer(scanOneChan+adcAddress[adcNum]);
    b1 = SPI.transfer(0x00);
    b2 = SPI.transfer(0x00);
    /*debug_print("chipNum ="); */
    /*debug_print(chipNum, DEC); */
    /*debug_print("    "); */
    /*debug_print(b0,BIN); */
    /*debug_print("    "); */
    /*debug_print(b1,BIN); */
    /*debug_print("    "); */
    /*debug_print(b2,BIN); */

    result |= b1 << 8;/*shift b1 left one byte */
    result |= b2;
    /*debug_print("NEXT\n"); */
    /* take the chip select high to de-select: */
    digitalWrite(chipSelectPin[chipNum], HIGH);

    writeRegister(chipNum,resetB);/*reset the FIFO only of the ADC */

    return(result);
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

string readURL()
{
    string readString;
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

void parseUrlAndExecuteCommand (string url)
{
    if (url.indexOf('?') > 0) {
        start = url.indexOf('?');
        debug_print("starting position of ? ");
        debug_println(start);
        string cmd = url.substring(start+1,start+3);
    }

    uint8_t cmd     = strtol (cmd.c_str()[0], NULL, 16);
    uint8_t channel = strtol (cmd.c_str()[1], NULL, 16);
    uint8_t chip    = strtol (cmd.c_str()[2], NULL, 16);

    executeCommand (cmd, channel, chip);
}


void openClient()
{
    /* listen for incoming clients */
    EthernetClient client = server.available();
    if (!client) {
        debug_print("error: ...");
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
