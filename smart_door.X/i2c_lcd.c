#include <xc.h>
#include "i2c_lcd.h"
 
unsigned char RS, i2c_add, BackLight_State = LCD_BACKLIGHT;
 
//---------------[ I2C Routines ]-------------------
//--------------------------------------------------
void I2C_Master_Init(void)
{
  SSPCON = 0x28;
  SSPCON2 = 0x00;
  SSPSTAT = 0x00;
  SSPADD = ((_XTAL_FREQ/4)/I2C_BaudRate) - 1;
  SCL_D = 1;
  SDA_D = 1;
}
 
void I2C_Master_Wait(void)
{
  while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));
}
 
void I2C_Master_Start(void)
{
  I2C_Master_Wait();
  SEN = 1;
}
 
void I2C_Master_RepeatedStart(void)
{
  I2C_Master_Wait();
  RSEN = 1;
}
 
void I2C_Master_Stop(void)
{
  I2C_Master_Wait();
  PEN = 1;
}
 
void I2C_ACK(void)
{
  ACKDT = 0; // 0 -> ACK
  I2C_Master_Wait();
  ACKEN = 1; // Send ACK
}
 
void I2C_NACK(void)
{
  ACKDT = 1; // 1 -> NACK
  I2C_Master_Wait();
  ACKEN = 1; // Send NACK
}
 
unsigned char I2C_Master_Write(unsigned char data)
{
  I2C_Master_Wait();
  SSPBUF = data;
  while(!SSPIF); // Wait Until Completion
  SSPIF = 0;
  return ACKSTAT;
}
 
unsigned char I2C_Read_Byte(void)
{
  //---[ Receive & Return A Byte ]---
  I2C_Master_Wait();
  RCEN = 1; // Enable & Start Reception
  while(!SSPIF); // Wait Until Completion
  SSPIF = 0; // Clear The Interrupt Flag Bit
  I2C_Master_Wait();
  return SSPBUF; // Return The Received Byte
}
//======================================================
 
//---------------[ LCD Routines ]----------------
//-----------------------------------------------
 
void LCD_Init(unsigned char I2C_Add)
{
  i2c_add = I2C_Add;
  IO_Expander_Write(0x00);
  __delay_ms(30);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(0x03);
  __delay_ms(5);
  LCD_CMD(LCD_RETURN_HOME);
  __delay_ms(5);
  LCD_CMD(0x20 | (LCD_TYPE << 2));
  __delay_ms(50);
  LCD_CMD(LCD_TURN_ON);
  __delay_ms(50);
  LCD_CMD(LCD_CLEAR);
  __delay_ms(50);
  LCD_CMD(LCD_ENTRY_MODE_SET | LCD_RETURN_HOME);
  __delay_ms(50);
}
 
void IO_Expander_Write(unsigned char Data)
{
  I2C_Master_Start();
  I2C_Master_Write(i2c_add);
  I2C_Master_Write(Data | BackLight_State);
  I2C_Master_Stop();
}
 
void LCD_Write_4Bit(unsigned char Nibble)
{
  // Get The RS Value To LSB OF Data
  Nibble |= RS;
  IO_Expander_Write(Nibble | 0x04);
  IO_Expander_Write(Nibble & 0xFB);
  __delay_us(50);
}
 
void LCD_CMD(unsigned char CMD)
{
  RS = 0; // Command Register Select
  LCD_Write_4Bit(CMD & 0xF0);
  LCD_Write_4Bit((CMD << 4) & 0xF0);
}
 
void LCD_Write_Char(char Data)
{
  RS = 1; // Data Register Select
  LCD_Write_4Bit(Data & 0xF0);
  LCD_Write_4Bit((Data << 4) & 0xF0);
}
 
void LCD_Write_String(char* Str)
{
  for(int i=0; Str[i]!='\0'; i++)
    LCD_Write_Char(Str[i]);
}
 
void LCD_Set_Cursor(unsigned char ROW, unsigned char COL)
{
  switch(ROW)
  {
    case 2:
      LCD_CMD(LCD_SECOND_ROW + COL-1);
      break;
    case 3:
      LCD_CMD(LCD_THIRD_ROW + COL-1);
      break;
    case 4:
      LCD_CMD(LCD_FOURTH_ROW + COL-1);
      break;
    // Case 0
    default:
      LCD_CMD(LCD_FIRST_ROW + COL-1);
  }
}
 
void Backlight(void)
{
  BackLight_State = LCD_BACKLIGHT;
  IO_Expander_Write(0);
}
 
void noBacklight(void)
{
  BackLight_State = LCD_NOBACKLIGHT;
  IO_Expander_Write(0);
}
 
void LCD_SL(void)
{
  LCD_CMD(LCD_SHIFT_LEFT);
  __delay_us(40);
}
 
void LCD_SR(void)
{
  LCD_CMD(LCD_SHIFT_RIGHT);
  __delay_us(40);
}
 
void LCD_Clear(void)
{
  LCD_CMD(LCD_CLEAR);
  __delay_us(40);
  LCD_CMD(LCD_FIRST_ROW);
  __delay_us(40);
}