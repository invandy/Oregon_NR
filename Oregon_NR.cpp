//6 июня 2018 - Изменены параметры захвата пакета. Принимать стало заметно лучше
#include "Oregon_NR.h"

//Всё, что относится к прерыванию/////////////////////////////////////
#ifndef Oregon_NR_int
#define Oregon_NR_int
static volatile unsigned long pm;
static volatile unsigned long pl, timer_mark;  
void receiver_interruption(void) {  
  if(digitalRead(RECEIVER_PIN)){
  //Начало импульса
    pl = 0;
    pm = micros();  
  }
  else{
    //Конец импульса
    //Вычисляется время окончания и длина
    pl = micros() - pm;  
    pm += pl;
  }
  //yield();
}  
#endif

//////////////////////////////////////////////////////////////////////
Oregon_NR::Oregon_NR(int MHZ, int MHZ_INT)
{
  INT_NO = MHZ_INT;
  RECEIVER_PIN = MHZ;
  pinMode(MHZ, INPUT); // Вывод, на который подключён приёмник
}

Oregon_NR::Oregon_NR(int MHZ, int MHZ_INT, byte led, bool pull_up)
{
  INT_NO = MHZ_INT;
  LED = led;
  PULL_UP = pull_up;
  RECEIVER_PIN = MHZ;
  pinMode(MHZ, INPUT);    // Вывод, на который подключён приёмник
  pinMode(LED, OUTPUT);   // Вывод светодиода
}

