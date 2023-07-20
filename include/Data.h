#pragma once

#include "Palette.h"

namespace color {

struct PaletteData
{
	static Palette DefaultPalette();
	static Palette VisibleSpectrumPalette();
};

}