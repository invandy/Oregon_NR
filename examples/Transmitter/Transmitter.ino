#include <Oregon_TM.h>

//Имитатор датчиков температуры и влажности Oregon Scientific

//В данном примере иммитируются пакеты следующих типов датчиков:
//THGN132N (тип 1D20h, протокол v2, 3 канала) 
//RTGN318  (тип ХСС3h, протокол v2, 5 каналов)
//THGR810  (тип F824h, протокол v3, 10 каналов)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Вывод передатчика один. Указывается только при создании первого объекта. В данном примере передатчик подключен к D4

Oregon_TM transmitter(4), transmitter2, transmitter3;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);     
  
  //Первый передатчик
  transmitter.setType(THGN132);
  transmitter.setChannel(3);        // Номер канала для THGN132 - 1...3
  transmitter.setBatteryFlag(1);    // Флаг разряженной батарейки
  transmitter.setTemperature(24.2); // -50C...+70C
  transmitter.setHumidity(13);      // 2...98%
  transmitter.setComfort(24.2, 13); // Расчёт передаваемого индекса комфорта

  //Второй передатчик
  transmitter2.setType(THGR810);    
  transmitter2.setChannel(1);       // Номер канала для THGR810 - 1...10 (Возможно приниматься базой будут тольок первые 5 каналов. Проверить мне не на чем...)
  transmitter2.setBatteryFlag(1); 
  transmitter2.setTemperature(+49.9); 
  transmitter2.setHumidity(98); 
  transmitter2.setComfort(+49.9, 98); 

  //Третий передатчик
  transmitter3.setType(RTGN318);
  transmitter3.setChannel(5);       // Номер канала для RTGN318 - 1...5. 
  transmitter3.setBatteryFlag(0); 
  transmitter3.setTemperature(-31); 
  transmitter3.setHumidity(50); 
  transmitter3.setComfort(-31,50); 
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
    //передача осуществляется по таймеру, который определяется типом датчика и номером канала
    
    if (transmitter.transmit()) PrintSentData(transmitter.SendBuffer);
    if (transmitter2.transmit()) PrintSentData(transmitter2.SendBuffer);
    if (transmitter3.transmit()) PrintSentData(transmitter3.SendBuffer);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintSentData(byte* buf)
{
      Serial.print(millis() / 1000);
      Serial.print("s \t\t");
      for (byte i = 0; i < 10; ++i)   
      {
        byte trmbuf = *buf;
        Serial.print(trmbuf >> 4, HEX);
        if (i < (10 - 1) )
        Serial.print(trmbuf & 0x0F, HEX);
        buf++;
      }  
      Serial.println();
}

