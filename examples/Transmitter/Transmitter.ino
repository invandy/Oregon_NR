#include <Oregon_TM.h>

//Имитатор датчиков температуры и влажности Oregon Scientific

//В данном примере иммитируются пакеты следующих типов датчиков:
//THGN132N (тип 1D20h, протокол v2, 3 канала) 
//RTGN318  (тип ХСС3h, протокол v2, 5 каналов)
//THGR810  (тип F824h, протокол v3, 10 каналов)
//BTHR968  (тип 5D60h, протокол v2, 1 канал) 
//BTHGN129 (тип 5D53h, протокол v2, 5 каналов) 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Вывод передатчика один. Указывается только при создании первого объекта. В данном примере передатчик подключен к D4

Oregon_TM transmitter(4,19), transmitter2(4,19), transmitter3(4,19), transmitter4(4, 24), transmitter5(4, 24); //вывод передатчика достаточно указать один раз, 
//Oregon_TM transmitter(4), transmitter2, transmitter3, transmitter4, transmitter5; //вывод передатчика достаточно указать один раз, 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup()
{
  Serial.begin(115200);     
  
  //THGN132
  transmitter.setType(THGN132);
  transmitter.setChannel(3);        // Номер канала для THGN132 - 1...3
  transmitter.setBatteryFlag(1);    // Флаг разряженной батарейки
  transmitter.setTemperature(24.2); // -49.9C...+69.9C
  transmitter.setHumidity(30);      // 2...98%
  transmitter.setComfort(24.2, 30); // Расчёт передаваемого индекса комфорта
  transmitter.buffer_size = 19;

  //THGR810
  transmitter2.setType(THGR810);    
  transmitter2.setChannel(1);       // Номер канала для THGR810 - 1...10 (Возможно приниматься базой будут тольок первые 5 каналов. Проверить мне не на чем...)
  transmitter2.setBatteryFlag(1); 
  transmitter2.setTemperature(24.3); // -49.9C...+69.9C
  transmitter2.setHumidity(50);      // 2...98%
  transmitter2.setComfort(24.3, 50); // Расчёт передаваемого индекса комфорта
  transmitter2.buffer_size = 19;

  //RTGN318
  transmitter3.setType(RTGN318);
  transmitter3.setChannel(5);       // Номер канала для RTGN318 - 1...5. 
  transmitter3.setBatteryFlag(0); 
  transmitter3.setTemperature(-24.2); // -49.9C...+69.9C
  transmitter3.setHumidity(98);      // 2...98%
  transmitter3.setComfort(-24.2, 98); // Расчёт передаваемого индекса комфорта
  transmitter3.buffer_size = 19;

  //Третий передатчик
  transmitter4.setType(BTHGN129);
  transmitter4.setChannel(1);       // Номер канала для BTHGN129 - 1...5. 
  transmitter4.setBatteryFlag(0); 
  transmitter4.setTemperature(-49.9); // -49.9C...+69.9C
  transmitter4.setHumidity(3);      // 2...98%
  transmitter4.setComfort(-49.9, 3); // Расчёт передаваемого индекса комфорта
  transmitter4.setPressure(760.0); 
  transmitter4.buffer_size = 23;

  //Третий передатчик
  transmitter5.setType(BTHR968);
  transmitter5.setChannel(0);       // BTHR968 имеет 1 канал, но вызвать процедуру надо
  transmitter5.setBatteryFlag(0); 
  transmitter5.setTemperature(49.9); // -49.9C...+69.9C
  transmitter5.setHumidity(97);      // 2...98%
  transmitter5.setComfort(49.9, 97); // Расчёт передаваемого индекса комфорта
  transmitter5.setPressure(785.0); 
  transmitter5.buffer_size = 23;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
    //передача осуществляется по таймеру, который определяется типом датчика и номером канала
    
    if (transmitter.transmit()) PrintSentData(transmitter.SendBuffer, transmitter.buffer_size);
    if (transmitter2.transmit()) PrintSentData(transmitter2.SendBuffer, transmitter2.buffer_size);
    if (transmitter3.transmit()) PrintSentData(transmitter3.SendBuffer, transmitter3.buffer_size);
    if (transmitter4.transmit()) PrintSentData(transmitter4.SendBuffer, transmitter4.buffer_size);
    if (transmitter5.transmit()) PrintSentData(transmitter5.SendBuffer, transmitter5.buffer_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void PrintSentData(byte* buf, int buf_size)
{
      Serial.print(millis() / 1000);
      Serial.print("s \t\t");
      for (byte i = 0; i < buf_size; i++)   
      {
        byte trmbuf = *buf;
        Serial.print(trmbuf >> 4, HEX);
        i++;
        if (i >= buf_size) break;
        Serial.print(trmbuf & 0x0F, HEX);
        buf++;
      }  
      Serial.println();
      delay(1000);
}
