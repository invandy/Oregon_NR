//Arduino library for receiving data from Oregon Scientific v2 wireless temperature and humidity sensors.
//Supported sensors: THGN132N, THN132N and similar.
//Most suitable for EM noisy environmental and weak signal conditions. 
//Release date: 04 Mar 2017. 
//Compatable with Atmega and ESP8266-based boards.

#include <Arduino.h>

#ifndef Oregon_NR_h
#define Oregon_NR_h

#define READ_BITS 96              
#define READ_BITS2 192            
#define PACKET_LENGTH 20          

#define THGN132 0x1D20            
#define THN132  0xEC40
#define GAS     0xAA00
#define FIRE    0xBB00

#define FIND_PACKET   1           
#define ANALIZE_PACKETS 2

#define PER_LENGTH 976            
#define THR_LENGTH 615            
#define LENGTH_TOLERANCE 20       

static int RECEIVER_PIN;

class Oregon_NR
{
  public:

    //Данные датчика
    word sens_type;               //Sensor type
    float sens_tmp,               //Temperature
    sens_tmp2;                    //Temperature2  (for GASv2 sensor)
    byte sens_CO,                 //CO (ppm*10) (for GASv2 sensor)
    sens_CH,                      //CH4 (ppm*100)(ppm)
    sens_hmdty,                   //Humidity
    sens_chnl,                    //Channel number
    sens_id,                      //ID
    sens_battery;                 //Battery status
    byte sens_ip22,               //IP22 channel data (for FIRE sensor)
    sens_ip72,                    //IP72 channel data (for FIRE sensor)         
    sens_lockalarm;               //LOCK_ALARM channel data (for FIRE sensor)
    bool crc_c = 0;               //CRC check result. Сбрасывается при захвате. Выставляется при приёме корректного пакета.
    bool captured = 0;            //Capture data flag. Выставляется, если были считанны данные в память.
    bool maybe_packet = 0;        //Флаг, что возможно был пакет, но мы его потеряли...
    unsigned long work_time;      //Capture time
    byte packet[PACKET_LENGTH];   //Result packet
    byte valid_p[PACKET_LENGTH];  //Validity mask
    byte packets_received = 0;    //Number of received packets in block (0...2)
    byte received_CRC;                     //Calculated СRC 
    
    Oregon_NR(int, int);          //Конструктор. Параметры:
    Oregon_NR(int, int, byte, bool);    //(вывод приёмника, номер прерывания, вывод светодиода, pull up)
    void start();                 //Star listening receiver
    void stop();                  //Stop listening receiver. Чтобы не занимал процессор, когда не нужен
    void capture(bool);           //Capture packet. if parameter is true function dumps capture data to Serial.

    bool consist_synchro = false; //При поиске синхронибла опираться подтверждённые или сомнительные данные?
    
    byte empty_space = 5;         //Какое количество "пустых" тактов нужно для определения конца посылки?
                                  //Параметр определяется уровнем сигнала и скоростью АРУ приёмника.
                                  //Чем они лучше, тем меньше число. НО меньше двух не рекомендуется
                                  
    bool is_assemble = true;      //Assemble data from both packets in block or decode it separately.
                                  
    
  private:

    
    byte read_tacts, read_tacts2, result_size;
    byte LED = 0xFF;            //вывод светодиода, который мигает при приёме
    bool PULL_UP;               //куда подключён светодиод
    byte packet_number = 0;     //Количесвто принятых пакетов в посылке  
    int INT_NO;                 //Номер прерывания приёмника
    //bool  reciever_ctrl = true; //Флаг контроля ресивера (выставляется при приходе импулься, сбрасывается в таймере)

    //Массивы данных для записи данных с канала и полученных битов
    byte decode_tacts[READ_BITS2]; //Массив тактов. значения
    //                          0=ноль
    //                          1=единица
    //                          2=неизвестен
    //                          3=переход+
    //                          4=переход-
    byte collect_data[READ_BITS2], collect_data2[READ_BITS2];
    //А когда становится массивом полученных битов, то значения такие:
    //                          128 - неизвестен
    //                          >128 - единица
    //                          <128 - ноль

    byte receive_status = FIND_PACKET;
    byte start_pulse_cnt = 0;
    unsigned long pulse_length, timer_marklong;  
    unsigned long pulse_marker, right_pulse_marker, last_premarker;
    unsigned long pre_marker[4]; // Для хранения временных меток преамбулы ()
    unsigned long first_packet_end;
    int data_val, data_val2;        // Качество пакетов
    byte synchro_pos, synchro_pos2; // Позиции синхрониблов в записи
    
    byte get_gas_CH(byte* gas_data);
    byte get_gas_CO(byte* gas_data);
    byte get_gas_hmdty(byte* gas_data);
    float get_gas_temperature_in(byte* gas_data);
    float get_gas_temperature_out(byte* gas_data);
    byte get_gas_channel(byte* gas_data);
    void restore_data(byte* oregon_data, word sensor_type);
    bool check_CRC(byte* oregon_data, word sensor_type);
    byte get_id(byte* oregon_data);
    byte get_humidity(byte* oregon_data);
    byte get_battery(byte* oregon_data);
    byte get_channel(byte* oregon_data);
    word get_sensor(byte* oregon_data);
    float get_temperature(byte* oregon_data);
    byte get_info_data(byte* code, byte* result, byte* valid);
    void assemble_data(byte* s1, byte* s2, int shift);
    int correlate_data(byte* ser1, byte* ser2);
    byte collect(byte* cdptr);
    int get_data(int btt, byte* cdptr);
    void get_bits(byte* cdptr);
    byte get_synchro_pos(byte* code);
    void led_light(bool);
    byte get_fire_ip22(byte* fire_data);
    byte get_fire_ip72(byte* fire_data);
    byte get_fire_lockalarm(byte* fire_data);

};

#endif