//////////////////////////////////////////////////////////////////////
void Oregon_NR::start()
{
  packet_number = 0;
  packets_received = 0;
  start_pulse_cnt = 0;
  receive_status = FIND_PACKET;
  led_light(false);
  attachInterrupt(INT_NO, receiver_interruption, CHANGE);  
}
//////////////////////////////////////////////////////////////////////
void Oregon_NR::stop()
{
  detachInterrupt(INT_NO);  
}
//////////////////////////////////////////////////////////////////////
//Захват и анализ пакета
//DEBUG_INFO - в Serial выводится информация о захвате данных
//////////////////////////////////////////////////////////////////////
void Oregon_NR::capture(bool DEBUG_INFO)
{
  ////////////////////////////////////////////////////////
  // Возвращаемся к исходному состоянию
  maybe_packet = 0;
  packets_received = 0;
  sens_type = 0;
  crc_c = 0;
  captured = 0;
  data_val = 0;
  data_val2 = 0;
  
  
  ////////////////////////////////////////////////////////
  //Чтение данных с приёмника
  noInterrupts();
  pulse_length = pl;
  pl = 0;
  pulse_marker = pm;
  interrupts();
  
  ////////////////////////////////////////////////////////
  //Пришёл импульс
  if (pulse_length != 0 && receive_status == FIND_PACKET){  
    if (start_pulse_cnt == 0){
      ////////////////////////////////////////////////////////
      //Найдена первый "правильный" импульс - длинная единица!  
      if (pulse_length < (PER_LENGTH + 16) && pulse_length > (THR_LENGTH) ) {
        start_pulse_cnt = 1;
        pre_marker[start_pulse_cnt] = pulse_marker;
        pulse_length = 0;
      }
    }
    else{
      ///////////////////////////////////////////////////////////
      //Найдена следующий "правильный" импульс - длинная единица!  
      if (pulse_length <= (PER_LENGTH + 16) && pulse_length >= (THR_LENGTH)) {
        ///////////////////////////////////////////////////////////
        //Если импульс в правильном месте, то добавляем счётчик найденых стартовых импульсов
        if(pulse_marker - pre_marker[start_pulse_cnt] > (PER_LENGTH*2-LENGTH_TOLERANCE) && pulse_marker - pre_marker[start_pulse_cnt] < (PER_LENGTH * 2 + LENGTH_TOLERANCE)){
          start_pulse_cnt++;
          pre_marker[start_pulse_cnt] = pulse_marker;
          pulse_length = 0;
        }
        ///////////////////////////////////////////////////////////
        //Следующий импульс находится в неправильном месте
        //Назначаем его первым
        else{
          start_pulse_cnt = 1;
          pre_marker[start_pulse_cnt] = pulse_marker;
          pulse_length = 0;
        }
      }
      else{
        ///////////////////////////////////////////////////////////
        //Если импульс неправильной длины, то стоит проверить, 
        //а не вышло ли время ожидания правильного импульса
        if (pulse_marker - pre_marker[start_pulse_cnt] < (PER_LENGTH * 2 + LENGTH_TOLERANCE)){
          ///////////////////////////////////////////////////////////
          //Время ещё не вышло, скорее всего это помеха. Пропускаем...
           pulse_length = 0;
        }
        else{
          ///////////////////////////////////////////////////////////
          //Время вышло, начинаем искать заново
          start_pulse_cnt = 0;
          pulse_length = 0;
        }
      }
    }
  }
  /////////////////////////////////////////////////////////////////////
  // Если найден первый пакет и вышло вермя ожидания второго
  // Не ждём второго, а переходм в режим анализа
  // Или если отключён режим сращивания пакетов
  if (packet_number == 1 && (millis() - first_packet_end) > 200) receive_status = ANALIZE_PACKETS;
  if (packet_number == 1 && !is_assemble) receive_status = ANALIZE_PACKETS;

  
  //////////////////////////////////////////////////////////////////////
  //Если Найдены три длинных единицы и два длинных нуля. Вероятнее всего это начало посылки...
  // Начинаем СБОР ДАННЫХ
  if (start_pulse_cnt == 3 && receive_status == FIND_PACKET) {
    
    work_time = millis();
    last_premarker = pre_marker[3];
    start_pulse_cnt = 0;
    if (packet_number == 0){
      read_tacts = collect(collect_data);
      first_packet_end = millis();
      packet_number = 1;
    }
    else{
      read_tacts2 = collect(collect_data2);
      packet_number = 2;
      receive_status = ANALIZE_PACKETS;
    }
  }
  //////////////////////////////////////////////////////////////////////
  // Анализ данных////////////////////////////////////////////////////////
  if  (receive_status == ANALIZE_PACKETS) {     
    //////////////////////////////////////////////////////////////////////
    //Отключаем прерывание, чтобы сократить время обработки
    detachInterrupt(INT_NO);
    
    led_light(true);
    
/*      //Дамп собранных данных        
        if (DEBUG_INFO){
          for(int bt = 0; bt < 180; bt++){
            Serial.print(collect_data[bt], HEX);
            Serial.print(' ');
          }
          Serial.println(" ");

          for(int bt = 0; bt < 180; bt++){
            Serial.print(collect_data2[bt],HEX);
            Serial.print(' ');
          }
          Serial.println(" ");
        }
*/
    //////////////////////////////////////////////
    //Обработка первой записи
    //Расшифровываем запись. Данные сохраянем в decode_tacts[]
    get_bits(collect_data);
    bool halfshift;
    if (get_data(0, collect_data) > get_data(1, collect_data)){
      data_val = get_data(0, collect_data);
      halfshift = 0;
    }
    else {
      data_val = get_data(1, collect_data);
      halfshift = 1;
    }
    //////////////////////////////////////////////
    //Ищем позицию синхронибла
    synchro_pos = get_synchro_pos(collect_data);
    //////////////////////////////////////////////
    //Выводим посылку
    if (DEBUG_INFO){
      Serial.print("1)     ");
      for(int bt = 0; bt < READ_BITS; bt++) {
        if (bt <= read_tacts / 2){
          if (collect_data[bt] > 128 + 1) Serial.print('I');
          if (collect_data[bt] < 128 - 1) Serial.print('O');
          if (collect_data[bt] == 128 + 1) Serial.print('i');
          if (collect_data[bt] == 128 - 1) Serial.print('o');
          if (collect_data[bt] == 128) Serial.print('.');
        }
        else Serial.print(' ');
      }
      Serial.print(" SIZE:");
      Serial.print(read_tacts);
      Serial.print(" VAL:");
      Serial.print(data_val);
      Serial.print(" SYN:");
      Serial.print(synchro_pos);
      Serial.print(" SHIFT:");
      Serial.println(halfshift);
    }
    //////////////////////////////////////////////
    //Аналогично обрабатываем вторую запись
    if (packet_number == 2){
      get_bits(collect_data2);
      if (get_data(0, collect_data2) > get_data(1, collect_data2)) {
        data_val2 = get_data(0, collect_data2);
        halfshift = 0;
      }
      else {
        data_val2 = get_data(1, collect_data2);
        halfshift = 1;
      }
      synchro_pos2 = get_synchro_pos(collect_data2);
      if (DEBUG_INFO){
        Serial.print("2)     ");
       for(int bt = 0; bt < READ_BITS; bt++) {
        if (bt <= read_tacts2 / 2){
          if (collect_data2[bt] > 128 + 1) Serial.print('I');
          if (collect_data2[bt] < 128 - 1) Serial.print('O');
          if (collect_data2[bt] == 128 + 1) Serial.print('i');
          if (collect_data2[bt] == 128 - 1) Serial.print('o');
          if (collect_data2[bt] == 128) Serial.print('.');
        }
        else Serial.print(' ');
      }
        Serial.print(" SIZE:");
        Serial.print(read_tacts2);
        Serial.print(" VAL:");
        Serial.print(data_val2);
        Serial.print(" SYN:");
        Serial.print(synchro_pos2);
        Serial.print(" SHIFT:");
        Serial.print(halfshift);
        }
      }
      byte* result_data, result_data_start, aux_data;
      int correlation;
      
      //////////////////////////////////////////////
      //СОПОСТАВЛЕНИЕ ПАКЕТОВ
      //Если пакет один, то и сопоставлять не из чего
       if (packet_number == 1) result_data = collect_data;     
       //////////////////////////////////////////////
       //А вот если два, то нужна СБОРКА ПАКЕТА
       //вычисляем оптимальное "смещение" пакетов друг относительно друга
       if (packet_number == 2) {
        correlation = correlate_data(collect_data, collect_data2);
        if (DEBUG_INFO){
          Serial.print(" COR: ");
          Serial.println(correlation);
        }
        //////////////////////////////////////////////
        //Собираем данные в пакет, где синхронибл найден раньше                
        //////////////////////////////////////////////
        if (synchro_pos >= synchro_pos2)
        {
          result_size = read_tacts2;
          result_data = collect_data2;
          correlation = -correlation;
          assemble_data(collect_data2, collect_data, correlation);
        }
        else
        {
          result_size = read_tacts;
          result_data = collect_data;
          assemble_data(collect_data, collect_data2, correlation);
        }
      }
    //////////////////////////////////////////////
    //Вывод готовой посылки
    if (DEBUG_INFO && packet_number == 2){
      Serial.print("RESULT ");
      byte* rdt = result_data;
      for(int bt = 0; bt < READ_BITS; bt++){
        if (bt <= result_size / 2){
          if (*rdt > 128 + 1) Serial.print('I');
          if (*rdt < 128 - 1) Serial.print('O');
          if (*rdt == 128 + 1) Serial.print('i');
          if (*rdt == 128 - 1) Serial.print('o');
          if (*rdt == 128) Serial.print('.');
        }
        else Serial.print(' ');
          rdt++;
      }
      Serial.println(" ");
    }
    //////////////////////////////////////////////
    //Извлекаем из тактовой последовательности биты
    sens_type = 0;
    if (get_info_data(result_data, packet, valid_p)){
      sens_type = get_sensor(packet); //Определяем тип пакета по типу датчика
      restore_data(packet, sens_type); // Восстанавливаем данные по типу датчика
      crc_c = check_CRC(packet, sens_type); // Проверяем CRC, если оно верно, то все сомнительные биты делаем уверенными
      //Если не все байты определены уверенно, нельзя считать, что пакет корректный
      byte secresingV;
      if (sens_type == THGN132 || (sens_type & 0xFF00) == GAS) secresingV = PACKET_LENGTH - 4;
      if (sens_type == THN132) secresingV = PACKET_LENGTH - 6;
      for (byte www = 0; www < (PACKET_LENGTH - secresingV + 2); www++)
      if (valid_p[www] < 0x0f) crc_c = false;
      //Захват пакета происходит тольок в случае, если найдена стартовая последовательность (нибл синхронизации)
      captured = 1;    
    }
    else if (data_val >=64 || data_val2 >=64) maybe_packet = 1;
////////////////////////////////////////////////////////////////////////////////////////////////////    
//Расшифровка датчиков Орегон
////////////////////////////////////////////////////////////////////////////////////////////////////    
    if ((sens_type == THGN132 || sens_type == THN132) && crc_c){
      sens_tmp2 = 404;
      sens_CO = 255;
      sens_CH = 255;
      sens_id = get_id(packet);
      sens_chnl = get_channel(packet);
      sens_battery = get_battery(packet);
      sens_tmp = get_temperature(packet);
      if (sens_type == THGN132) 
        sens_hmdty = get_humidity(packet);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////    
//Расшифровка комплексных газовых датчиков 
////////////////////////////////////////////////////////////////////////////////////////////////////    
    if ((sens_type & 0xFF00) == GAS && crc_c){
      sens_id = 0;
      sens_battery = 0;
      sens_chnl = get_gas_channel(packet);
      sens_tmp = get_gas_temperature_out(packet);
      if (packet[9] == 0x0F) sens_tmp = 404;
      sens_tmp2 = get_gas_temperature_in(packet);
      if (packet[12] == 0x0F) sens_tmp2 = 404;
      sens_hmdty = get_gas_hmdty(packet);
      sens_CO = get_gas_CO(packet);
      sens_CH = get_gas_CH(packet);
    }
//////////////////////////////////////////////////////////////////////////////////////////////////    
//Расшифровка датчиков пожарной сигнализации
////////////////////////////////////////////////////////////////////////////////////////////////////    
    if ((sens_type & 0xFF00) == FIRE && crc_c){
      sens_id = 0;
      sens_battery = 0;
           
      sens_chnl = get_gas_channel(packet);
      sens_ip22 = get_fire_ip22(packet);
      sens_ip72 = get_fire_ip72(packet);
      sens_lockalarm = get_fire_lockalarm(packet);
    }
    
////////////////////////////////////////////////////////////////////////////////
//Остальные вычисления
//Возвращаем всё в исходное состояние и включаем прослушивание приёмника
    work_time = millis() - work_time;
    packets_received = 0;
    if (data_val >= 64 && synchro_pos != 255) packets_received++;
    if (data_val2 >= 64 && synchro_pos2 != 255) packets_received++;
    receive_status = FIND_PACKET;
    start_pulse_cnt = 0;  
    packet_number = 0;
    led_light(false);
    //Serial.print("LED = ");
    //Serial.println(LED);
    attachInterrupt(INT_NO, receiver_interruption, CHANGE);  
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Извлекает из тактовой последовательности - битовую
//Параметры: cdptr - указатель на записанную тактовую последовательность
////////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_NR::get_bits(byte* cdptr){
  
//Сброс массивов
  byte* cdp=cdptr;
  for(int bt=0 ; bt<READ_BITS*2; bt++) decode_tacts[bt]=2;
      
        
  for(int bt=0 ; bt<READ_BITS*2; bt++){
       
    if ((*cdp&0xf0)>0x20 && (*cdp&0x0f)>0x04) decode_tacts[bt]=1;
    if ((*cdp&0xf0)<0x30 && (*cdp&0x0f)<0x05) decode_tacts[bt]=0;
    if ((*cdp&0xf0)<0x20 && (*cdp&0x0f)>0x04) decode_tacts[bt]=4;
    if ((*cdp&0xf0)>0x40 && (*cdp&0x0f)<0x02) decode_tacts[bt]=3;
    *cdp++;
  }
  return;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Извлекает из записи канала тактовую последовательность
//Параметры: cdptr - указатель на записанные данные
// btt - смещение в тактах. Смещение на такт при анализе может поммочь восстановить пакет, у которого разрушено начало
// Функция вовзращает качество или "годность" расшифровки - количесвто уверенно узнаных тактов.
// Сравнивая годность с btt=0 и btt=1 выбираем лучшую
////////////////////////////////////////////////////////////////////////////////////////////////////
int Oregon_NR::get_data(int btt, byte* cdptr){ //btt - смещение на такт при анализе может поммочь восстановить пакет, у которого разрушено начало
  
  byte* cdp=cdptr;
  for(int bt=0 ; bt<READ_BITS; bt++){
    
    *cdp=128;
    cdp++;
  }
  cdp=cdptr;
  *cdp=(128+2);
  cdp++;
  int packet_validity=0;
  for(int bt=1 ; bt<READ_BITS; bt++){
    
    if(decode_tacts[bt*2-btt]==0) *cdp-=1;
    if(decode_tacts[bt*2-btt]==1) *cdp+=1;
    if(decode_tacts[bt*2-2-btt]==1 && decode_tacts[bt*2-1-btt]==4) *cdp-=1;
    if(decode_tacts[bt*2-2-btt]==0 && decode_tacts[bt*2-1-btt]==3) *cdp+=1;
    if(decode_tacts[bt*2-2-btt]==0 && decode_tacts[bt*2-1-btt]==1) *cdp-=1;
    if(decode_tacts[bt*2-2-btt]==1 && decode_tacts[bt*2-1-btt]==0) *cdp+=1;
    
    if(decode_tacts[bt*2+2-btt]==1 && decode_tacts[bt*2+1-btt]==3) *cdp-=1;
    if(decode_tacts[bt*2+2-btt]==0 && decode_tacts[bt*2+1-btt]==4) *cdp+=1;
    if(decode_tacts[bt*2+2-btt]==0 && decode_tacts[bt*2+1-btt]==1) *cdp-=1;
    if(decode_tacts[bt*2+2-btt]==1 && decode_tacts[bt*2+1-btt]==0) *cdp+=1;
    
        //Подсчитываем кол-во достоверных бит в пакете
    if (*cdp>(128+1) )  packet_validity+=*cdp-128;
    if (*cdp<(128-1)) packet_validity+=128-*cdp;
    cdp++;
  }
  return packet_validity;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Прослушивание канала с частотой дискретизации 16384Гц
//cdptr - указатель на область памяти, куда записываются сигнал
//dtl - указатель на количество считанных тактов
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::collect(byte* cdptr){
  
  
  bool cdp_prev_null;
  byte* cdp = cdptr;
  byte nulls_found = 0;
  //////////////////////////////////////////////////////
  //Запись начинаем с этого момента (конец последнего импулься зацепки + 1/16 такта)
  unsigned long tmp_marker = last_premarker+PER_LENGTH/32;
  byte bt2 = 0;
  //////////////////////////////////////////////////////
  //Первые два такта - единицы. Мы же поймали импульс!
  *cdp = 0x88; 
  cdp++;
  while (micros() <= tmp_marker);
  //////////////////////////////////////////////////////
  //Начинаем читать данные в память
  // Читаем максимум ПО 90 БИТ
  // ПОСЫЛКА thgn - 96БИТ, THN - 76 бИТ + как минимум 3 бита 111, которые мы уже нашли
  byte bt;
  for (bt = 0 ; bt < READ_BITS2; bt++) {        
    *cdp = 0;
    for (byte ckl = 0; ckl < 8; ckl++) {            // Читаем 8 раз за полутакт
      if (digitalRead(RECEIVER_PIN)) *cdp += 0x10;  // Измерения запиываем в старший полубайт
      tmp_marker += PER_LENGTH / 16;
      while (micros() < tmp_marker);
    }
    last_premarker += PER_LENGTH / 2;
    tmp_marker = last_premarker + PER_LENGTH / 32;
    for (byte ckl = 0; ckl < 8; ckl++) {
      if (digitalRead(RECEIVER_PIN)) *cdp += 1;     // В следующий полутакт измерения запиываем в младший  полубайт. Это экономит память.
      tmp_marker += PER_LENGTH / 16;
      while (micros() < tmp_marker);
    }
    last_premarker += PER_LENGTH / 2;
    bt2++;
    //  Каждые 8 тактов добавлять 5мкс для выравнивания периода с 976мкс до 976.56мкс
    if (bt2 == 8) { 
      last_premarker += 5;
      bt2 = 0;
    }
    tmp_marker = last_premarker + PER_LENGTH / 32;
    cdp++;
    /////////////////////////////////////////////
    //Есть время до прихода следующего полутакта
    //Можно проверить, а не закончилась ли посылка
    //Если в канале последнее время пустота, то это надо отметить

    //if ((*cdp&0xf0)<0x30 && (*cdp&0x0f)<0x05) decode_tacts[bt]=0;
    //if (*cdp == 0 && (*(cdp - 1) == 0)) nulls_found++;
    yield();
    if ((*cdp & 0xf0) < 0x30 && (*cdp & 0x0f) < 0x05 && ((*(cdp - 1) & 0xf0) < 0x30 && (*(cdp - 1) & 0x0f) < 0x05)) nulls_found++;
    else nulls_found = 0;
    /////////////////////////////////////////////
    //Если более empty_space пустых полей в записи, то
    //это вероятнее всего конец посылки. Дальше читать нет смысла
    //empty_space - число эмпирическое, зависит от типа приёмника и уровня сигнала
    //Если уменьшить, возможно спутать с повреждением пакета
    //Если увеличить, то можно не прекратить чтение и начать записывать помехи
    if (nulls_found > empty_space ) return bt;
    /////////////////////////////////////////////
    //Ждём прихода времени следующего полутакта
    while (micros() < last_premarker);                    
  }
  return bt;
} 
////////////////////////////////////////////////////////////////////////////////////////////////////
//Определение смещения пакетов друг относительно друга
//В качестве параметров передаются указатели на массивы данных
// Возваращаяется смещение
// >0 - второй пакет начался раньше, <0 - Первый пакет начался раньше
////////////////////////////////////////////////////////////////////////////////////////////////////
int Oregon_NR::correlate_data(byte* ser1, byte* ser2){
  
  byte best_correl = 0;
  int best_shift = 0;
  byte best_correl_back = 0;
  int best_shift_back = 0;
  byte shift_score[READ_BITS];
  byte* s1;
  byte* s2;
  byte* s2t = ser2;
  //смещаем первый пакет относительно второго
  for (byte sht = 0; sht < READ_BITS; sht++){
    s1 = ser1;
    s2 = s2t;
    shift_score[sht] = 0;
    for(byte sp = 0; sp < READ_BITS - sht; sp++){
      if ((*s1 > (128+1) && *s2 > (128+1))||(*s1 < (128-1) && *s2 < (128-1)) ) shift_score[sht]++;
      s2++;
      s1++;
    }
    yield();
    s2t++;
  }
  for (int i = 0; i < READ_BITS; i++){

    if (shift_score[i] > best_correl){
      best_correl = shift_score[i];
      best_shift = i;
    }
  }
 
//Теперь наоборот -втрой пакет относительно первого
  
  byte* s1t = ser1;
  for (byte sht = 0; sht < READ_BITS; sht++){
    s2 = ser2;
    s1 = s1t;
    shift_score[sht] = 0;
    for (byte sp = 0; sp < READ_BITS-sht; sp++){
      
      if ((*s1 > (128+1) && *s2 > (128+1))||(*s1 < (128-1) && *s2 < (128-1)) ) shift_score[sht]++;
      s2++;
      s1++;
    }
    yield();
    s1t++;
  }
// Ищем наилучшее совпадение для обоих вариантов 
    
  for (int i = 0; i < READ_BITS; i++){
    
    if (shift_score[i] > best_correl_back){
      best_correl_back = shift_score[i];
      best_shift_back = i;
    }
  }
  //И возвращаем самое лучшее из двух
  if (best_correl_back > best_correl) return -best_shift_back;
  else return best_shift;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Сборка из двух пакетов
//В качестве параметров передаются указатели на массивы данных
// Причём первым должен идти результирующий пакет, т.е. тот который имеет более длинную преамбулу.
//shift - смещение втрого пакета относительного первого
////////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_NR::assemble_data(byte* s1, byte* s2, int shift){
  
  if (shift >= 0) {
    for (int g = 0; g < shift; g++) s2++;
    for (int i = 0; i < READ_BITS - shift; i++){
      if(*s1 < (128 + 2) && *s1 > (128 - 2) && (*s2 > (128 + 1) || *s2 < (128 - 1))) {
      *s1 = *s2;
      }
      s1++;
      s2++;
    }
  }
  else {
    for (int g = 0; g < -shift; g++) s1++;
    for (int i = 0; i < READ_BITS + shift; i++){
      if(*s1 < (128 + 2) && *s1 > (128 - 2) && (*s2 > (128 + 1) || *s2 < (128 - 1))) {
      *s1 = *s2;
      }
      s1++;
      s2++;
    }
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает позицию синхронибла в посылке
// 0xFF - нет синхронибла
//code - указатель на расшифрованную битовую последовательность
//result - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_synchro_pos(byte* code){
  
  bool syn_found = false;
  byte* cp = code;
  int i = 0;
  for (i = 0; i < READ_BITS - 8; i++){
    if (!consist_synchro && (*cp < 128 && *(cp + 1) > 128 && *(cp + 2) < 128 && *(cp + 3) > 128)) {
      syn_found = true;
      break;
    }

    if (consist_synchro && (*cp < 127 && *(cp + 1) > 129 && *(cp + 2) < 127 && *(cp + 3) > 129)) {
      syn_found = true;
      break;
    }
    
    cp++;
  }
  if (!syn_found) return 0xFF;
  //Последовательность нашли, но надо убедиться, что перед этим идёт перамбула, т. е. либо уверенные единицы, либо неразборчивый сигнал
  //Преамбулу надо просматривать на 16-3 = 13 битов назад. Ну хотя бы на 10!!!
  
  for (byte g = i; i - g < 10 && g > 0; g --){
    cp --;
    if (*cp < 127) return 0xFF; // Перед синхрониблом в преамбуле не может быть уверенного нуля. Нет тут синхронибла!
  }
  return (byte) i;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Создаёт кодовую посылку
//code - указатель на расшифрованную битовую последовательность
//result - указатель на кодовую посылку
//valid - указатель на карту достоверности кодовой посылки
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_info_data(byte* code, byte* result, byte* valid){

  byte* rd = result;
  byte* vd = valid;
  //Чистим массивы
  for (int l = 0; l < PACKET_LENGTH; l++){
    *vd = 0;
    *rd = 0;
    vd++;
    rd++;
  }
  rd = result;
  vd = valid;
  
  int csm;
  for (csm = 0; csm < 30; csm++){
    if ( !consist_synchro && (*code < 128 && *(code + 1) > 128 && *(code + 2) < 128 && *(code + 3) > 128)) break; //Найдена последовательность 0101 
    if (  consist_synchro && (*code < 127 && *(code + 1) > 129 && *(code + 2) < 127 && *(code + 3) > 129)) break; 
    code++;
  }
  // Стартовая последовательность в первых 20 битах не найдена, такой пакет этим методом не расшифруешь
  
  if (csm > 22) return 0; 
  //Переходим на начало считывания
  code += 4;
  int ii = 0;
  for (int i = 0; i < READ_BITS - csm; i++) 
  {
    // Чтобы не выйти за пределы
    if (i >= PACKET_LENGTH * 4) break;
    byte multipl;
    switch (ii){
      case 0: {multipl = 0x01; break;}
      case 1: {multipl = 0x02; break;}
      case 2: {multipl = 0x04; break;}
      case 3: {multipl = 0x08; break;}
    }
    if (*code == 129 ) *rd += multipl;
    if (*code > 129 ) {
      *rd += multipl;
      *vd += multipl;
    }
    if (*code < 127 ) *vd += multipl;
    code ++;
    ii ++;
    if (ii == 4) {
      ii = 0;
      vd++;
      rd++;
    }
  }
  return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает значение температуры
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
float Oregon_NR::get_temperature(byte* oregon_data){
  float tmprt;
  oregon_data+=8;
  //исправляем возможные ошибки:
  for (int g=0;g<4; g++)  if (*(oregon_data+g)>9) *(oregon_data+g)=*(oregon_data+g)-8;
  tmprt+=*(oregon_data)*0.1;
  tmprt+=*(oregon_data+1);
  tmprt+=*(oregon_data+2)*10;
  return (*(oregon_data+3)) ? -tmprt : tmprt;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает тип сенсора
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
word Oregon_NR::get_sensor(byte* oregon_data){
    return (word)(*(oregon_data))*0x1000 + (*(oregon_data+1))*0x0100 + (*(oregon_data+2))*0x10 + *(oregon_data+3);
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает значение канала
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_channel(byte* oregon_data){

  byte channel;  
  switch (*(oregon_data+4))  
  {  
  case 1:  
    channel = 1;  
    break;  
  case 2:  
    channel = 2;  
    break;  
  case 4:  
    channel = 3;  
    break;  
  }  
  return channel;  
}
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_battery(byte* oregon_data){

  return (*(oregon_data+7) & 0x4) ? 0 : 1;  
  
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает значение влажности
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_humidity(byte* oregon_data){

  byte tmprt;
  oregon_data+=12;
  //исправляем возможные ошибки:
  for (int g=0;g<2; g++)  if (*(oregon_data+g)>9) *(oregon_data+g)=*(oregon_data+g)-8;
  tmprt=*(oregon_data);
  tmprt+=*(oregon_data+1)*10;
  return tmprt;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Возвращает id датчика
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_id(byte* oregon_data){

  byte tmprt;
  oregon_data+=5;
  tmprt=*(oregon_data)*0x10;
  tmprt+=*(oregon_data+1);
  return tmprt;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Проверка CRC
//oregon_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
bool Oregon_NR::check_CRC(byte* oregon_data, word sensor_type){

  byte* pp=oregon_data;
  byte crc, resived_crc, truecrc, resived_truecrc, i;
  crc=0;
  byte CCIT_POLY = 0x07;
  
  if (sensor_type==THGN132){
    truecrc = 0x3C;
    for(int x=0; x<15; x++){
      crc += *pp;
      if ( x != 5 && x != 6){
        truecrc ^= *pp;
        for(i = 0; i<4; i++) 
          if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
          else truecrc <<= 1;
        //truecrc &= 0xff;
      }
      pp++;  
    }
    for(i = 0; i<4; i++) 
      if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
      else truecrc <<= 1;

    resived_crc = (*(oregon_data+15))+(*(oregon_data+16))*0x10;
    resived_truecrc = (*(oregon_data+17))+(*(oregon_data+18))*0x10;
    received_CRC = truecrc;
    return (resived_crc == crc && resived_truecrc == truecrc)? 1 : 0;
  }


  if ((sensor_type & 0xFF00) == GAS || (sensor_type & 0xFF00) == FIRE ){
    truecrc = 0x00;
    for(int x=0; x<15; x++){
      crc += *pp;
      truecrc ^= *pp;
      for(i = 0; i<4; i++) 
        if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
        else truecrc <<= 1;
      //truecrc &= 0xff;
      pp++;  
    }
    for(i = 0; i<4; i++) 
      if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
      else truecrc <<= 1;

    resived_crc = (*(oregon_data+15))+(*(oregon_data+16))*0x10;
    resived_truecrc = (*(oregon_data+17))+(*(oregon_data+18))*0x10;
    received_CRC = truecrc;
    return (resived_crc == crc && resived_truecrc == truecrc)? 1 : 0;
    //return (resived_crc == crc)? 1 : 0;
  }

  if (sensor_type==THN132){
    truecrc = 0xD6;
    for(int x=0; x<12; x++){
      crc += *pp;
      if ( x != 5 && x != 6){
        truecrc ^= *pp;
        for(i = 0; i<4; i++) 
          if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
          else truecrc <<= 1;
        //truecrc &= 0xff;
      }
      pp++;  
    }
    for(i = 0; i<4; i++) 
      if(truecrc & 0x80) truecrc = (truecrc << 1) ^ CCIT_POLY;
      else truecrc <<= 1;

    resived_crc = (*(oregon_data+12))+(*(oregon_data+13))*0x10;
    resived_truecrc = (*(oregon_data+14))+(*(oregon_data+15))*0x10;
    received_CRC = truecrc;
    return (resived_crc == crc && resived_truecrc == truecrc)? 1 : 0;
  }
  return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
//Востановление данных по типу датчика
////////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_NR::restore_data(byte* oregon_data, word sensor_type){
  
  byte* pp=oregon_data;
  if (sensor_type==THGN132){
    pp+=8;
    for(int x=0; x<6; x++){
      if(*pp>9 && x!=3) *pp-=8;
      pp++;  
    }
  }
  if (sensor_type==THN132){
    pp+=8;
    for(int x=0; x<3; x++){
      if(*pp>9) *pp-=8;
      pp++;  
    }
  }
  return;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//Функции расшифровки данных датчиков GAS
//gas_data - указатель на кодовую посылку
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_gas_channel(byte* gas_data){

  return gas_data[2];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
float Oregon_NR::get_gas_temperature_out(byte* gas_data){

  int temperat = gas_data[9] * 0x0100 + gas_data[8] * 0x0010 + gas_data[7];
  return ((float)(-1000 + temperat)) / 10;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
float Oregon_NR::get_gas_temperature_in(byte* gas_data){

  int temperat = gas_data[12] * 0x0100 + gas_data[11] * 0x0010 + gas_data[10];
  return ((float)(-1000 + temperat)) / 10;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_gas_hmdty(byte* gas_data){

  return gas_data[14] * 0x10 + gas_data[13];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_gas_CO(byte* gas_data){

  return gas_data[6] * 0x10 + gas_data[5];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_gas_CH(byte* gas_data){

  return gas_data[4] * 0x10 + gas_data[3];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void Oregon_NR::led_light(bool led_on) {
  if (LED != 0xFF) { 
    if (PULL_UP && led_on) digitalWrite(LED, LOW);
    if (PULL_UP && !led_on) digitalWrite(LED, HIGH);
    if (!PULL_UP && led_on) digitalWrite(LED, HIGH);
    if (!PULL_UP && !led_on) digitalWrite(LED, LOW);
  }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_fire_ip22(byte* fire_data){

  return fire_data[4] * 0x10 + fire_data[5];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_fire_ip72(byte* fire_data){

  return fire_data[6] * 0x10 + fire_data[7];
}
////////////////////////////////////////////////////////////////////////////////////////////////////
byte Oregon_NR::get_fire_lockalarm(byte* fire_data){

  return fire_data[8] * 0x10 + fire_data[9];
}
  
