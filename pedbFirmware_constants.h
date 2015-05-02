#ifndef PEDB_FIRMWARE_CONSTANTS_H
#define PEDB_FIRMWARE_CONSTANTS_H

//*************************************************************************************************************
#define ISPRIMARY /*set this if the is being compiled for the primary box. comment out if secondary is needed */
//**************************************************************************************************************

#ifdef ISPRIMARY
    static const int isPrimary = 1;
#else
    static const int isPrimary = 0;
#endif

/*set up the VSET lines on the Arduino board (for PS control) */
static const int VSET1  = 48;
static const int VSET2  = 46;
static const int VSET3  = 44;
static const int VSET4  = 42;
static const int VSET5  = 40;
static const int VSET6  = 38;
static const int VSET7  = 36;
static const int VSET8  = 34;
static const int VSET9  = 32;
static const int VSET10 = 30;
static const int VSET11 = 28;
static const int VSET12 = 26;
static const int VSET13 = 24;
static const int VSET14 = 22;
static const int VSET15 = 23;
static const int VSET16 = 25;
static const int VSET17 = 27;
static const int VSET18 = 7;



const unsigned char doorStatePin = 8;  /*the digital line for reading the state of the enclosure door */
const unsigned char averageB     = B00111100; /*register to select averging (32 samples) */
const unsigned char scanOneChan  = B10000110; /*scan one channel (averaging must be set) */

/*setup uint8_t */
/*internal conversion clock (needed for averaging) */
/*external and acquisition clock */
/*external reference */
const unsigned char setupB = B01110110;

/* primary box enable lines */
int pslinePri[18] = {VSET17,VSET1,VSET2,VSET3,VSET4,VSET5,VSET6,VSET7,VSET8,VSET9,VSET10,VSET11,VSET12,VSET13,VSET14,VSET15,VSET16,VSET18};
int chipSelectPinPri[4] = {29,31,33,35}; /*primary box chip select pins */

/* secondary box */
int pslineSec[18] = {VSET9,VSET7,VSET8,VSET5,VSET6,VSET3,VSET4,VSET1,VSET2,-1,-1,-1,-1,-1,-1,-1,-1,VSET10};
int chipSelectPinSec[4] = {33,35,29,31};/*secondary box, first two are ADCs on J3 board, next are on J5 board */

/* addresses for the ADC channels */
const uint8_t adcAddress[16] = {
    0x0, // B00000000,
    B00001000,
    B00010000,
    B00011000,
    B00100000,
    B00101000,
    B00110000,
    B00111000,
    B01000000,
    B01001000,
    B01010000,
    B01011000,
    B01100000,
    B01101000,
    B01110000,
    B01111000
};

/*reset uint8_t for the power distribution board ADCs */
static const uint8_t resetB = B00011000;

//int ps1state = 0;/*power supply 1 state */

/* two MAC address were randomly generated using the http://www.miniwebtool.com/mac-address-generator/ web site */
/*DF-08-DD-81-C9-86 (primary box) */
/*39-32-6C-9F-33-6F (secondary box) */

#ifdef ISPRIMARY
static uint8_t mac[6]={0xDF, 0x08, 0xDD, 0x81, 0xC9, 0x86};
#else
static uint8_t mac[6]={0x39, 0x32, 0x6C, 0x9F, 0x33, 0x6F};
#endif

#endif
