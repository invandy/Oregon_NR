#include <Oregon_NR.h>
#include <Oregon_TM.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// В данном примере описан ретранслятор пакетов 
// Может быть полезен для увеличения дальности приёма сигналов от некоторых датчиков
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LED 13             //вывод светодиода
#define MAX_SEND_BUFFER 30 //максимальный размер буфера передачи в ниблах
Oregon_NR oregon(2, 0, 255, true, MAX_SEND_BUFFER, true);   // Приёмник на D2, прерывание 0, светодиод не нужен, буфера на 30 ниблов, сборка пакетов включена
Oregon_TM transmitter(4, MAX_SEND_BUFFER);   // Передатчик на D4, буфер на 30 ниблов

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void(* resetFunc) (void) = 0;

void setup() {
   Serial.begin(115200);
   oregon.start();        // Включаем приёмник
   pinMode(LED, OUTPUT);
   digitalWrite(LED,LOW);
   Serial.println("START");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  oregon.capture(0);
  if (oregon.captured)  
  {
   if (micros() > 3600000000) resetFunc();
    transmitter.buffer_size = 0;
    for (int q = 0; q < oregon.packet_length; q++) 
    {
      if (oregon.valid_p[q] != 0x00)
      {
        transmitter.buffer_size++;
        Serial.print(oregon.packet[q], HEX);
      }
      else break;
    }
    //Проверяем, нужно ли этот пакет ретранслировать
    if ((oregon.packet[0] == 0x0E && oregon.packet[1] == 0x0C)
    //Например, нам нужно ретранслировать пакет, если первые два нибла пакета 19h (датчик WGR800)
     || (oregon.packet[0] == 0x01 && oregon.packet[1] == 0x09)
     || (oregon.packet[0] == 0x05 && oregon.packet[1] == 0x05)
     || (oregon.packet[0] == 0x01 && oregon.packet[1] == 0x0D))
    
    {
      Serial.println("\t -> ");
      delay(500); 

        
      //Подготавливаем буфер передачи
      //Добавляем в хвост флаг, что это ретранслируемый пакет
      oregon.packet[transmitter.buffer_size] = 0x00;
      transmitter.buffer_size ++;
      oregon.packet[transmitter.buffer_size] = 0x00;
      transmitter.buffer_size ++;
      //переписываем буфер в передатчик
      if (transmitter.buffer_size > MAX_SEND_BUFFER) transmitter.buffer_size = MAX_SEND_BUFFER;
      //Переписываем буфер из приёмника в передатчик
      for ( int q = 0; q < transmitter.max_buffer_size; q++)
        transmitter.SendBuffer[q] = oregon.packet[q*2+1] + oregon.packet[q*2]*16;        

      //Будем передавать в том же протоколе, что и приняли
      transmitter.protocol = oregon.ver;
        
      //Передаём данные
      digitalWrite(13,HIGH);
      transmitter.SendPacket();
      digitalWrite(13,LOW);
    }
    else Serial.println(' ');
  }
}

