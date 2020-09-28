#include "Oregon_TM.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file is part of the Arduino OREGON_NR library.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The MIT License (MIT)
//
// Copyright (c) 2020 Sergey Zawislak 
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Этот файл - часть библиотеки OREGON_NR
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2020 Сергей Зависляк
//
// Данная лицензия разрешает лицам, получившим копию данного программного обеспечения и сопутствующей документации 
// (в дальнейшем именуемыми «Программное Обеспечение»), безвозмездно использовать Программное Обеспечение без ограничений,
// включая неограниченное право на использование, копирование, изменение, слияние, публикацию, распространение, сублицензирование
// и/или продажу копий Программного Обеспечения, а также лицам, которым предоставляется данное Программное Обеспечение, при соблюдении следующих условий:
//
// Указанное выше уведомление об авторском праве и данные условия должны быть включены во все копии или значимые части данного Программного Обеспечения.
//
// ДАННОЕ ПРОГРАММНОЕ ОБЕСПЕЧЕНИЕ ПРЕДОСТАВЛЯЕТСЯ «КАК ЕСТЬ», БЕЗ КАКИХ-ЛИБО ГАРАНТИЙ, ЯВНО ВЫРАЖЕННЫХ ИЛИ ПОДРАЗУМЕВАЕМЫХ, ВКЛЮЧАЯ ГАРАНТИИ ТОВАРНОЙ 
// ПРИГОДНОСТИ, СООТВЕТСТВИЯ ПО ЕГО КОНКРЕТНОМУ НАЗНАЧЕНИЮ И ОТСУТСТВИЯ НАРУШЕНИЙ, НО НЕ ОГРАНИЧИВАЯСЬ ИМИ. НИ В КАКОМ СЛУЧАЕ АВТОРЫ ИЛИ ПРАВООБЛАДАТЕЛИ 
// НЕ НЕСУТ ОТВЕТСТВЕННОСТИ ПО КАКИМ-ЛИБО ИСКАМ, ЗА УЩЕРБ ИЛИ ПО ИНЫМ ТРЕБОВАНИЯМ, В ТОМ ЧИСЛЕ, ПРИ ДЕЙСТВИИ КОНТРАКТА, ДЕЛИКТЕ ИЛИ ИНОЙ СИТУАЦИИ, 
// ВОЗНИКШИМ ИЗ-ЗА ИСПОЛЬЗОВАНИЯ ПРОГРАММНОГО ОБЕСПЕЧЕНИЯ ИЛИ ИНЫХ ДЕЙСТВИЙ С ПРОГРАММНЫМ ОБЕСПЕЧЕНИЕМ. 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




//Конструктор
Oregon_TM::Oregon_TM(byte tr_pin)
{
  TX_PIN = tr_pin;
  pinMode(TX_PIN, OUTPUT); 
  digitalWrite(TX_PIN, LOW);

}

