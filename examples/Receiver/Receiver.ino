#include <Oregon_NR.h>

#if defined ( ESP8266 ) || ( ESP32 )// Для Wemos
  Oregon_NR oregon(13, 13,          // приёмник на выводе D7 (GPIO13)
                        2, true,    // Светодиод на D2 подтянут к +пит(true). Если светодиод не нужен, то номер вывода - 255
                        50, true);  // Буфер на приём посылки из 50 ниблов, включена сборка пакетов для v2
                                    
#else                               // Для AVR
  Oregon_NR oregon(2, 0,            // Приёмник на выводе D2, Прерывание 0, 
                     13, false);    // Светодиод приёма на вывод 13
                                    // По умолчанию Буфер на приём посылки стандартный - на 24 нибла, включена сборка пакетов для v2  
                                    
//Oregon_NR oregon(2, 0); 	        // Конструктор по умолчанию, со стандартным буфером и без светодиода
#endif

void setup() {
   Serial.begin(115200);
   Serial.println();  
   if (oregon.no_memory)
   {
    Serial.println("NO MEMORY");   //Не хватило памяти
    do yield();
    while(true);
   }
   
  //вкючение прослушивания радиоканала  
  oregon.start(); 
  oregon.receiver_dump = 0;       //true - Включает "осциллограф" - отображение данных, полученных с приёмника
}

