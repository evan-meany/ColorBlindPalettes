#include <iostream>
#include "Palette.h"

int main(int argc, char *argv[])
{
   color::Palettes palettes;
   palettes.GenerateColorPalette();
   palettes.EvaluateColourPalettes();
   palettes.PrintPaletteQuality();
}