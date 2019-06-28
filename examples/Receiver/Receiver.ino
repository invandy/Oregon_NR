#include <Oregon_NR.h>
//#include <Oregon_TR.h>

#define UNO

#ifdef UNO
//Oregon_NR oregon(2, 0, 13); // Для Arduino UNO/Nano - приёмник на выводе D2, Прерывание 0, Светодиод приёма на вывод 13
Oregon_NR oregon(2, 0); // Если светодиод не нужен
#endif

#ifdef WEMOS_D1
Oregon_NR oregon(13, 13, 2, true); // для Wemos D1 - датчик на выводе D7 (GPIO13), Светодиод на D2 подтянут к +пит.
#endif

void setup() {
   Serial.begin(115200);
  //вкючение прослушивания радиоканала  
  oregon.start(); 
  oregon.receiver_dump = 1;       //true - Включает "осциллограф" - отображение данных, полученных с приёмника
  
  //Если не распознаются последние байты принятого пакета можно попробовать подстроить частоту захвата данных
  oregon.timing_correction = 0;  // коррекция частоты завхвата данных. Обычно достаоточно подобрать значение от -5 до 5
}

void loop() {
  //////////////////////////////////////////////////////////////////////
  //Захват пакета,/////////////////////////////////////////////////////
  oregon.capture(1); // 1 - выводить в Serial сервисную информацию
  
  //Захваченные данные годны до следующего вызова capture
  //ОБработка полученного пакета//////////////////////////////////////////////
  if (oregon.captured)  {
    //Вывод информации в Serial
    Serial.print (millis() / 1000); //Время
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

    //Вывод полученного пакета. Точки - это ниблы, содержащие сомнительные биты
    for (int q = 0;q < PACKET_LENGTH - 1; q++)
      if (oregon.valid_p[q] == 0x0F) Serial.print(oregon.packet[q], HEX);
      else Serial.print(".");
        
    //Время обработки пакета
    Serial.print("  ");
    Serial.print(oregon.work_time);
    Serial.print("ms ");
    
    if ((oregon.sens_type == THGN132 || (oregon.sens_type & 0x0FFF) == RTGN318 || oregon.sens_type == THGR810 || oregon.sens_type == THN132) && oregon.crc_c){
      Serial.print("\t");
      Serial.print(" TYPE: ");
      if (oregon.sens_type == THGN132) Serial.print("THGN132N");
      if (oregon.sens_type == THGR810) Serial.print("THGR810 ");
      if ((oregon.sens_type & 0x0FFF) == RTGN318) Serial.print("RTGN318");
      if (oregon.sens_type == THN132) Serial.print("THN132N ");
      Serial.print(" CHNL: ");
      Serial.print(oregon.sens_chnl);
      if (oregon.sens_tmp >= 0 && oregon.sens_tmp < 10) Serial.print(" TMP:  ");
      if (oregon.sens_tmp < 0 && oregon.sens_tmp >-10) Serial.print(" TMP: ");
      if (oregon.sens_tmp <= -10) Serial.print(" TMP:");
      if (oregon.sens_tmp >= 10) Serial.print(" TMP: ");
      Serial.print(oregon.sens_tmp, 1);
      Serial.print("C ");
      if (oregon.sens_type == THGN132 || oregon.sens_type == THGR810 || (oregon.sens_type & 0x0FFF) == RTGN318) {
        Serial.print("HUM: ");
        Serial.print(oregon.sens_hmdty, 0);
        Serial.print("%");
      }
      else Serial.print("        ");
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
    }

  if (oregon.sens_type == WGR800 && oregon.crc_c){
      Serial.print("\t");
      float wnddata;
      Serial.print(" TYPE: ");
      Serial.print("WGR800");
      Serial.print(" AVG WS: ");
      Serial.print(oregon.sens_avg_ws, 1);
      Serial.print("m/s MAX WS: ");
      Serial.print(oregon.sens_max_ws, 1);
      Serial.print("m/s WDIR: "); //N = 0, E = 4, S = 8, W = 12
      Serial.print(oregon.sens_wdir);
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
    }    

    if (oregon.sens_type == UVN800 && oregon.crc_c){
      Serial.print("\t");
      float wnddata;
      Serial.print(" TYPE: ");
      Serial.print("UVN800");
      Serial.print(" UV IDX: ");
      Serial.print(oregon.UV_index);
      Serial.print(" BAT: ");
      if (oregon.sens_battery) Serial.print("F "); else Serial.print("e ");
      Serial.print("ID: ");
      Serial.print(oregon.sens_id, HEX);
    }    
    
#if ADD_SENS_SUPPORT == 1
      if ((oregon.sens_type & 0xFF00) == THP && oregon.crc_c) {
      Serial.print("\t");
      Serial.print(" TYPE: ");
      Serial.print("THP");
      Serial.print(" CHNL: ");
      Serial.print(oregon.sens_chnl);
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
      Serial.print(" BAT: ");
      Serial.print(oregon.sens_voltage, 2);
      Serial.print("V");
    }
#endif
    Serial.println();
  }
}
