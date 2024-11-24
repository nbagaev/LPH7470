/*display LPH7470-1 controller probably PCF8558 97 columns, 4 rows 8 pix + 1 row 3 pix
pinout:
1 - LED GND
2 - LED +5V 20 mA
3 - -6.5V VLCD
4 - +3V VDD
5 - GND
6 - SCL
7 - SDA
              ______
          |--|__91__|--|<|--|<|--|
LED GND --|   ______             |----|
          |--|__91__|--|<|--|<|--|    |
LED +5V ------------------------------|
*/

#include <Wire.h>
#include "font.h"

#define ADDRESS 0x3D
#define COLS 97
#define ROWS 5
#define CHAR_WIDTH 5
#define SPACE 1

#define DISPLAY_BLANK 0x00
#define DISPLAY_NORMAL  0x60
#define DISPLAY_ALL_SEGMENTS_ON 0x40
#define DISPLAY_INVERSE 0x20
#define POWER_ON 0x0
#define POWER_DOWN 0x10
#define HOR_ADDRESSING 0x0
#define VERT_ADDRESSING 0x8

void lcd_all_pix_on(uint8_t on)
{
	Wire.beginTransmission(ADDRESS);
  if(on)
    Wire.write(DISPLAY_ALL_SEGMENTS_ON);
  else
     Wire.write(DISPLAY_NORMAL);
  Wire.write(0x00);
  Wire.endTransmission();
}

void lcd_clear()
{
  for (uint8_t r = 0; r < ROWS; r++)
  {
    for (uint8_t b = 0; b < 4; b++) // 97 columns, 32 byte wire buffer, 4 * 25 = 100 columns
    {
	    Wire.beginTransmission(ADDRESS);
      Wire.write(DISPLAY_NORMAL | r); // Display status = normal; power-down mode = normal; addressing = horizontal; line = r
      Wire.write(0x19*b);// x Address = 25 * b
      for (uint16_t c = 0; c < 25; c++)
      {
	      Wire.write(0x00);//write blank pixels
      }
      Wire.endTransmission();
    }
  }
}

void lcd_draw_char(char ch, uint8_t line, uint8_t column)
{	
  Wire.beginTransmission(ADDRESS);
  Wire.write(DISPLAY_NORMAL | line );
  Wire.write(column);
	uint16_t i = (ch - 32) * CHAR_WIDTH;//first 32 ascii not in use
	uint16_t j = i + CHAR_WIDTH;
	while ( i < j )
	{   	
    Wire.write(pgm_read_byte(&font7x5[i]));
    i++;
	}
  Wire.endTransmission();
}

void lcd_draw_string(char *str, uint8_t line, uint8_t column)
{  
  uint8_t i = 0;
  while (str[i] != '\0' && (column + i * (CHAR_WIDTH + SPACE) + CHAR_WIDTH) < COLS)
  {    
    lcd_draw_char(str[i], line, column + i * (CHAR_WIDTH + SPACE));
    i++; 
  }
}

void setup()
{
  Wire.setClock(100000);
  lcd_all_pix_on(1);
  delay(2000);
  lcd_all_pix_on(0);
  delay(2000);
}

void loop()
{
  for(uint8_t i = 0; i < 4; i++)
  {
    lcd_draw_string((char *)"Hello World!", i, 14);
    delay(1000);
    lcd_clear();
    delay(1000);
  }
}
