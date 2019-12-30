#include <Arduino.h>
#ifndef Oregon_TM_h
#define Oregon_TM_h

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This Arduino code is for receive and transmit data using Oregon Scientific RF protocol version 2.1 and 3.0. 
//
// Last updated: 14 October 2019
//
// The folowed sensors data format are supported.
//
// Receive and transmit:
// THGN132N (THGR122N, THGN123N),
// RTGN318,
// THGR810.

// Receive only:
// THN132N,
// WGR800,
// UVN800.
//
// Also supported self-developed sensors. Please contact author for additional information.
//
// This file is part of the Arduino OREGON_NR library.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// The MIT License (MIT)
//
// Copyright (c) 2019 Sergey Zawislak 
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
//
// ������ ���������� ������� ������������� ��� ������ � �������� ������ � ������� ������������� ��������� Oregon Scientific v2.1 � v3.0
//
// ��������� ���������� 14 ������� 2019
//
// �������������� ������ ��������� ��������
//
// ���� � ��������:
// THGN132N (THGR122N, THGN123N),
// RTGN318,
// THGR810.

// ������ ����:
// THN132N,
// WGR800,	
// UVN800.
//
// ����� �������������� ������� ����������� ���������� (�� �������������� ������������ ���������� � ������)
//
//  ���� ���� - ����� ���������� OREGON_NR
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2019 ������ ��������
//
// ������ �������� ��������� �����, ���������� ����� ������� ������������ ����������� � ������������� ������������ 
// (� ���������� ���������� ������������ �����������), ������������ ������������ ����������� ����������� ��� �����������,
// ������� �������������� ����� �� �������������, �����������, ���������, �������, ����������, ���������������, �����������������
// �/��� ������� ����� ������������ �����������, � ����� �����, ������� ��������������� ������ ����������� �����������, ��� ���������� ��������� �������:
//
// ��������� ���� ����������� �� ��������� ����� � ������ ������� ������ ���� �������� �� ��� ����� ��� �������� ����� ������� ������������ �����������.
//
// ������ ����������� ����������� ��������������� ���� ���ܻ, ��� �����-���� ��������, ���� ���������� ��� ���������������, ������� �������� �������� 
// �����������, ������������ �� ��� ����������� ���������� � ���������� ���������, �� �� ������������� ���. �� � ����� ������ ������ ��� ��������������� 
// �� ����� ��������������� �� �����-���� �����, �� ����� ��� �� ���� �����������, � ��� �����, ��� �������� ���������, ������� ��� ���� ��������, 
// ��������� ��-�� ������������� ������������ ����������� ��� ���� �������� � ����������� ������������. 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define TR_TIME 488
#define TWOTR_TIME 976
#define PULSE_SHORTEN_2   80
#define PULSE_SHORTEN_3   120

#define THGN132   0x1D20 
#define THGR810   0xF824
#define RTGN318   0xDCC3 
#define THP	  0x5500

static byte TX_PIN = 4;
static const int buffersize = 10;

class Oregon_TM
{
  public:

    byte SendBuffer[buffersize];
    byte protocol = 2;
    word sens_type = 0x0000;               
    int timing_corrector2 = 4;
    int timing_corrector3 = 2;

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


    void calculateAndSetChecksumTHP();

    unsigned long time_marker = 0;
    unsigned long time_marker_send = 0;
    unsigned long send_time = 0;
    bool prevbit = 1;
    bool prevstate = 1;


};

#endif

