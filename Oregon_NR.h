#include <Arduino.h>
#ifndef Oregon_NR_h
#define Oregon_NR_h

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
//

// Распознавание пакетов от следующих датчиков Oregon Scientific:
//
#define THGN132   0x1D20 // Температура, влажность, 3 канала,
#define THGN500   0x1D30 // Температура, влажность, 
#define THN132    0xEC40 // Температура,  3 канала,
#define RTGN318   0x0CC3 // Температура, влажность, время, 5 каналов,
#define RTHN318   0x0CD3 // Температура, время, 5 каналов,
#define RFCLOCK   0x0CF3 //  
#define BTHGN129  0x5D53 // Температура, влажность, давление, 5 каналов,
#define BTHR968   0x5D60 // Температура, влажность, давление,
#define THGR810   0xF824 // Температура, влажность, 10 каналов,
#define THN800	  0xC844 // Температура,  3 канала,
#define WGR800	  0x1984 // Направление и скорость ветра
#define UVN800	  0xD874 // УФ-индекс, освещённость (спасибо XOR за предоставленные данные). 
#define PCR800	  0x2914 // счётчик осадков
#define PCR800	  0x2914 // счётчик осадков

// Распознавание пакетов от следующих датчиков Explore Scientific:
#define ST1004	  0x00B5 // Температура, влажность, 3 канала, 
#define ST1005	  0x00B6 // Температура, влажность, 3 канала, 

//
// Датчики собственной разработки:
#define THP	  0x5500 // Температура, влажность, атм давление, 8 каналов, работа от 3-ех батарей АА, (документация прилагается)
#define GAS       0xAA00 // Температура, влажность, концентрация СО и CH4, 8 каналов,
#define FIRE      0xBB00 // Контроль сигналов пожарных линий датчиков ИП212-22 и ИП212-72
#define CURRENT   0xEE00 // Ток, напряжение, 8 каналов,
#define CAPRAIN   0x8800 // Емкостной всепогодный датчик осадков
#define UVS	  0x7700 // Датчик УФ
//
//Этими параметрами можно поиграть для экономии ресурсов
#define ADD_SENS_SUPPORT 1      // Поддежка дополнительных типов датчиков собственной разработки - отключение незначительно экономит ОЗУ

//
// Этими параметрами можно поиграть для настройки наилучшего приёма


#define PER_LENGTH2      976	// Период передачи данных. Для v2 и v3 976,56мкс (1024Гц)
#define PER_LENGTH3      488
#define PER_LENGTH_ES2   680

#define LENGTH_TOLERANCE 100   // Свыше разбросов длины импульсов нужно ещё около 60-90мс для запаса по уровню сигнала


#define CATCH_PULSES 3		// Сколько искать правильных импульсов для начала захвата. Рекомендовано 2 - 4. 
// Больше - можно не поймать пакет в условиях сильных шумов
// Меньше - можно пропустить пакет, сильно отвлекаясь на анализ шумов
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Эти параметры трогать не надо!

#define IS_ASSEMBLE 1           // Пытаться ли собрать из двух повреждённых пакетов один целый (для v2) - отключение сильно экономит ОЗУ!


#define PACKET_LENGTH 26        // Длина пакета в ниблах без учёта преамбулы и синхронибла,
// максимальная у PCR800 - 22 нибла
//ДЛя захвата более длинных пакетов можно увеличить


#define READ_BITS ((PACKET_LENGTH + 8) * 4)

// Максимальная длина пакета в битах
// преамбула для v2 - 5 ниблов(FFFFA), самый длинный пакет - 19ниблов (THGN132)
//итого (19 + 5) * 4 = 96 бит
//преамбула для v3 - 7 ниблов(FFFFFFA), самый длинный пакет - 22нибла PCR900
//итого (22 + 7) * 4 = 116 бит

