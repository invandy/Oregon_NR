#include <Arduino.h>
#ifndef Oregon_NR_h
#define Oregon_NR_h

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
// UVN800,
// PCR800.
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
// UVN800,
// PCR800.
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
//

// ������������� ������� �� ��������� �������� Oregon Scientific:
//
#define THGN132   0x1D20 // �����������, ���������, 3 ������,
#define THN132    0xEC40 // �����������,  3 ������,
#define RTGN318   0xDCC3 // �����������, ���������, 5 �������,
#define THGR810   0xF824 // �����������, ���������, 10 �������,
#define WGR800	  0x1984 // ����������� � �������� �����
#define UVN800	  0xD874 // ��-������, ������������ (������� XOR �� ��������������� ������)
#define PCR800    0x2914 // Rain gauge
//
// ������� ����������� ����������:
#define THP	  0x5500 // �����������, ���������, ��� ��������, 8 �������, ������ �� 3-�� ������� ��,
#define GAS       0xAA00 // �����������, ���������, ������������ �� � CH4, 8 �������,
#define FIRE      0xBB00 // �������� �������� �������� ����� �������� ��22 � ��72
#define CURRENT   0xEE00 // ���, ����������, 8 �������,
#define CAPRAIN   0x8800 // ��������� ����������� ������ �������
//
//����� ����������� ����� �������� ��� �������� ��������
#define ADD_SENS_SUPPORT 1      // �������� �������������� ����� �������� ����������� ���������� - ���������� ������������� �������� ���
#define IS_ASSEMBLE 1           // �������� �� ������� �� ���� ����������� ������� ���� ����� (��� v2) - ���������� ������ �������� ���!
//
// ����� ����������� ����� �������� ��� ��������� ���������� �����

#define MAX_LENGTH2 976		 // ������������ ����� �������� ��������� v2 - �� ������ �������, ����� ������� ������ �� ������ ����������
#define MIN_LENGTH2 883          // ����������� ����� �������� ��� ������� (��� v2 �������� ��������� �� 93���), �.�. ������ ���� �� ������ 883���,
				
#define MAX_LENGTH3 488         // ������������ ����� �������� ��������� v3 - �� ������ �����������, ����� ������� ������ �� ������ ����������
#define MIN_LENGTH3 330         // ����������� ����� �������� ��� ������� (��� v2 �������� ��������� �� 138���), �.�. ������ ���� �� ������ 350���,

#define LENGTH_TOLERANCE 64     // ���������� ���������� ��� ����� ��������. ������� �� ��������
                                // � ����������� �� ������ ������� ����� "������� ����� ���������"

#define CATCH_PULSES 3		// ������� ������ ���������� ��������� ��� ������ �������. ������������� 2 - 4. 
                                // ������ - ����� �� ������� ����� � �������� ������� �����
                                // ������ - ����� ���������� �����, ������ ���������� �� ������ �����

//��� ��������� ������� �� ����!

#define FIND_PACKET   1           
#define ANALIZE_PACKETS 2
#define PER_LENGTH2 976		// ������ �������� ������. ��� v2 � v3 976,56��� (1024��)
#define PER_LENGTH3 488

#define READ_BITS 105  		// ������������ ����� ������ � ������
#define READ_BITS2 210		// ������������ ����� ������ � ������ * 2
#define PACKET_LENGTH 20        // ����� ������ � ������ ��� ����� ��������� (� ������������) 

static int RECEIVER_PIN;

class Oregon_NR
{
  public:

    //������ �������
    word sens_type;               //Sensor type


    float sens_tmp,               //Temperature
    sens_hmdty;                   //Humidity

    byte sens_chnl,               //Channel number
    sens_id,                      //ID
    sens_battery;                 //Battery status

    byte ver = 0;                 //Protocol version

    bool crc_c = 0;               //CRC check result. ������������ ��� �������. ������������ ��� ����� ����������� ������.
    bool captured = 0;            //Capture data flag. ������������, ���� ���� ������� ������ � ������.

    unsigned long work_time;      //Capture time
    byte packet[PACKET_LENGTH];   //Result packet
    byte valid_p[PACKET_LENGTH];  //Validity mask - ����� ���������� ������������� �����
    byte packets_received = 0;    //Number of received packets in block (0...2)
    byte received_CRC;            //Calculated �RC 
    
    Oregon_NR(byte, byte);          //�����������. ���������:
    Oregon_NR(byte, byte, byte, bool);    //(����� ��������, ����� ����������, ����� ����������, pull up)
    void start();                 //Star listening receiver
    void stop();                  //Stop listening receiver. ����� �� ������� ���������, ����� �� �����
    void capture(bool);           //Capture packet. if parameter is true function dumps capture data to Serial.

    bool consist_synchro = false; //��� ������ ����������� ��������� ������������� ��� ������������ ������?
    
    byte empty_space = 3;         //����� ���������� "������" ������ ����� ��� ����������� ����� �������?
                                  //�������� ������������ ������� ������� � ��������� ��� ��������.
                                  //��� ��� �����, ��� ������ �����. �� ������ ���� �� �������������
				  //� ������ ������ ���� 5
    bool catch2 = 1, catch3 = 1;  //����� ������ ��������� ���������
    int timing_correction = 0;   //��������� ������� ����� (�� -10 �� +10)
    byte decode_method = 3;      // ����� ������������� ������
                                 //1 - ������������
                                 //3 - ��� �������� ������� 

