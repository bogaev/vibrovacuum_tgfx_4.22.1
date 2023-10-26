#include <gui/common/GuiUtilities.hpp>
//#include "common_\common.h"

using namespace touchgfx;

void updateTextArea
(Unicode::UnicodeChar* txtNew,
	Unicode::UnicodeChar* txtAreaBuffer,
	TextAreaWithOneWildcard& txtObject,
	const uint16_t txtAreaSize)
{
	Unicode::snprintf(txtAreaBuffer, txtAreaSize, "%s", txtNew);
	txtObject.resizeToCurrentTextWithAlignment();
	txtObject.invalidate();
}

void updateValueArea(int value,
	Unicode::UnicodeChar* txtAreaBuffer,
	TextArea& txtObject,
	const uint16_t txtAreaSize)
{
	Unicode::UnicodeChar txtNew[4];
	Unicode::itoa(value, txtNew, txtAreaSize, 10);
	Unicode::snprintf(txtAreaBuffer, txtAreaSize, "%s", txtNew);
	txtObject.invalidate();
}

void updateValueArea(double value,
	Unicode::UnicodeChar* txtAreaBuffer,
	TextArea& txtObject,
	const uint16_t txtAreaSize)
{
	Unicode::UnicodeChar txtNew[4];
	Unicode::snprintfFloat(txtNew, 4, "%2.2f", (float)value);
	Unicode::snprintf(txtAreaBuffer, txtAreaSize, "%s", txtNew);
	txtObject.invalidate();
}