#define READ_TACTS ((PACKET_LENGTH + 6) * 8)
// Максимальная длина пакета в такта
// v2 - 96 * 2 = 192
// v3 - 116 = 116


#define FIND_PACKET   1
#define ANALIZE_PACKETS 2


static int RECEIVER_PIN;

class Oregon_NR
{
  public:

    int packet_length = PACKET_LENGTH;
    int no_read_bits = (PACKET_LENGTH + 8) * 4;
    int no_read_tacts = (PACKET_LENGTH + 6) * 8;
    bool is_assemble = IS_ASSEMBLE;
    bool no_memory = false;

    //Данные датчика
    word sens_type;               //Sensor type


    float sens_tmp,               //Temperature
          sens_hmdty;                   //Humidity

    byte sens_chnl,               //Channel number
         sens_id,                      //ID
         sens_battery;                 //Battery status

    byte ver = 0;                 //Protocol version

    bool crc_c = 0;               //CRC check result. Сбрасывается при захвате. Выставляется при приёме корректного пакета.
    bool captured = 0;            //Capture data flag. Выставляется, если были считаны данные в память.

    unsigned long work_time;      //Capture time
    byte* packet;   //Result packet
    byte* valid_p;  //Validity mask - маска уверенного распознавания битов
    byte packets_received = 0;    //Number of received packets in block (0...2)
    byte received_CRC;            //Calculated СRC
    byte es2_bit[3];

    Oregon_NR(byte, byte);          //Конструктор. Параметры:
    Oregon_NR(byte, byte, byte, bool);    //(вывод приёмника, номер прерывания, вывод светодиода, pull up)
    Oregon_NR(byte, byte, byte, bool, int, bool);    //(вывод приёмника, номер прерывания, вывод светодиода, pull up, размер буфера)
    void start();                 //Star listening receiver
    void stop();                  //Stop listening receiver. Чтобы не занимал процессор, когда не нужен
    void capture(bool);           //Capture packet. if parameter is true function dumps capture data to Serial.

    bool consist_synchro = false; //При поиске синхронибла опираться подтверждённые или сомнительные данные?

    byte empty_space = 3;         //Какое количество "пустых" тактов нужно для определения конца посылки?
    //Параметр определяется уровнем сигнала и скоростью АРУ приёмника.
    //Чем они лучше, тем меньше число. НО меньше двух не рекомендуется
    //В сатрой версии было 5
    bool catch2 = 1, catch3 = 1, catch_es = 1, catch_es2 = 1;  //какие версии протокола принимать

    int timing_correction = 0;   //Коррекция частоты приёма (от -10 до +10)
    byte decode_method = 3;      // Метод декодирования тактов
    //1 - традиционный
    //3 - при девиации частоты

    //Ветрометр
    float sens_avg_ws, sens_max_ws;
    byte  sens_wdir;
    float get_avg_windspeed(byte*);
    float get_max_windspeed(byte*);
    byte get_winddirection(byte*);
    //UV
    byte UV_index, lightness;
    byte get_UV(byte*);
    byte get_light(byte*);
    //счётчик осадков
    float get_total_rain();
    float get_rain_rate();
    //Барометр
    float get_pressure();

    byte restore_sign;            //Битовое поле, инормирующее об успешных способах реставрации пакета

    //0 - восстановлены одиночные такты
    //1 - восстановлены двойные такты
    //2 - исправление версии протокола при разборке пакета
    //3 - восстановлено методом сращивания (v2) - отключено для экономии ресурсов

