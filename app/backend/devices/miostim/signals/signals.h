/**
  ******************************************************************************
  * @file    signals.h
  * @author  bogaev.s@gmail.com
  * @brief   Математические функции для вычисления значений сигнала.
  *
  *          Формулы заимствованы отсюда:
  *          https://math.stackexchange.com/questions/178079/
  *          integration-of-sawtooth-square-and-triangle-wave-functions
  ******************************************************************************
  */

#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include <cmath>
#include <memory>

#include "backend\devices\miostim\signal_generator_c_interface\signal_generator_c_interface.h"

/**
  * @brief  Класс абстрактного сигнала
  */
class Signal {
 public:
  Signal(uint32_t sample_timer_period);
  virtual ~Signal() {};
  void SetParam(uint8_t param, FP_TYPE value);

  Signal& SetAmp(FP_TYPE amp);
  Signal& SetFreq(FP_TYPE freq);
  Signal& SetFmodDepth(FP_TYPE mod_depth_percent);

  FP_TYPE GetAmp() const;
  FP_TYPE GetFreq() const;
  FP_TYPE GetModDepth() const;

  std::unique_ptr<Signal> Create(enSignalTypes sig_type);

  /**
    * @brief  Вычисляет отдельное значение сигнала
    * @param  point порядковый номер отсчета сигнала
    * @retval Значение сигнала в точке point
    */
  virtual FP_TYPE GetValue(uint32_t point) const {
    return 0.0;
  }

  /**
    * @brief  Вычисляет частотную модуляцию сигнала на базе 2 сигналов
    * @param  point порядковый номер отсчета сигнала
    * @param  sig сигнал, которым модулируется частота текущего сигнала
    * @retval Значение сигнала в точке point
    */
  virtual FP_TYPE FreqMod(uint32_t point, Signal& sig) const {
    return 0.0;
  }

  /**
    * @brief  Вычисляет значение интеграла текущего сигнала.
              Используется для вычисления частотной модуляции.
    * @param  point порядковый номер отсчета сигнала
    * @retval Значение интеграла сигнала в точке point
    */
  virtual FP_TYPE GetIntegral(uint32_t point) const {
    return 0.0;
  }

  /**
    * @brief  Возвращает тип сигнала
    * @retval Тип сигнала
    */
  virtual inline enSignalTypes GetSignalType() const {
    return SIG_GEN_TYPE_NONE;
  }

 protected:
  const FP_TYPE pi = std::acosf(-1.0);
  const FP_TYPE sample_rate_; /// частота семплирования сигнала в Гц
  FP_TYPE amp_ = 1.0; /// амплитуда сигнала (используются значения между 0.0 и 1.0)
  FP_TYPE freq_ = 1.0; /// частота сигнала в герцах
  FP_TYPE period_ = 1.0; /// период сигнала в секундах
  uint8_t fmod_depth_percent_ = 100.0; /// глубина модуляции в процентах
};

/**
  * @brief  Класс синусоидального сигнала
  */
class Sinus : public Signal {
 public:
  Sinus(uint32_t sample_timer_period);
  FP_TYPE GetValue(uint32_t point) const override;
  FP_TYPE FreqMod(uint32_t point, Signal& fmod) const override;
  FP_TYPE GetIntegral(uint32_t point) const override;
  enSignalTypes inline GetSignalType() const override {
    return SIG_GEN_TYPE_SINUS;
  }
};

/**
  * @brief  Класс прямоугольного сигнала
  */
class Square : public Signal {
 public:
  Square(uint32_t sample_timer_period);
  FP_TYPE GetValue(uint32_t point) const override;
  FP_TYPE FreqMod(uint32_t point, Signal& fmod) const override;
  FP_TYPE GetIntegral(uint32_t point) const override;
  enSignalTypes inline GetSignalType() const override {
    return SIG_GEN_TYPE_SQUARE;
  }

 private:
  FP_TYPE square(FP_TYPE t) const;
};

/**
  * @brief  Класс треугольного сигнала
  */
class Triangle : public Signal {
 public:
  Triangle(uint32_t sample_timer_period);
  FP_TYPE GetValue(uint32_t point) const override;
  FP_TYPE FreqMod(uint32_t point, Signal& fmod) const override;
  FP_TYPE GetIntegral(uint32_t point) const override;
  enSignalTypes inline GetSignalType() const override {
    return SIG_GEN_TYPE_TRIANGLE;
  }

 private:
  FP_TYPE triangle(FP_TYPE t) const;
};

/**
  * @brief  Класс пилообразного сигнала
  */
class Saw : public Signal {
 public:
  Saw(uint32_t sample_timer_period);
  FP_TYPE GetValue(uint32_t point) const override;
  FP_TYPE FreqMod(uint32_t point, Signal& fmod) const override;
  FP_TYPE GetIntegral(uint32_t point) const override;
  enSignalTypes inline GetSignalType() const override {
    return SIG_GEN_TYPE_SAW;
  }

 private:
  FP_TYPE sawtooth(FP_TYPE t) const;
};

#endif // #ifndef _SIGNALS_H_