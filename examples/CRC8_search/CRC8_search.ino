#include <Oregon_NR.h>

Oregon_NR oregon(2, 0);
  byte t_sum;
  byte t_crc;
  byte packet_size;
  byte data1[100], data2[100], data3[100];
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Утилита для поиска параметров расчёта CRC8 для датчиков Oregon Scientific
// используется при добавлении новых датчиков в базу библиотеки
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);     
  Serial.println();
  Serial.println("POLY\tSTART_SUM");
  
  Serial.println("THN132");
  hexCharacterStringToBytes(data1,"EC401B183520D33F");
  hexCharacterStringToBytes(data2,"EC401B182520C34A");
  hexCharacterStringToBytes(data3,"EC401AD272203444");
  packet_size = 16;
  Calc();
  
  Serial.println("THGN132");
  hexCharacterStringToBytes(data1,"1D204BBC0058898C5E5");
  hexCharacterStringToBytes(data2,"1D2011B04320528530F");
  hexCharacterStringToBytes(data3,"1D201BF85620528F43F");
  packet_size = 19;
  Calc();

  Serial.println("UVN800");
  hexCharacterStringToBytes(data1,"D874156000F60145B");
  hexCharacterStringToBytes(data2,"D874156010580a32f");
  hexCharacterStringToBytes(data3,"D874156010580a32f");
  packet_size = 17;
  Calc();

  Serial.println("RTGN318");
  hexCharacterStringToBytes(data1,"BCC31190230008CA49A");
  hexCharacterStringToBytes(data2,"ACC33E884320038A542");
  hexCharacterStringToBytes(data3,"CCC32EA80520038B571");
  packet_size = 19;
  Calc();

  Serial.println("THGR810");
  hexCharacterStringToBytes(data1,"F82428307320318245E");
  hexCharacterStringToBytes(data2,"F824283083203183473");
  hexCharacterStringToBytes(data3,"F824283083203183473");
  packet_size = 19;
  Calc();

  Serial.println("I300");
  hexCharacterStringToBytes(data1,"2915012A3021010171700171710F6F62E1020006032E8AE07");
  hexCharacterStringToBytes(data2,"2915012A3021010171700171710F6F62E1020006032E8AEC5");
  hexCharacterStringToBytes(data3,"2915012A3021010171700171710F6F62E1020007032F8DBAA");
  packet_size = 47;
  Calc();

  Serial.println("THGN500");
  hexCharacterStringToBytes(data1,"1D301C94950016004F1");
  hexCharacterStringToBytes(data2,"1D301C94950016004F1");
  hexCharacterStringToBytes(data3,"1D301C94950016004F1");
  packet_size = 19;
  Calc();



// I300  
//  hexCharacterStringToBytes(data1,"00A5012A302400000000000000000000000000000000052F6");
//  hexCharacterStringToBytes(data2,"00A5012A3023000000000000000000000000000000000421C");
//  hexCharacterStringToBytes(data3,"00A5012A30220000000000000000000000000000000003212");
//  packet_size = 49;


Serial.println("DONE");
}

void loop()
{
 delay(100); 
}

void hexCharacterStringToBytes(byte* dat, String my_str)
{
  byte* dt = dat;
  const char* cch = my_str.c_str();
  for (int i = 0; i < my_str.length(); i++)
  {
    if (*cch == '0') *dt = 0;
    if (*cch == '1') *dt = 1;
    if (*cch == '2') *dt = 2;
    if (*cch == '3') *dt = 3;
    if (*cch == '4') *dt = 4;
    if (*cch == '5') *dt = 5;
    if (*cch == '6') *dt = 6;
    if (*cch == '7') *dt = 7;
    if (*cch == '8') *dt = 8;
    if (*cch == '9') *dt = 9;
    if (*cch == 'A' || *cch == 'a') *dt = 10;
    if (*cch == 'B' || *cch == 'b') *dt = 11;
    if (*cch == 'C' || *cch == 'c') *dt = 12;
    if (*cch == 'D' || *cch == 'd') *dt = 13;
    if (*cch == 'E' || *cch == 'e') *dt = 14;
    if (*cch == 'F' || *cch == 'f') *dt = 15;
    dt++;
    cch++; 
  }
}

void Calc()
{
  int j, k;
  
  for(j = 0; j < 256; j++) 
  {
    for(k = 0; k < 256; k++) 
    {
      if (oregon.check_oregon_crcsum(data1, t_crc, t_sum, packet_size) && oregon.check_oregon_crcsum(data2, t_crc, t_sum, packet_size) && oregon.check_oregon_crcsum(data3, t_crc, t_sum, packet_size))
      {
      Serial.print(t_crc,HEX);
      Serial.print("h\t");
      Serial.print(t_sum,HEX);
      Serial.println('h');
      }
      t_sum++;
    }
    t_crc++;
  }
 Serial.println(); 
 yield();
}