Oregon_TM::Oregon_TM(void)
{
  pinMode(TX_PIN, OUTPUT); 
  digitalWrite(TX_PIN, LOW);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// Функции передатчика////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_TM::sendZero(void)
{
  if (protocol == 2){
    while (time_marker + TR_TIME * 4 >= micros());
    time_marker += TR_TIME * 4;
    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(TR_TIME - PULSE_SHORTEN_2);
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(TWOTR_TIME + PULSE_SHORTEN_2);
    digitalWrite(TX_PIN, HIGH);
  }
  if (protocol == 3)
    {
      if (prevstate) while (time_marker + TWOTR_TIME - PULSE_SHORTEN_3 >= micros());
      else while (time_marker + TWOTR_TIME >= micros());
      
    time_marker += TWOTR_TIME;
    
    if (prevbit && prevstate)
    {
      digitalWrite(TX_PIN, LOW);
      prevstate = 0;
      prevbit = 0;
      return;            
    }
    if (prevbit && !prevstate)
    {
      digitalWrite(TX_PIN, HIGH);
      delayMicroseconds(TWOTR_TIME);
      prevstate = 1;
      prevbit = 0;
      return;                  
    }
    if (!prevbit && prevstate)
    {
      digitalWrite(TX_PIN, LOW);
      delayMicroseconds(TR_TIME);
      digitalWrite(TX_PIN, HIGH);
      prevbit = 0;
      return;                  
    }
    if (!prevbit && !prevstate)
    {
      digitalWrite(TX_PIN, HIGH);
      delayMicroseconds(TR_TIME);
      digitalWrite(TX_PIN, LOW);
      prevbit = 0;
      return;                  
    }
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::sendOne(void)
{
  if (protocol == 2){
    while (time_marker + TR_TIME * 4 - PULSE_SHORTEN_2>= micros());
    time_marker += TR_TIME * 4;
    digitalWrite(TX_PIN, LOW);
    delayMicroseconds(TR_TIME + PULSE_SHORTEN_2);
    digitalWrite(TX_PIN, HIGH);
    delayMicroseconds(TWOTR_TIME - PULSE_SHORTEN_2);
    digitalWrite(TX_PIN, LOW);
  }
  
  if (protocol == 3)
  {
    if (prevstate) while (time_marker + TWOTR_TIME - PULSE_SHORTEN_3 >= micros());
    else while (time_marker + TWOTR_TIME >= micros());
    time_marker += TWOTR_TIME;
    
    if (!prevbit && prevstate)
    {
      digitalWrite(TX_PIN, LOW);
      prevstate = 0;
      prevbit = 1;
      return;
    }
    if (!prevbit && !prevstate)
    {
      digitalWrite(TX_PIN, HIGH);
      prevstate = 1;
      prevbit = 1;
      return;      
    }
    if (prevbit && prevstate)
    {
      digitalWrite(TX_PIN, LOW);
      delayMicroseconds(TR_TIME);
      digitalWrite(TX_PIN, HIGH);
      prevbit = 1;
      return;      
    }
    if (prevbit && !prevstate)
    {
      digitalWrite(TX_PIN, HIGH);
      delayMicroseconds(TR_TIME);
      digitalWrite(TX_PIN, LOW);
      prevbit = 1;
      return;      
    }
    
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::sendMSB(byte data)
{
  (bitRead(data, 4)) ? sendOne() : sendZero();
  (bitRead(data, 5)) ? sendOne() : sendZero();
  (bitRead(data, 6)) ? sendOne() : sendZero();
  (bitRead(data, 7)) ? sendOne() : sendZero();
  if (protocol == 2) time_marker += timing_corrector2;       //Поправка на разницу тактовых частот 1024.07Гц и 1024.60Гц
  if (protocol == 3) time_marker += timing_corrector3;
                     
  
}
///////////////////////////////////////////////////////////////////////////////////////////////////
 
void Oregon_TM::sendLSB(byte data)
{
  (bitRead(data, 0)) ? sendOne() : sendZero();
  (bitRead(data, 1)) ? sendOne() : sendZero();
  (bitRead(data, 2)) ? sendOne() : sendZero();
  (bitRead(data, 3)) ? sendOne() : sendZero();
  if (protocol == 2) time_marker += timing_corrector2;       //Поправка на разницу тактовых частот 1024.07Гц и 1024.60Гц
  if (protocol == 3) time_marker += timing_corrector3;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::sendData()
{
   int q = 0;
   for(byte i = 0; i < OREGON_SEND_BUFFER_SIZE; ++i)
   {
     if (q >= buffer_size) break;
     q++;
     sendMSB(SendBuffer[i]);
     if (q >= buffer_size) break;
     q++;
     sendLSB(SendBuffer[i]);
     if (protocol == 2) time_marker += 4;       //Поправка на разницу тактовых частот 1024.07Гц и 1024.60Гц
   //if (protocol == 3) time_marker += 4;
     //Поправка на разницу тактовых частот 1024.07Гц и 1024Гц
   }
}
///////////////////////////////////////////////////////////////////////////////////////////////////
 
void Oregon_TM::sendOregon()
{
    time_marker=micros();
    sendPreamble();
    sendLSB(0xA);
    sendData();
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::sendPreamble(void)
{
  if (protocol == 2){
    sendLSB(0xF);
    sendLSB(0xF);
    time_marker += 9; 
    sendLSB(0xF);
    sendLSB(0xF);
    time_marker += 9;
  }
  if (protocol == 3){
    sendLSB(0xF);
    sendLSB(0xF);
    sendLSB(0xF);
    sendLSB(0xF);
    time_marker += 4;
    sendLSB(0xF);
    sendLSB(0xF);
    time_marker += 3;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
 
void Oregon_TM::calculateAndSetChecksum132(void)
{
  byte CCIT_POLY = 0x07;
  SendBuffer[7] &= 0xF0;
  SendBuffer[8] = 0x00;
  SendBuffer[9] = 0x00;
  byte summ = 0x00;
  byte crc = 0x3C;
  byte cur_nible;
  for(int i = 0; i<8; i++) 
  {
    cur_nible = (SendBuffer[i] & 0xF0) >> 4;
    summ += cur_nible;
    if (i !=3)
    {
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
    }  
    cur_nible = SendBuffer[i] & 0x0F;
    summ += cur_nible;
    if (i !=2)
    {
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
    }  
  }
  SendBuffer[7] += summ & 0x0F;
  SendBuffer[8] += summ & 0xF0;
  SendBuffer[8] += crc & 0x0F;
  SendBuffer[9] += crc & 0xF0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::calculateAndSetChecksum318()
{
  byte CCIT_POLY = 0x07;
  SendBuffer[7] = SendBuffer[7] & 0xF0;
  SendBuffer[8] = 0x00;
  SendBuffer[9] = 0x00;
  byte summ = 0x00;
  byte crc = 0x00;
  byte cur_nible;
  for(int i = 0; i<8; i++) 
  {
    cur_nible = (SendBuffer[i] & 0xF0) >> 4;
    summ += cur_nible;
    if (i !=3)
    {
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
    }  
    cur_nible = SendBuffer[i] & 0x0F;
    summ += cur_nible;
    if (i !=2)
    {
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
    }  
  }
  SendBuffer[7] += summ & 0x0F;
  SendBuffer[8] += summ & 0xF0;
  SendBuffer[8] += crc & 0x0F;
  SendBuffer[9] += crc & 0xF0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::calculateAndSetChecksum810()
{
  byte CCIT_POLY = 0x07;
  SendBuffer[7] = SendBuffer[7] & 0xF0;
  SendBuffer[8] = 0x00;
  SendBuffer[9] = 0x00;
  byte summ = 0x00;
  byte crc = 0x0B;
  byte cur_nible;
  for(int j = 0; j < 4; j++)
  if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
  else crc <<= 1;
  
  for(int i = 0; i<8; i++) 
  {
    cur_nible = (SendBuffer[i] & 0xF0) >> 4;
    summ += cur_nible;
   
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
   
    cur_nible = SendBuffer[i] & 0x0F;
    summ += cur_nible;
   
      crc ^= cur_nible;
      for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
   
  }
  SendBuffer[7] += summ & 0x0F;
  SendBuffer[8] += summ & 0xF0;
  SendBuffer[8] += crc & 0x0F;
  SendBuffer[9] += crc & 0xF0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::SendPacket()
{
  if (sens_type == THGN132)
    calculateAndSetChecksum132();
  if (sens_type == RTGN318)
    calculateAndSetChecksum318();
  if (sens_type == THGR810)
    calculateAndSetChecksum810();
  if (sens_type == THP)
    calculateAndSetChecksumTHP();

  
  sendOregon();
  digitalWrite(TX_PIN, LOW);
  if (protocol == 2){
    delayMicroseconds(TWOTR_TIME*15);
    sendOregon();
    digitalWrite(TX_PIN, LOW);
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Функции кодирования данных//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_TM::setType(word type)
  {
    sens_type = type;
    if (type == THP)
    {
      SendBuffer[0] = 0x55;
      return;
    }
    SendBuffer[0] = (type & 0xFF00) >> 8;
    SendBuffer[1] = type & 0x00FF;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setChannel(byte channel)
  {
    byte channel_code;
    if (sens_type == THGN132)
    {
      if (channel <= 1) 
      {
        channel_code = 0x10; 
        setId(0xE3);
        send_time = 39000;
      }
      if (channel == 2) 
      {
        channel_code = 0x20; 
        setId(0xE3);
        send_time = 41000;
      }
      if (channel == 3) 
      {
        channel_code = 0x40; 
        setId(0xBB);
        send_time = 43000;
      }
      protocol = 2;
    }

    if (sens_type == RTGN318)
    {

      if (channel <= 1) 
      {
        channel_code = 0x10; 
        setId(0xF1);
        send_time = 53000;
      }
      if (channel == 2) 
      {
        channel_code = 0x20; 
        setId(0x92);
        send_time = 59000;
      }
      if (channel == 3) 
      {
        channel_code = 0x30; 
        setId(0xAA);
        send_time = 61000;
      }

      if (channel == 4) 
      {
        channel_code = 0x40; 
        setId(0x8A);
        send_time = 67000;
      }
      
      if (channel >= 5) 
      {
        channel_code = 0x50; 
        setId(0xB1);
        send_time = 71000;
      }
      protocol = 2;
    }

if (sens_type == THGR810)
    {
      if (channel <= 1) 
      {
        channel_code = 0x10; 
        setId(0xCB);
        send_time = 53000;
      }
      if (channel == 2) 
      {
        channel_code = 0x20; 
        setId(0x69);
        send_time = 59000;
      }
      if (channel == 3) 
      {
        channel_code = 0x30; 
        setId(0xAA);
        send_time = 61000;
      }
      if (channel == 4) 
      {
        channel_code = 0x40; 
        setId(0x8A);
        send_time = 67000;
      }
      if (channel == 5) 
      {
        channel_code = 0x50; 
        setId(0xB1);
        send_time = 71000;
      }
      if (channel == 6) 
      {
        channel_code = 0x60; 
        send_time = 79000;
      }
      if (channel == 7) 
      {
        channel_code = 0x70; 
        send_time = 83000;
      }
      if (channel == 8) 
      {
        channel_code = 0x80; 
        send_time = 87000;
      }
      if (channel == 9) 
      {
        channel_code = 0x90; 
        send_time = 91000;
      }
      if (channel >= 10) 
      {
        channel_code = 0xA0; 
        send_time = 93000;
      }
      protocol = 3;
    }
      SendBuffer[2]&= 0x0F;
      SendBuffer[2] += channel_code & 0xF0;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setId(byte ID)
  {
    SendBuffer[2]&= 0xF0;
    SendBuffer[2] += (ID & 0xF0) >> 4;
    SendBuffer[3]&= 0x0F;
    SendBuffer[3] += (ID & 0x0F) << 4;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setBatteryFlag(bool level)
  {
    SendBuffer[3] &= 0xFB;
    if (level) SendBuffer[3] |= 0x04;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setStartCount(byte startcount)
  {
     SendBuffer[3] &= 0xF4;
    if (startcount == 8) SendBuffer[3] |= 0x08;
    if (startcount == 2) SendBuffer[3] |= 0x02;
    if (startcount == 1) SendBuffer[3] |= 0x01;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////
  
void Oregon_TM::setTemperature(float temp)
  {
    if(temp < 0)
    {
      SendBuffer[5] = 0x08;
      temp *= -1;
    }
    else
    {
      SendBuffer[5] = 0x00;
    }
    byte tempInt = (byte) temp;
    byte td = (tempInt / 10);
    byte tf = tempInt - td * 10;
    byte tempFloat = (temp - (float)tempInt) * 10;
    
    SendBuffer[5] += (td << 4);
    SendBuffer[4] = tf;
    SendBuffer[4] |= (tempFloat << 4);
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setHumidity(byte hum)
  {
      SendBuffer[6] = (hum/10);
      SendBuffer[6] += (hum - (SendBuffer[6] * 10)) << 4;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

void Oregon_TM::setComfort(float temp, byte hum)
  {
      if (hum > 70)
     {
      SendBuffer[7] = 0xC0;
      return;
     }
      if (hum < 40)
     {
      SendBuffer[7] = 0x80;
      return;
     }
      if (temp > 20 && temp < 25)
     {
      SendBuffer[7] = 0x40;
      return;
     }
     else SendBuffer[7] = 0x00;
     return;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Oregon_TM::transmit()
{
  if (millis() >= time_marker_send && send_time) 
  {
    SendPacket();
    time_marker_send = millis() + send_time; 
    return true;
  }
  else return false;
}






///////////////////////////////////////////////////////////////////////////////////////////////////
//Поддержка датчика THP
///////////////////////////////////////////////////////////////////////////////////////////////////


void Oregon_TM::setChannelTHP(byte channel)
  {
  SendBuffer[1] &= 0x0F;
  SendBuffer[1] += channel << 4;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////  
void Oregon_TM::setBatteryTHP( word bat_voltage)
  {
    SendBuffer[6] = (bat_voltage & 0x0FF0) >> 4;
    SendBuffer[7] &= 0x0F;
    SendBuffer[7] += (bat_voltage & 0x000F) << 4;
   
  }
///////////////////////////////////////////////////////////////////////////////////////////////////  
void Oregon_TM::setTemperatureTHP(float bme_temperature)
  {
    word temp_code;
    if (bme_temperature < -100 || bme_temperature > 100) temp_code = 0x0FFF;
    else temp_code = (word)((bme_temperature + 100) * 10);
    SendBuffer[2] = temp_code & 0x00FF;
    SendBuffer[1] &= 0xF0;
    SendBuffer[1] += (temp_code & 0x0F00) >> 8;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////  
void Oregon_TM::setHumidityTHP(float bme_humidity)
  {
    word hum_code;
    if (bme_humidity > 100) hum_code = 0x0FFF;
    else hum_code = (word)(bme_humidity * 10);
    SendBuffer[3] = (hum_code & 0x0FF0) >> 4;
    SendBuffer[4] &= 0x0F;
    SendBuffer[4] += (hum_code & 0x000F) << 4;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_TM::setPressureTHP(float bme_pressure)
  {
    word pres_code;
    if (bme_pressure < 500) pres_code = 0x0000;
    else pres_code = (word)((bme_pressure - 500) * 10);
    SendBuffer[5] = pres_code & 0x00FF;
    SendBuffer[4] &= 0xF0;
    SendBuffer[4] += (pres_code & 0x0F00) >> 8;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_TM::setErrorTHP()
  {
    SendBuffer[1] |= 0x0F;
    SendBuffer[2] = 0xFF;
    SendBuffer[3] = 0xFF;
    SendBuffer[4] = 0xFF;
    SendBuffer[5] = 0xFF;
  }
///////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_TM::calculateAndSetChecksumTHP()
{
  byte CCIT_POLY = 0x07;
  SendBuffer[7] = SendBuffer[7] & 0xF0;
  SendBuffer[8] = 0x00;
  SendBuffer[9] = 0x00;
  byte summ = 0x00;
  byte crc = 0x00;
  byte cur_nible;
  for(int i = 0; i<8; i++) 
  {
    cur_nible = (SendBuffer[i] & 0xF0) >> 4;
    summ += cur_nible;
    crc ^= cur_nible;
    for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
      
    cur_nible = SendBuffer[i] & 0x0F;
    summ += cur_nible;
    crc ^= cur_nible;
    for(int j = 0; j < 4; j++)
      if (crc & 0x80) crc = (crc << 1) ^ CCIT_POLY;
      else crc <<= 1;
  }
  SendBuffer[7] += summ & 0x0F;
  SendBuffer[8] += summ & 0xF0;
  SendBuffer[8] += crc & 0x0F;
  SendBuffer[9] += crc & 0xF0;
}

