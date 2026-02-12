#include <avr/pgmspace.h>

#ifndef __ILI9341_H__
#define __ILI9341_H__

  // HARDWARE DEFINITION
  // 
  // Data
  // ---------------------------------------------------------------

  // Control 
  // ---------------------------------------------------------------
  // DDR JUST SET ONCE FOR OUTPUT
  #define ILI9341_DDR_CONTROL   DDRB
  // REPEATEDLY TOGGLE FOR CONTROL
  #define ILI9341_PORT_CONTROL  PORTB
  #define ILI9341_PIN_RST	 0
  #define ILI9341_PIN_DC   1
  #define ILI9341_PIN_CK   5
  #define ILI9341_PIN_MOSI 3
  #define ILI9341_PIN_MISO 4

  // HEX COMMANDS
  // ---------------------------------------------------------------
  #define ILI9341_NOP           0x00  // No Operation
  #define ILI9341_SWRESET       0x01  // Software Reset
  #define ILI9341_RDDST         0x09  // Read Display Status
  // ---------------------------------------------------------------
  #define ILI9341_SLPIN         0x10  // Enter Sleep Mode
  #define ILI9341_SLPOUT        0x11  // Sleep Out
  // ---------------------------------------------------------------
  #define ILI9341_DINVOFF       0x20  // Display Inversion Off
  #define ILI9341_DINVON        0x21  // Display Inversion On
  #define ILI9341_GAMSET        0x26  // Gamma Set  
  #define ILI9341_DISPOFF       0x28  // Display OFF
  #define ILI9341_DISPON        0x29  // Display ON
  #define ILI9341_CASET         0x2A  // Column Address Set
  #define ILI9341_PASET         0x2B  // Page Address Set
  #define ILI9341_RAMWR         0x2C  // Memory Write
  // ---------------------------------------------------------------
  #define ILI9341_MADCTL        0x36  // Memory Access Control
  #define ILI9341_COLMOD        0x3A  // Pixel Format Set
  // ---------------------------------------------------------------
  #define ILI9341_FRMCRN1       0xB1  // Frame Control (In Normal Mode)
  #define ILI9341_DISCTRL       0xB6  // Display Function Control
  #define ILI9341_ETMOD         0xB7  // Entry Mode Set
  // ---------------------------------------------------------------
  #define ILI9341_PWCTRL1       0xC0  // Power Control 1
  #define ILI9341_PWCTRL2       0xC1  // Power Control 2
  #define ILI9341_VCCR1         0xC5  // VCOM Control 1
  #define ILI9341_VCCR2         0xC7  // VCOM Control 2
   // ---------------------------------------------------------------
  #define ILI9341_RDID1         0xDA  // Read ID1
  #define ILI9341_RDID2         0xDB  // Read ID2
  #define ILI9341_RDID3         0xDC  // Read ID3
  // ---------------------------------------------------------------
  #define ILI9341_GMCTRP1       0xE0  // Positive Gamma Correction
  #define ILI9341_GMCTRN1       0xE1  // Negative Gamma Correction

  #define ILI9341_LCD_POWERA    0xCB   // Power control A register
  #define ILI9341_LCD_POWERB    0xCF   // Power control B register
  #define ILI9341_LCD_DTCA      0xE8   // Driver timing control A
  #define ILI9341_LCD_DTCB      0xEA   // Driver timing control B
  #define ILI9341_LCD_POWER_SEQ 0xED   // Power on sequence register
  #define ILI9341_LCD_3GAMMA_EN 0xF2   // 3 Gamma enable register
  #define ILI9341_LCD_PRC       0xF7   // Pump ratio control register

  // set bit
  #define SETBIT(REG, BIT)      { REG |= (1 << BIT); }
  // clear bit
  #define CLRBIT(REG, BIT)      { REG &= ~(1 << BIT); }

  // for 16 MHz crystal T = 62.5ns =>
  // T pulse H -> Th = 31.25ns > twrh > 15ns
  // T pulse L -> Tl = 31.25ns > twrl > 15ns
  #define WR_IMPULSE()          { ILI9341_PORT_CONTROL &= ~(1 << ILI9341_PIN_WR); ILI9341_PORT_CONTROL |= (1 << ILI9341_PIN_WR); }

  // SOFTWARE DEFINITION
  // ---------------------------------------------------------------
  #define ILI9341_SUCCESS       0
  #define ILI9341_ERROR         1

  // Colors
  #define ILI9341_BLACK         0x0000
  #define ILI9341_WHITE         0xFFFF
  #define ILI9341_RED           0xF000

  // max columns
  #define ILI9341_MAX_X         240
  // max rows
  #define ILI9341_MAX_Y         320UL
  // columns max counter
  #define ILI9341_SIZE_X        ILI9341_MAX_X - 1
  // rows max counter
  #define ILI9341_SIZE_Y        ILI9341_MAX_Y - 1
  // whole pixels
  #define ILI9341_CACHE_MEM     (ILI9341_MAX_X * ILI9341_MAX_Y)

  /** @const Command list ILI9341B */
  extern const uint8_t INIT_ILI9341[];

  /**
   * @desc    LCD Init
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_Init (void);

  /**
   * @desc    LCD Hardware Reset
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_HWReset (void);

 /**
   * @desc    LCD Init PORTs
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_InitPorts (void);

  /**
   * @desc    LCD Transmit Command
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void ILI9341_TransmitCmmd (uint8_t);

  /**
   * @desc    LCD Transmit 8 bits data
   *
   * @param   uint8_t
   *
   * @return  void
   */
  void ILI9341_Transmit8bitData (uint8_t);

  /**
   * @desc    LCD Transmit 16 bits data
   *
   * @param   uint16_t
   *
   * @return  void
   */
  void ILI9341_Transmit16bitData (uint16_t);

  /**
   * @desc    LCD Transmit 32 bits data
   *
   * @param   uint32_t
   *
   * @return  void
   */
  void ILI9341_Transmit32bitData (uint32_t);

  /**
   * @desc    LCD Set window
   *
   * @param   uint16_t
   * @param   uint16_t
   * @param   uint16_t
   * @param   uint16_t 
   *
   * @return  char
   */
  char ILI9341_SetWindow (uint16_t, uint16_t, uint16_t, uint16_t);

  /**
   * @desc    LCD Write Color Pixels
   *
   * @param   uint16_t
   * @param   uint32_t
   *
   * @return  void
   */
  void ILI9341_SendColor565 (uint16_t, uint32_t);

  /**
   * @desc    LCD Draw Pixel
   *
   * @param   uint16_t
   * @param   uint16_t
   * @param   uint16_t
   *
   * @return  char
   */
  char ILI9341_DrawPixel (uint16_t, uint16_t, uint16_t);

  /**
   * @desc    LCD Clear screen
   *
   * @param   uint16_t
   *
   * @return  void
   */
  void ILI9341_ClearScreen (uint16_t);

  /**
   * @desc    LCD Inverse Screen
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_InverseScreen (void);

  /**
   * @desc    LCD Normal Screen
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_NormalScreen (void);

  /**
   * @desc    LCD Update Screen
   *
   * @param   void
   *
   * @return  void
   */
  void ILI9341_UpdateScreen (void);

  /**
   * @desc    LCD Fast draw line horizontal - depend on MADCTL
   *
   * @param   uint16_t - x start position
   * @param   uint16_t - x end position
   * @param   uint16_t - y position
   * @param   uint16_t - color
   *
   * @return  char
   */
  char ILI9341_DrawLineHorizontal (uint16_t, uint16_t, uint16_t, uint16_t);

  /**
   * @desc    LCD Fast draw line vertical - depend on MADCTL
   *
   * @param   uint16_t - x position
   * @param   uint16_t - y start position
   * @param   uint16_t - y end position
   * @param   uint16_t - color
   *
   * @return  char
   */
  char ILI9341_DrawLineVertical (uint16_t, uint16_t, uint16_t, uint16_t);

  /**
   * @desc    Delay
   *
   * @param   uint16_t
   *
   * @return  void
   */
  void ILI9341_Delay (uint16_t);

#endif
