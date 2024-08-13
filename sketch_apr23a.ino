#include <Wire.h>

#define ENDERECO 0x3E
#define RS 0x40
#define CO 0x80

#define DISPLAY_CLEAR 0x01
#define CURSOR_HOME 0x02
#define INPUT_SET_MAIS 0x07
#define INPUT_SET_MENOS 0x05 
#define BLINK_OFF 0x08
#define BLINK_ON 0x0F
#define FUNCTION_SET 0x20
#define DISPLAY_2LINHAS 0x08
#define DISPLAY_SHIFT_LEFT 0x18
#define DISPLAY_SHIFT_RIGHT 0x1C
#define DISPLAY_ON 0x04
#define CURSOR_ON 0x02
#define DISPLAY_ON_OFF 0x08

void displayShiftLeft()
{
  escreverComando8(DISPLAY_SHIFT_LEFT);
  delayMicroseconds(39);
}

void displayShiftRight()
{
  escreverComando8(DISPLAY_SHIFT_RIGHT);
  delayMicroseconds(39);
}

void escreverComando8(byte oitoBits)
{
  Wire.beginTransmission(ENDERECO);
  Wire.write(0x00);
  Wire.write(oitoBits);
  Wire.endTransmission();
  delayMicroseconds(39);
}

void escreverDados8(byte oitoBits)
{
  Wire.beginTransmission(ENDERECO);
  Wire.write(RS);
  Wire.write(oitoBits);
  Wire.endTransmission();
  delayMicroseconds(45);
}

void displaySetCursor(byte linha, byte coluna)
{
  linha = linha - 1;
  coluna = coluna - 1;
  if (linha == 0)
    escreverComando8(CO | 0x00 | coluna);
  else if (linha == 1)
    escreverComando8(CO | 0x40 | coluna);
}

void displayClear()
{
  escreverComando8(DISPLAY_CLEAR);
  delay(2);
}

void displayCursorOffBlinkOff()
{
  escreverComando8(BLINK_OFF);
  delayMicroseconds(45);
}

void displayCursorOnBlinkOn()
{
  escreverComando8(BLINK_ON);
  delayMicroseconds(45);
}

void displayOn()
{
  escreverComando8(DISPLAY_ON_OFF | DISPLAY_ON | CURSOR_ON | BLINK_ON);
  delayMicroseconds(45);
}

void displayCursorHome()
{
  escreverComando8(CURSOR_HOME);
  delayMicroseconds(45);
}

void functionSet()
{
  escreverComando8(FUNCTION_SET | DISPLAY_2LINHAS);
  delayMicroseconds(39);
}

void displayInit()
{
  delay(50);
  functionSet();
  displayOn();
  escreverComando8(BLINK_ON);
  displayClear();
  escreverComando8(0x06);
}

void displayPrintChar(char c)
{
  escreverDados8(c);
}

void displayPrintString(const char* s)
{
  int i = 0;
  while (s[i] != '\0')
  {
    displayPrintChar(s[i]);
    i++;
  }
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  displayInit();
  displaySetCursor(1, 1);
  displayPrintString("ola bom dia");
}

// comandos:
// write(string)      -- escreve a string na posicao atual do cursor
// cursor(x, y)       -- move o cursor
// clear              -- limpa o display
// shift(direção, numero_de_casas)  -- causa o display dar shift na direção requesitada
// metro              -- demonstra uma utilização do display com shift automático

void loop()
{
  if (Serial.available() > 0)
  {
    String input = Serial.readStringUntil('\n');

    if (input.startsWith("clear"))
      displayClear();
    else if (input.startsWith("cursor"))
    {
      int x = input.substring(input.indexOf("(") + 1, input.indexOf(",")).toInt();
      int y = input.substring(input.indexOf(",") + 1, input.indexOf(")")).toInt();
      displaySetCursor(x, y);
    }
    else if (input.startsWith("write"))
    {
      String toWrite = input.substring(input.indexOf("(") + 1, input.indexOf(")"));
      displayPrintString(toWrite.c_str());
    }
    else if (input.startsWith("shift"))
    {
      int i = 0;
      String direction = input.substring(input.indexOf("(") + 1, input.indexOf(","));
      int count = input.substring(input.indexOf(",") + 1, input.indexOf(")")).toInt();
      if (strcmp(direction.c_str(), "L") == 0 || strcmp(direction.c_str(), "l") == 0)
      {
        while (i < count)
        {
          displayShiftLeft();
          i++;
        }
      }
      else if (strcmp(direction.c_str(), "R") == 0 || strcmp(direction.c_str(), "r") == 0)
      {
        while (i < count)
        {
          displayShiftRight();
          delay(1000);
          i++;
        }
      }
    }
    else if (input.startsWith("metro"))
    {
      input = Serial.readStringUntil('\n');
      displayClear();
      displaySetCursor(1, 1);
      displayPrintString("Perturbacoes  na  linha  amarela");
      displaySetCursor(2, 1);
      displayPrintString("Tempo  de  espera  acima  do  normal");
      escreverComando8(0x0C);
      while (1)
      {
        displayShiftLeft();
        delay(800);
        input = Serial.readStringUntil('\n');
        if (input.startsWith("stop"))
          break;
      }
      escreverComando8(0x0F);
      displayClear();
      displaySetCursor(1, 1);

      displayPrintString("ola bom dia");
    }
  }
}