void loop() {
  //////////////////////////////////////////////////////////////////////
  //Захват пакета,/////////////////////////////////////////////////////
  oregon.capture(0); // 1 - выводить в Serial сервисную информацию
  
  //Захваченные данные годны до следующего вызова capture
  //ОБработка полученного пакета//////////////////////////////////////////////
  if (oregon.captured)  {
    //Вывод информации в Serial
    Serial.print ((float) millis() / 1000, 1); //Время
    Serial.print ("s\t\t");
    //Версия протокола
    if (oregon.ver == 2) Serial.print("  ");
    if (oregon.ver == 3) Serial.print("3 ");
    
    //Информация о восстановлени пакета
    if (oregon.restore_sign & 0x01) Serial.print("s"); //восстановлены одиночные такты
    else  Serial.print(" ");
    if (oregon.restore_sign & 0x02) Serial.print("d"); //восстановлены двойные такты
    else  Serial.print(" ");
    if (oregon.restore_sign & 0x04) Serial.print("p "); //исправленна ошибка при распознавании версии пакета
    else  Serial.print("  ");
    if (oregon.restore_sign & 0x08) Serial.print("r "); //собран из двух пакетов (для режима сборки в v.2)
    else  Serial.print("  ");

    //Вывод полученного пакета.
    for (int q = 0;q < oregon.packet_length; q++)
      if (oregon.valid_p[q] == 0x0F) Serial.print(oregon.packet[q], HEX);
      else Serial.print(" ");
        
    //Время обработки пакета
    Serial.print("  ");
    Serial.print(oregon.work_time);
    Serial.print("ms ");
    
    if ((oregon.sens_type == THGN132 ||
    (oregon.sens_type & 0x0FFF) == RTGN318 ||
    (oregon.sens_type & 0x0FFF) == RTHN318 ||
    oregon.sens_type == THGR810 ||
    oregon.sens_type == THN132 ||
    oregon.sens_type == THN800 ||
    oregon.sens_type == BTHGN129 ||
    oregon.sens_type == BTHR968 ||
    oregon.sens_type == THGN500) && oregon.crc_c){
      Serial.print("\t");
      
      if (oregon.sens_type == THGN132) Serial.print("THGN132N");
      if (oregon.sens_type == THGN500) Serial.print("THGN500 ");
      if (oregon.sens_type == THGR810) Serial.print("THGR810 ");
      if ((oregon.sens_type & 0x0FFF) == RTGN318) Serial.print("RTGN318 ");
      if ((oregon.sens_type & 0x0FFF) == RTHN318) Serial.print("RTHN318 ");
      if (oregon.sens_type == THN132 ) Serial.print("THN132N ");
      if (oregon.sens_type == THN800 ) Serial.print("THN800  ");
      if (oregon.sens_type == BTHGN129 ) Serial.print("BTHGN129");
      if (oregon.sens_type == BTHR968 ) Serial.print("BTHR968 ");

      if (oregon.sens_type != BTHR968 && oregon.sens_type != THGN500)
      {
        Serial.print(" CHNL: ");
        Serial.print(oregon.sens_chnl);
      }
      else Serial.print("        ");
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
      
      if (oregon.sens_tmp >= 0 && oregon.sens_tmp < 10) Serial.print(" TMP:  ");
      if (oregon.sens_tmp < 0 && oregon.sens_tmp >-10) Serial.print(" TMP: ");
      if (oregon.sens_tmp <= -10) Serial.print(" TMP:");
      if (oregon.sens_tmp >= 10) Serial.print(" TMP: ");
      Serial.print(oregon.sens_tmp, 1);
      Serial.print("C ");
      if (oregon.sens_type == THGN132 ||
          oregon.sens_type == THGR810 ||
          oregon.sens_type == BTHGN129 ||
          oregon.sens_type == BTHR968 ||
          (oregon.sens_type & 0x0FFF) == RTGN318 ||
          oregon.sens_type == THGN500 ) {
        Serial.print("HUM: ");
        Serial.print(oregon.sens_hmdty, 0);
        Serial.print("%");
      }
      else Serial.print("        ");

      if (oregon.sens_type == BTHGN129 ||  oregon.sens_type == BTHR968)
      {
      Serial.print(" PRESS: ");
      Serial.print(oregon.get_pressure(), 1);
      Serial.print("Hgmm ");
      }
    }

  if (oregon.sens_type == WGR800 && oregon.crc_c){
      Serial.print("\tWGR800  ");
      Serial.print("        ");
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
      
      Serial.print(" AVG: ");
      Serial.print(oregon.sens_avg_ws, 1);
      Serial.print("m/s  MAX: ");
      Serial.print(oregon.sens_max_ws, 1);
      Serial.print("m/s  DIR: "); //N = 0, E = 4, S = 8, W = 12
      switch (oregon.sens_wdir)
      {
      case 0: Serial.print("N"); break;
      case 1: Serial.print("NNE"); break;
      case 2: Serial.print("NE"); break;
      case 3: Serial.print("NEE"); break;
      case 4: Serial.print("E"); break;
      case 5: Serial.print("SEEE"); break;
      case 6: Serial.print("SE"); break;
      case 7: Serial.print("SSE"); break;
      case 8: Serial.print("S"); break;
      case 9: Serial.print("SSW"); break;
      case 10: Serial.print("SW"); break;
      case 11: Serial.print("SWW"); break;
      case 12: Serial.print("W"); break;
      case 13: Serial.print("NWW"); break;
      case 14: Serial.print("NW"); break;
      case 15: Serial.print("NNW"); break;
      }
      
    }    

    if (oregon.sens_type == UVN800 && oregon.crc_c){
      Serial.print("\tUVN800  ");
      Serial.print("        ");
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
      
      Serial.print(" UV IDX: ");
      Serial.print(oregon.UV_index);
      
    }    

    if (oregon.sens_type == PCR800 && oregon.crc_c){
      Serial.print("\tPCR800  ");
      Serial.print("        ");
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print(" ID: ");
      Serial.print(oregon.sens_id, HEX);
      Serial.print("   TOTAL: ");
      Serial.print(oregon.get_total_rain(), 1);
      Serial.print("mm  RATE: ");
      Serial.print(oregon.get_rain_rate(), 1);
      Serial.print("mm/h");
      
    }    
    
#if ADD_SENS_SUPPORT == 1
      if ((oregon.sens_type & 0xFF00) == THP && oregon.crc_c) {
      Serial.print("\tTHP     ");
      Serial.print(" CHNL: ");
      Serial.print(oregon.sens_chnl);
      Serial.print(" BAT: ");
      Serial.print(oregon.sens_voltage, 2);
      Serial.print("V");
      if (oregon.sens_tmp > 0 && oregon.sens_tmp < 10) Serial.print(" TMP:  ");
      if (oregon.sens_tmp < 0 && oregon.sens_tmp > -10) Serial.print(" TMP: ");
      if (oregon.sens_tmp <= -10) Serial.print(" TMP:");
      if (oregon.sens_tmp >= 10) Serial.print(" TMP: ");
      Serial.print(oregon.sens_tmp, 1);
      Serial.print("C ");
      Serial.print("HUM: ");
      Serial.print(oregon.sens_hmdty, 1);
      Serial.print("% ");
      Serial.print("PRESS: ");
      Serial.print(oregon.sens_pressure, 1);
      Serial.print("Hgmm");
      
    }
#endif
    Serial.println();
  }
  yield();
}
