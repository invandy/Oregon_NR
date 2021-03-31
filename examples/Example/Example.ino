#include <Oregon_NR.h>
#include <ESP8266WiFi.h>

#define WEMOS_D1
//#define UNO

#ifdef UNO
Oregon_NR oregon(2, 0, 13); // Для Arduino UNO/Nano - датчик на выводе D2, Прерывание 0, Светодиод приёма на вывод 13
//Oregon_NR oregon(2, 0); // Если светодиод не нужен
#endif

#ifdef WEMOS_D1
Oregon_NR oregon(4, 4, 2, true); // для Wemos D1 - датчик на выводе D4 (GPIO2), Светодиод на D2 подтянут к +пит.
#endif

void setup() {
   Serial.begin(115200);
  //вкючение прослушивания радиоканала  
  oregon.start(); 
}

void loop() {
  //////////////////////////////////////////////////////////////////////
  //Захват пакета,//////////////////////////////////////////////
  oregon.capture(1);
  //Захваченные данные годны до следующего вызова capture
  //ОБработка полученного пакета//////////////////////////////////////////////
  if (oregon.captured)  {
    //Вывод информации в Serial
    if ((oregon.sens_type == THGN132 || oregon.sens_type == THN132) && oregon.crc_c){
      Serial.print("PERIOD:\t");
      Serial.print(millis()/40000);
      Serial.print("\t");
      if (oregon.sens_type == THGN132)  for (int q = 0;q < PACKET_LENGTH - 2; q++) Serial.print(oregon.packet[q], HEX);
      if (oregon.sens_type == THN132)  for (int q = 0;q < PACKET_LENGTH - 4; q++) Serial.print(oregon.packet[q], HEX);
      Serial.print(" TYPE: ");
      if (oregon.sens_type == THGN132) Serial.print("THGN132N");
      if (oregon.sens_type == THN132) Serial.print("THN132N ");
      Serial.print(" CHNL: ");
      Serial.print(oregon.sens_chnl);
      if (oregon.sens_tmp > 0 && oregon.sens_tmp < 10) Serial.print(" TMP:  ");
      if (oregon.sens_tmp < 0 && oregon.sens_tmp >-10) Serial.print(" TMP: ");
      if (oregon.sens_tmp <= -10) Serial.print(" TMP:");
      if (oregon.sens_tmp >= 10) Serial.print(" TMP: ");
      Serial.print(oregon.sens_tmp, 1);
      Serial.print("C ");
      if (oregon.sens_type == THGN132) {
        Serial.print("HUM: ");
        Serial.print(oregon.sens_hmdty, 1);
        Serial.print("%");
      }
            
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
      //Serial.print(" PACKETS: ");
      //Serial.print(oregon.packets_received);
      //Serial.print(" PROC. TIME: ");
      //Serial.print(oregon.work_time);
      //Serial.println("ms ");
      Serial.println("");
    }
  }
}