    //���������
    float sens_avg_ws, sens_max_ws;
    byte  sens_wdir;
    float get_avg_windspeed(byte*);
    float get_max_windspeed(byte*);
    byte get_winddirection(byte*);
    //UV
    byte UV_index, lightness;
    byte get_UV(byte*);
    byte get_light(byte*);
    //Rain
    float sens_total_rain, sens_rain_rate;
    float get_total_rain(byte*);
    float get_rain_rate(byte*);

    byte restore_sign;            //������� ����, ������������ �� �������� �������� ����������� ������

                                  //0 - ������������� ��������� �����
                                  //1 - ������������� ������� �����
                                  //2 - ����������� ������ ��������� ��� �������� ������
                                  //3 - ������������� ������� ���������� (v2) - ��������� ��� �������� ��������

    bool receiver_dump = 0;       //���������� �� ���� ������ � Serial. �������� ������ ���� capture(true)
                                  // ���������� ��� ������������� ��������� ������� � ��������
                                  // ����� ��������� �������� ������������������ �� � ����� ��������������

#if ADD_SENS_SUPPORT == 1
    float sens_pressure,          //Pressure
    sens_voltage,                 // ���������� � � (for CURRENT � THP sensors)
    sens_tmp2;                    //Temperature2  (for GASv2 sensor)
    byte sens_CO,                 //CO (ppm*10) (for GASv2 sensor)
    sens_CH;                      //CH4 (ppm*100)(ppm)
    byte sens_ip22,               //IP22 channel data (for FIRE sensor)
    sens_ip72,                    //IP72 channel data (for FIRE sensor)         
    sens_lockalarm;               //LOCK_ALARM channel data (for FIRE sensor)
    float sens_current;           // ��� � � (for CURRENT sensor)
    
    word  sens_pump_count;        // ������� ������
    unsigned long sens_drop_counter;// ������� ������ (for CAPRAIN sensor)
    int sens_capacitance;         //������� ������� (for CAPRAIN sensor)
#endif                                  


  private:

    
    byte read_tacts, read_tacts2, result_size;
    byte LED = 0xFF;            //����� ����������, ������� ������ ��� �����
    bool PULL_UP;               //���� ��������� ���������
    byte packet_number = 0;     //���������� �������� ������� � �������  
    int INT_NO;                 //����� ���������� ��������
    //bool  reciever_ctrl = true; //���� �������� �������� (������������ ��� ������� ��������, ������������ � �������)

    //������� ������ ��� ������ ������ � ������ � ���������� �����
    byte decode_tacts[READ_BITS2]; //������ ������. ��������
    //                          0=����
    //                          1=�������
    //                          2=����������
    //                          3=�������+
    //                          4=�������-

    byte collect_data[READ_BITS2], //������ ��� ����� ������ � ��������
#if IS_ASSEMBLE
    collect_data2[READ_BITS2];
#else
    collect_data2[1];
#endif
    //� ����� ���������� �������� ���������� �����, �� �������� �����:
    //                          128 - ����������
    //                          >128 - �������
    //                          <128 - ����

    byte receive_status = FIND_PACKET;
    byte start_pulse_cnt = 0;
    unsigned long pulse_length, timer_marklong;  
    unsigned long pulse_marker, right_pulse_marker;
    unsigned long pre_marker; // ��� �������� ��������� ����� ��������� ��� ������� ������
    unsigned long first_packet_end;
    byte data_val, data_val2;        // �������� �������
    byte synchro_pos, synchro_pos2; // ������� ������������ � ������
    
    byte get_gas_CH(byte* gas_data);
    byte get_gas_CO(byte* gas_data);
    byte get_gas_hmdty(byte* gas_data);
    float get_gas_temperature_in(byte* gas_data);
    float get_gas_temperature_out(byte* gas_data);
    byte get_gas_channel(byte* gas_data);
    void restore_data(byte* oregon_data, word sensor_type);
    bool check_CRC(byte* oregon_data, word sensor_type);
    byte get_id(byte* oregon_data);
    float get_humidity(byte* oregon_data);
    byte get_battery(byte* oregon_data);
    byte get_channel(byte* oregon_data);
    word get_sensor(byte* oregon_data);
    float get_temperature(byte* oregon_data);
    int get_info_data(byte* code, byte* result, byte* valid);
    void assemble_data(byte* s1, byte* s2, int shift);
    int correlate_data(byte* ser1, byte* ser2);
    int collect(byte* cdptr);
    int get_data(int btt, byte p_ver, byte* cdptr);
    void get_tacts(byte*, byte);
    int get_synchro_pos(byte* code);
    void led_light(bool);

#if ADD_SENS_SUPPORT == 1
    byte get_fire_ip22(byte* fire_data);
    byte get_fire_ip72(byte* fire_data);
    byte get_fire_lockalarm(byte* fire_data);
    float get_current(byte* curent_data);
    float get_voltage(byte* voltage_data);
    word  get_pump_count(byte* voltage_data);
    unsigned long get_dropcounter(byte* packetdata);
    int get_capacitance(byte* packetdata);
    float get_thp_humidity(byte* oregon_data);
    float get_thp_temperature(byte* oregon_data);
    float get_thp_pressure(byte* oregon_data);
    float get_thp_voltage(byte* oregon_data);
#endif    

};

#endif

