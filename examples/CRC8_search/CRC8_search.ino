#include <Oregon_NR.h>

Oregon_NR oregon(2, 0);
  byte t_sum;
  bool OSV3;
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
  Serial.println("VERSION\tPOLY\tSTART_SUM");
  Serial.println();

  Serial.println("BTHGN129");
  hexCharacterStringToBytes(data1,"5D5323B051207389D13E528");
  hexCharacterStringToBytes(data2,"5D5314D01510950AC13B529");
  hexCharacterStringToBytes(data3,"5D5323B051207389D13E528");
  packet_size = 23;
  Calc();

   
  Serial.println("THGR810");
  //hexCharacterStringToBytes(data1,"F8242248952031894F0");
  //hexCharacterStringToBytes(data1,"F8242248752031874BC");
  //hexCharacterStringToBytes(data1,"F82414C842206408469");
  //hexCharacterStringToBytes(data1,"F8242248652031864DD");
  //hexCharacterStringToBytes(data1,"F824283083203183473");
  //hexCharacterStringToBytes(data1,"F82428307320318245E");
  //hexCharacterStringToBytes(data1,"F82426901610440E3A0");
  //hexCharacterStringToBytes(data1,"F82414C842206408469");
  //hexCharacterStringToBytes(data1,"F82411485220340B322");
  hexCharacterStringToBytes(data1,"F82426901610440E3A0");
  hexCharacterStringToBytes(data2,"F82414C842206408469");
  hexCharacterStringToBytes(data3,"F82411485220340B322");
  packet_size = 19;
  Calc();

  Serial.println("THN800");
  hexCharacterStringToBytes(data1,"C8441EE06810842D");
  hexCharacterStringToBytes(data2,"C8441EE0481064C7");
  hexCharacterStringToBytes(data3,"C8441EE028104498");
  packet_size = 16;
  Calc();

  Serial.println("PCR800");
  hexCharacterStringToBytes(data1,"29140EC00000279410142E");
  hexCharacterStringToBytes(data2,"29140EC027900225106402");
  hexCharacterStringToBytes(data3,"29140EC04900583510D4C7");
  packet_size = 22;
  Calc();
  
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

  Serial.println("I300 HUB");
  hexCharacterStringToBytes(data1,"2915012A3021010171700171710F6F62E1020006032E8AE07");
  hexCharacterStringToBytes(data2,"2915012A3021010171700171710F6F62E1020006032E8AEC5");
  hexCharacterStringToBytes(data3,"2915012A3021010171700171710F6F62E1020007032F8DBAA");
  packet_size = 47;
  Calc();

  Serial.println("I300 HUB");
  hexCharacterStringToBytes(data1,"00A5012A302400000000000000000000000000000000052F6");
  hexCharacterStringToBytes(data2,"00A5012A3023000000000000000000000000000000000421C");
  hexCharacterStringToBytes(data3,"00A5012A30220000000000000000000000000000000003212");
  packet_size = 49;
  Calc();
  
  Serial.println("BTHR968");
  hexCharacterStringToBytes(data1,"5D600A50332004857933511");
  hexCharacterStringToBytes(data2,"5D600A50332004857933511");
  hexCharacterStringToBytes(data3,"5D600A50332004857933511");
  packet_size = 23;
  Calc();
  

  Serial.println("THGN500");
  hexCharacterStringToBytes(data1,"1D301C94950016004F1");
  hexCharacterStringToBytes(data2,"1D301C94950016004F1");
  hexCharacterStringToBytes(data3,"1D301C94950016004F1");
  packet_size = 19;
  Calc();

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
  for (byte vr = 0; vr < 2; vr++)
  {
  for(j = 0; j < 256; j++) 
  {
    for(k = 0; k < 256; k++) 
    {
      if (oregon.check_oregon_crcsum(data1, t_crc, t_sum, packet_size, vr) && oregon.check_oregon_crcsum(data2, t_crc, t_sum, packet_size, vr) && oregon.check_oregon_crcsum(data3, t_crc, t_sum, packet_size, vr))
      {
      if (vr) Serial.print("3");
      else  Serial.print("2");
      Serial.print("\t");
      Serial.print(t_crc,HEX);
      Serial.print("h\t");
      Serial.print(t_sum,HEX);
      Serial.println('h');
      }
      t_sum++;
    }
    t_crc++;
  }
  }
 Serial.println(); 
 yield();
}
