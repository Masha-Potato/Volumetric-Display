#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdio.h>
#include "ili9341.h"
#include <avr/io.h>

byte clr;

/** @array Init command */
const uint8_t INIT_ILI9341[] PROGMEM = {
  // number of initializers
  12,

  // --------------------------------------------  
  0,  50, ILI9341_SWRESET,                                      // 0x01 -> Software reset
  0,   0, ILI9341_DISPOFF,                                      // 0x28 -> Display OFF
/*
  // --------------------------------------------
  3,   0, 0xEF, 0x03, 0x80, 0x02,                               // 0xEF
  3,   0, ILI9341_LCD_POWERB, 0x00, 0xC1, 0x30,                 // 0xCF -> Power control B
  4,   0, ILI9341_LCD_POWER_SEQ, 0x64, 0x03, 0x12, 0x81,        // 0xED -> Power on sequence
  3,   0, ILI9341_LCD_DTCA, 0x85, 0x00, 0x78,                   // 0xE8 -> Driver timing control A
  5,   0, ILI9341_LCD_POWERA, 0x39, 0x2C, 0x00, 0x34, 0x02,     // 0xCB -> Power control A
  1,   0, ILI9341_LCD_PRC, 0x20,                                // 0xF7 -> Pump ratio control
  2,   0, ILI9341_LCD_DTCB, 0x00, 0x00,                         // 0xEA -> Driver timing control B
*/
  // --------------------------------------------  
  1,   0, ILI9341_PWCTRL1, 0x23,                                // 0xC0 -> Power Control 1
  1,   0, ILI9341_PWCTRL2, 0x10,                                // 0xC1 -> Power Control 2
  2,   0, ILI9341_VCCR1, 0x2B, 0x2B,                            // 0xC5 -> VCOM Control 1
  1,   0, ILI9341_VCCR2, 0xC0,                                  // 0xC7 -> VCOM Control 2

  // -------------------------------------------- 
  1,   0, ILI9341_MADCTL, 0x48,                                 // 0x36 -> Memory Access Control
  1,   0, ILI9341_COLMOD, 0x55,                                 // 0x3A -> Pixel Format Set
  2,   0, ILI9341_FRMCRN1, 0x00, 0x1B,                          // 0xB1 -> Frame Rate Control
/*
  3,   0, ILI9341_DISCTRL, 0x08, 0x82, 0x27,                    // 0xB6 -> Display Function Control
  1,   0, 0xF2, 0x00,                                           // 0xF2 -> gamma function disable
  2,   0, ILI9341_GAMSET, 0x00, 0x1B,                           // 0x26 -> Gamma Set
*/
  1,   0, ILI9341_ETMOD, 0x07,                                  // 0xB7 -> Entry Mode Set
/*  
  // Set Gamma - positive
  15,  0, ILI9341_GMCTRP1 , 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 
    0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  // Set Gamma - negative
  15,  0, ILI9341_GMCTRN1 , 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 
    0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
*/
  // --------------------------------------------
  0, 150, ILI9341_SLPOUT,                                       // 0x11 -> Sleep Out
  0, 200, ILI9341_DISPON                                        // 0x29 -> Display on
};

