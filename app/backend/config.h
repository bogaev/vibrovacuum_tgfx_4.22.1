#ifndef _CONFIG_H_
#define _CONFIG_H_

//#define NO_COIL
//#define NO_PUMP
//#define NO_ELECTRO
#define NO_SERIALIZATION
#if defined(NO_PUMP) || defined(NO_COIL) || defined(NO_ELECTRO)
  #define DISCONNECTED_BACKEND
#endif

#endif // _CONFIG_H_
