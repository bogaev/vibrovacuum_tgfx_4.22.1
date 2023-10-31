#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DISCONNECTED_BACKEND
//#define NO_COIL
//#define NO_PUMP
#define NO_ELECTRO
#define NO_SERIALIZATION

#if defined (WIN32)
  #define USE_FAKE_INTERFACE
  #define USE_FAKE_MEMORY
#elif defined (HARDWARE_TESTS)
  #define USE_HARDWARE_TESTS
#elif defined (SIMULATOR_TESTS)
  #define USE_SIMULATOR_TESTS
#else
  #define USE_FLASH_MEMORY
//  #define USE_EEPROM_MEMORY
//  #define USE_FAKE_MEMORY
  #define USE_FAKE_INTERFACE
//  #define USE_WIRELESS_INTERFACE
#endif

#endif // _CONFIG_H_