/**
 * @desc    LCD init
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_Init (void)
{
  // variables
  const uint8_t *commands = INIT_ILI9341;
  // number of commands
  unsigned short int no_of_commands = pgm_read_byte(commands++);
  // arguments
  char no_of_arguments;
  // command
  char command;
  // delay
  unsigned short int delay;

  // Init ports
  ILI9341_InitPortsnSPI();

  // Init hardware reset
  ILI9341_HWReset();

  // loop throuh commands
  while (no_of_commands--) {
    // number of arguments
    no_of_arguments = pgm_read_byte(commands++);
    // delay
    delay = pgm_read_byte(commands++);
    // command
    command = pgm_read_byte(commands++);
    // send command
    // -------------------------    
    ILI9341_TransmitCmmd(command);
    // send arguments
    // -------------------------
    while (no_of_arguments--) {
      // send arguments
      ILI9341_Transmit8bitData(pgm_read_byte(commands++));
    }
    // delay
    ILI9341_Delay(delay);
  }
  // set window -> after this function display show RAM content
  ILI9341_SetWindow(0, 0, ILI9341_MAX_X-1, ILI9341_MAX_Y-1);
}

/**
 * @desc    LCD init PORTs
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_InitPortsnSPI (void)
{
  // set control pins as output
  SETBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_RST);
  SETBIT(ILI9341_DDR_CONTROL, 2); //even though not using ss, have to set it so avr does spi properly
  //SETBIT(ILI9341_PORT_CONTROL, 2);
  SETBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_DC);
  SETBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_MOSI);
  SETBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_CK);

  //set MISO as input
  CLRBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_MISO);

  SPCR = (1 << SPE) | (1 << MSTR);

  SPSR = (1 << SPI2X);

  //clear out previous junk
  clr = SPSR;

  clr = SPDR;
}

/**
 * @desc    LCD Hardware Reset
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_HWReset (void)
{
  // set RESET as Output
  SETBIT(ILI9341_DDR_CONTROL, ILI9341_PIN_RST);

  // RESET SEQUENCE
  // --------------------------------------------
  // set Reset LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RST);
  // delay LOW > 10us
  _delay_ms(1);
  // set Reset HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RST);

  // delay HIGH > 120ms
  _delay_ms(200);  
}

/**
 * @desc    LCD Transmit Command
 *
 * @param   uint8_t
 *
 * @return  void
 */
void ILI9341_TransmitCmmd (uint8_t cmmd)
{
  // D/C goes LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RS);
  // chip select goes LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);

  //SPI peripheral do your thing
  SPDR = value;
    
  while (!(SPSR & _BV(SPIF)));

  // D/C goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RS);
  // chip select goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS); 
}

/**
 * @desc    LCD transmit 8 bits data
 *
 * @param   uint8_t
 *
 * @return  void
 */
void ILI9341_Transmit8bitData (uint8_t data)
{
  // D/C goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RS);
  // chip select goes LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);
 
  //SPI peripheral do your thing
  SPDR = value;
    
  while (!(SPSR & _BV(SPIF)));

  // chip select goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);
}

/**
 * @desc    LCD transmit 16 bits data
 *
 * @param   uint16_t
 *
 * @return  void
 */
void ILI9341_Transmit16bitData (uint16_t data)
{
  // D/C goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RS);
  // chip select goes LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);

  //SPI peripheral do your thing
  // High byte
  SPDR = (uint8_t)(value >> 8);
  // Low byte (queued)   
  SPDR = (uint8_t)(value);

  while (!(SPSR & _BV(SPIF)));

  // chip select goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);
}

/**
 * @desc    LCD transmit 32 bits data
 *
 * @param   uint16_t
 *
 * @return  void
 */
void ILI9341_Transmit32bitData (uint32_t data)
{
  // D/C goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_RS);
  // chip select goes LOW
  CLRBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);

  SPDR = (uint8_t)(value >> 24);  // Byte 3
  SPDR = (uint8_t)(value >> 16);  // Byte 2
  while (!(SPSR & _BV(SPIF)));

  SPDR = (uint8_t)(value >> 8);   // Byte 1
  SPDR = (uint8_t)(value);        // Byte 0
  while (!(SPSR & _BV(SPIF)));

  // chip select goes HIGH
  SETBIT(ILI9341_PORT_CONTROL, ILI9341_PIN_CS);
}

/**
 * @desc    LCD Set address window
 *
 * @param   uint16_t
 * @param   uint16_t
 * @param   uint16_t
 * @param   uint16_t
 *
 * @return  char
 */
char ILI9341_SetWindow (uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
  // check if coordinates is out of range
  if ((xs > xe) || (xe > ILI9341_SIZE_X) ||
      (ys > ye) || (ye > ILI9341_SIZE_Y)) 
  { 
    // out of range
    return ILI9341_ERROR;
  }  

  // set column
  ILI9341_TransmitCmmd(ILI9341_CASET);
  // set column -> set column
  ILI9341_Transmit32bitData(((uint32_t) xs << 16) | xe);
  // set page
  ILI9341_TransmitCmmd(ILI9341_PASET);
  // set page -> high byte first
  ILI9341_Transmit32bitData(((uint32_t) ys << 16) | ye);
  // success
  return ILI9341_SUCCESS;
}

/**
 * @desc    LCD Draw Pixel
 *
 * @param   uint16_t
 * @param   uint16_t
 * @param   uint16_t
 *
 * @return  char
 */
