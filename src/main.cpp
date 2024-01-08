#include <iostream>
#include "Palette.h"

#include "GLEW/glew.h" 
#include "GLFW/glfw3.h"

int main(int argc, char* argv[])
{
   color::PalettesGA palettes(color::BlindnessType::DEUTERANOPIA, 30);
   palettes.RunGA(1000, 0.8, 0.3);

   return 0;
}