    bool receiver_dump = 1;       //Сбрасывать ли дамп канала в Serial. работает тольок если capture(true)
    // фактически это осциллограмма огибающей сигнала с приёмника
    // Также выводятся тактовые последовательности до и после восстановления
    unsigned long pulse1, pulse2, pulse3;

#if ADD_SENS_SUPPORT == 1
    float sens_pressure,          //Pressure
          sens_voltage,                 // напряжение в В (for CURRENT и THP sensors)
          sens_tmp2;                    //Temperature2  (for GASv2 sensor)
    byte sens_CO,                 //CO (ppm*10) (for GASv2 sensor)
         sens_CH;                      //CH4 (ppm*100)(ppm)
    byte sens_ip22,               //IP22 channel data (for FIRE sensor)
         sens_ip72,                    //IP72 channel data (for FIRE sensor)
         sens_lockalarm;               //LOCK_ALARM channel data (for FIRE sensor)
    float sens_current;           // ток в А (for CURRENT sensor)
    float sens_uv;                // УФ-индекс

    word  sens_pump_count;        // счётчик насоса
    unsigned long sens_drop_counter;// счётчик капель (for CAPRAIN sensor)
    int sens_capacitance;         //Емкость сенсора (for CAPRAIN sensor)
#endif
    bool check_oregon_crcsum(byte*, byte , byte , byte, bool );

  private:


    int read_tacts, read_tacts2, result_size;
    byte LED = 0xFF;            //вывод светодиода, который мигает при приёме
    bool PULL_UP;               //куда подключён светодиод
    byte packet_number = 0;     //Количесвто принятых пакетов в посылке
    int INT_NO;                 //Номер прерывания приёмника
    //bool  reciever_ctrl = true; //Флаг контроля ресивера (выставляется при приходе импулься, сбрасывается в таймере)

    //Массивы данных для записи данных с канала и полученных битов
    byte* decode_tacts;        //Массив тактов. значения
    //                          0=ноль
    //                          1=единица
    //                          2=неизвестен
    //                          3=переход+
    //                          4=переход-

    byte* collect_data;         //Память для сбора данных с приёмника
#if IS_ASSEMBLE
    byte* collect_data2;
#else
    byte* collect_data2;
#endif
    //А когда становится массивом полученных битов, то значения такие:
    //                          128 - неизвестен
    //                          >128 - единица
    //                          <128 - ноль

    byte receive_status = FIND_PACKET;
    byte start_pulse_cnt = 0;
    byte pulse_cnt_2 = 0;
    byte pulse_cnt_3 = 0;
    byte pulse_cnt_es = 0;
    byte pulse_cnt_es2 = 0;

    unsigned long pulse_length, timer_marklong;
    unsigned long pulse_marker, right_pulse_marker;
    unsigned long pre_marker = 0, pre_marker_2 = 0, pre_marker_3 = 0, pre_marker_es = 0, pre_marker_es2 = 0; // Для хранения временных меток преамбулы при захвате пакета
    unsigned long first_packet_end;
    int data_val, data_val2;        // Качество пакетов
    int synchro_pos, synchro_pos2; // Позиции синхрониблов в записи

    byte get_gas_CH(byte* gas_data);
    byte get_gas_CO(byte* gas_data);
    byte get_gas_hmdty(byte* gas_data);
    float get_gas_temperature_in(byte* gas_data);
    float get_gas_temperature_out(byte* gas_data);
    byte get_gas_channel(byte* gas_data);
    void restore_data(byte* oregon_data, word sensor_type);
    bool check_CRC(byte*, word, byte*);
    bool check_own_crcsum(byte* , byte);
    byte get_id(byte* oregon_data);
    float get_humidity(byte* oregon_data);
    byte get_battery(byte* oregon_data);
    byte get_channel(byte* oregon_data);
    word get_sensor(byte* oregon_data);
    float get_temperature(byte* oregon_data);
    int get_info_data(byte*, byte , byte* , byte* );
    void assemble_data(byte* s1, byte* s2, int shift);
    int correlate_data(byte* ser1, byte* ser2);
    int collect(byte* cdptr);
    int get_data(int btt, byte p_ver, byte* cdptr, int);
    void get_tacts(byte*, int);
    int get_synchro_pos(byte* code, int);
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
    float get_uvs(byte* oregon_data);
#endif

};

#endif
