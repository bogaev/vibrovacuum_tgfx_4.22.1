#ifndef _SIG_GEN_CONFIG_H_
#define _SIG_GEN_CONFIG_H_

// Параметры сигналов
#define CARRIER_FREQ_MAX_HZ 5000 // максимальная частота несущего сигнала
#define CARRIER_FREQ_MIN_HZ 1 // максимальная частота несущего сигнала
#define POINTS_PER_PERIOD_NUM 100 // количество точек на минимальный период несущего сигнала
#define SAMPLE_RATE (POINTS_PER_PERIOD_NUM * CARRIER_FREQ_MAX_HZ) // частота семплирования таймера без DMA / частота вывода DMA
#define MAX_PATTERN_STAGES 2 // максимальное количество паттернов в одном сигнале, без учета паузы

// Параметры генератора DMA
#define GEN_TOTAL_NUM MAX_PATTERN_STAGES // общее количество генераторов DMA
#define USE_SDRAM 1
#define SDRAM_ADDRESS_START 0xD0000000
#define SDRAM_BANK_SIZE 0x00200000
#define BUF_ADDRESS_START (SDRAM_ADDRESS_START + 2 * SDRAM_BANK_SIZE) // первые 2 банка зарезервированы touchgfx
//#define BUF_DATA_TYPE int16_t // тип данных для DMA
#define BUF_DATA_TYPE int32_t // тип данных для DMA
#define IT_BUF_DATA_TYPE BUF_DATA_TYPE // тип данных для IT

#define BUF_SIZE ((CARRIER_FREQ_MAX_HZ - CARRIER_FREQ_MIN_HZ) \
                  / CARRIER_FREQ_MIN_HZ \
                  * POINTS_PER_PERIOD_NUM)

#define RTOS_TASK_STACK_SIZE 1024

/*
  Прерывание таймера семплирования для генераторов, которые работают не по DMA,
  должны иметь более высокий приоритет
*/

#endif // #ifndef _SIG_GEN_CONFIG_H_