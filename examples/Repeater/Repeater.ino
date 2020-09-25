#include <Oregon_NR.h>
#include <Oregon_TM.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// В данном примере описан ретранслятор пакетов 
// Может быть полезен для увеличения дальности приёма сигналов от некоторых датчиков
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LED 13
Oregon_NR oregon(2, 0);   // Приёмник на D2, прерывание 0
Oregon_TM transmitter(4); // Передатчик на D4

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void setup() {
   Serial.begin(115200);
   oregon.start();        // Включаем приёмник
   pinMode(13, OUTPUT);
   digitalWrite(13,LOW);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  
  oregon.capture(0);
  if (oregon.captured)  
  {
    transmitter.buffer_size = 0;
    for (int q = 0; q < PACKET_LENGTH; q++) 
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
     || (oregon.packet[0] == 0x01 && oregon.packet[1] == 0x0D))
    
    {
      Serial.println(" -> ");
      delay(500); 
        
      //Подготавливаем буфер передачи
      for ( int q = 0;q < OREGON_SEND_BUFFER_SIZE; q++)
      {
        transmitter.SendBuffer[q] = oregon.packet[q*2+1] + oregon.packet[q*2]*16;
      }
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
