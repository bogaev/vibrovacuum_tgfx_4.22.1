#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

/**
  @brief        ќбновление текстового пол€ TouchGFX
  @param [in]   txtNew Ч новый текст
  @param [in]   txtAreaBuffer Ч буфер дл€ записи текста
  @param [in]   txtObject Ч объект текстового пол€
  @param [in]   txtAreaSize Ч размер текстового пол€
*/
void updateTextArea(touchgfx::Unicode::UnicodeChar* txtNew,
					touchgfx::Unicode::UnicodeChar* txtAreaBuffer,
					touchgfx::TextArea& txtObject,
					const uint16_t txtAreaSize);

void updateValueArea(int value,
	touchgfx::Unicode::UnicodeChar* txtAreaBuffer,
	touchgfx::TextArea& txtObject,
	const uint16_t txtAreaSize);

void updateValueArea(double value,
	touchgfx::Unicode::UnicodeChar* txtAreaBuffer,
	touchgfx::TextArea& txtObject,
	const uint16_t txtAreaSize);

#endif // UTILITIES_HPP
