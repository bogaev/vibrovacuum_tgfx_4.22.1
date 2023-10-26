#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <touchgfx/widgets/TextAreaWithWildcard.hpp>

/**
  @brief        ���������� ���������� ���� TouchGFX
  @param [in]   txtNew � ����� �����
  @param [in]   txtAreaBuffer � ����� ��� ������ ������
  @param [in]   txtObject � ������ ���������� ����
  @param [in]   txtAreaSize � ������ ���������� ����
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