char ILI9341_DrawPixel (uint16_t x, uint16_t y, uint16_t color)
{
  // check dimension
  if ((x > ILI9341_SIZE_X) || (y > ILI9341_SIZE_Y)) {
    // error
    return ILI9341_ERROR;
  }
  // set window
  ILI9341_SetWindow(x, y, x, y);
  // draw pixel by 565 mode
  ILI9341_SendColor565(color, 1);
  // success
  return ILI9341_SUCCESS;
}

/**
 * @desc    LCD Write Color Pixels
 *
 * @param   uint16_t
 * @param   uint32_t
 *
 * @return  void
 */
void ILI9341_SendColor565 (uint16_t color, uint32_t count)
{
  // access to RAM
  ILI9341_TransmitCmmd(ILI9341_RAMWR);
  // counter
  while (count--) {
    // write color - first colors byte
    ILI9341_Transmit16bitData(color);
  }
}

/**
 * @desc    Clear screen
 *
 * @param   uint16_t color
 *
 * @return  void
 */
void ILI9341_ClearScreen (uint16_t color)
{
  // set whole window
  ILI9341_SetWindow(0, 0, ILI9341_SIZE_X, ILI9341_SIZE_Y);
  // draw individual pixels
  ILI9341_SendColor565(color, ILI9341_CACHE_MEM);
}

/**
 * @desc    LCD Inverse Screen
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_InverseScreen (void)
{
  // display on
  ILI9341_TransmitCmmd(ILI9341_DINVON);
}

/**
 * @desc    LCD Normal Screen
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_NormalScreen (void)
{
  // display on
  ILI9341_TransmitCmmd(ILI9341_DINVOFF);
}

/**
 * @desc    LCD Update Screen
 *
 * @param   void
 *
 * @return  void
 */
void ILI9341_UpdateScreen (void)
{
  // display on
  ILI9341_TransmitCmmd(ILI9341_DISPON);
}

/**
 * @desc    LCD Fast draw line horizontal - depend on MADCTL
 *
 * @param   uint16_t - xs start position
 * @param   uint16_t - xe end position
 * @param   uint16_t - y position
 * @param   uint16_t - color
 *
 * @return  char
 */
char ILI9341_DrawLineHorizontal (uint16_t xs, uint16_t xe, uint16_t y, uint16_t color)
{
  // temp variable
  uint16_t temp;
  // check if out of range
  if ((xs > ILI9341_SIZE_X) || (xe > ILI9341_SIZE_X) || (y > ILI9341_SIZE_Y)) {
    // error
    return ILI9341_ERROR;
  }
  // check if start is > as end  
  if (xs > xe) {
    // temporary safe
    temp = xe;
    // start change for end
    xe = xs;
    // end change for start
    xs = temp;
  }
  // set window
  ILI9341_SetWindow(xs, y, xe, y);
  // draw pixel by 565 mode
  ILI9341_SendColor565(color, xe - xs);
  // success
  return ILI9341_SUCCESS;
}

/**
 * @desc    LCD Fast draw line vertical - depend on MADCTL
 *
 * @param   uint16_t - x position
 * @param   uint16_t - ys start position
 * @param   uint16_t - ye end position
 * @param   uint16_t - color
 *
 * @return  char
 */
char ILI9341_DrawLineVertical (uint16_t x, uint16_t ys, uint16_t ye, uint16_t color)
{
  // temp variable
  uint16_t temp;
  // check if out of range
  if ((ys > ILI9341_SIZE_Y) || (ye > ILI9341_SIZE_Y) || (x > ILI9341_SIZE_X)) {
    // error
    return ILI9341_ERROR;
  }  
  // check if start is > as end
  if (ys > ye) {
    // temporary safe
    temp = ye;
    // start change for end
    ye = ys;
    // end change for start
    ys = temp;
  }
  // set window
  ILI9341_SetWindow(x, ys, x, ye);
  // draw pixel by 565 mode
  ILI9341_SendColor565(color, ye - ys);
  // success
  return ILI9341_SUCCESS;
}

/**
 * @desc    Delay
 *
 * @param   uint16_t
 *
 * @return  void
 */
void ILI9341_Delay (uint16_t time)
{
  // loop through real time
  while (time--) {
    // 1 s delay
    _delay_ms(1);
  }
}
