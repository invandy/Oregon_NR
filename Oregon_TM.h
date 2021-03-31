#include <Arduino.h>
#ifndef Oregon_TM_h
#define Oregon_TM_h
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This file is part of the Arduino OREGON_NR library.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The MIT License (MIT)
//
// Copyright (c) 2021 Sergey Zawislak 
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
// Copyright (c) 2021 Сергей Зависляк
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

#define TR_TIME 488
#define TWOTR_TIME 976
#define PULSE_SHORTEN_2   93
#define PULSE_SHORTEN_3   138

#define THGN132   0x1D20 
#define THN132   0xEC40
#define THGR810   0xF824
#define RTGN318   0xDCC3 
#define THP	  0x5500
#define BTHGN129  0x5D53 
#define BTHR968   0x5D60 


#define OREGON_SEND_BUFFER_SIZE 12

static byte TX_PIN = 4;


class Oregon_TM
{
  public:

    int max_buffer_size = OREGON_SEND_BUFFER_SIZE;
    int  buffer_size = 24;
    byte* SendBuffer;
    byte protocol = 2;
    word sens_type = 0x0000;               
    int timing_corrector2 = 4;
    int timing_corrector3 = 2;

    Oregon_TM(byte, int); 
    Oregon_TM(byte); 
    Oregon_TM(); 
    void setType(word);
    void setChannel( byte);
    void setId(byte);
    void setBatteryFlag(bool);
    void setStartCount(byte);
    void setTemperature(float);
    void setHumidity(byte);
    void setComfort(float, byte);
    void setPressure(float);
    bool transmit();
    void SendPacket();

    void setErrorTHP();
    void setPressureTHP(float);
    void setTemperatureTHP(float);
    void setBatteryTHP(word);
    void setChannelTHP(byte);
    void setHumidityTHP(float);



  private:
    
    void sendZero(void);
    void sendOne(void);
    void sendMSB(const byte);
    void sendLSB(const byte);
    void sendData();
    void sendOregon();
    void sendPreamble();
    void calculateAndSetChecksum132();
    void calculateAndSetChecksum318();
    void calculateAndSetChecksum810();
    void calculateAndSetChecksum968();
    void calculateAndSetChecksum129();
	void calculateAndSetChecksum132S();									


    void calculateAndSetChecksumTHP();

    unsigned long time_marker = 0;
    unsigned long time_marker_send = 0;
    unsigned long send_time = 0;
    bool prevbit = 1;
    bool prevstate = 1;


};

#endif

