#pragma once

#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <iostream>

namespace color
{

static const double PROTANOPIA_MATRIX[9] = 
{
   0.567, 0.433, 0.0,
   0.558, 0.442, 0.0,
   0.0, 0.242, 0.758
};
static const double DEUTERANOPIA_MATRIX[9] = 
{
   0.625, 0.375, 0.0,
   0.7, 0.3, 0.0,
   0.0, 0.3, 0.7
};
const double TRITANOPIA_MATRIX[9] = 
{
   0.95, 0.05, 0.0,
   0.0, 0.433, 0.567,
   0.0, 0.475, 0.525
};
const double PROTANOMALY_MATRIX[9] = 
{
   0.817, 0.183, 0.0,
   0.333, 0.667, 0.0,
   0.0, 0.125, 0.875
};
const double DEUTERANOMALY_MATRIX[9] = 
{
   0.8, 0.2, 0.0,
   0.258, 0.742, 0.0,
   0.0, 0.142, 0.858
};
const double TRITANOMALY_MATRIX[9] = 
{
   0.967, 0.033, 0.0,
   0.0, 0.733, 0.267,
   0.0, 0.183, 0.817
};
const double TRISTIMULUS_MATRIX[9] = 
{
   3.2406, -1.5372, -0.4986,
   -0.9689, 1.8758, 0.0415,
   0.0557, -0.2040, 1.0570
};

enum BlindnessType : size_t
{
   NORMAL, DEUTERANOPIA, PROTANOPIA, TRITANOPIA,
   DEUTERANOMALY, PROTANOMALY, TRITANOMALY, LAST
};

const std::map<BlindnessType, std::string> BlindnessTypeNames = {
   {BlindnessType::NORMAL, "Normal"},
   {BlindnessType::DEUTERANOPIA, "Deuteranopia"},
   {BlindnessType::PROTANOPIA, "Protanopia"},
   {BlindnessType::TRITANOPIA, "Tritanopia"},
   {BlindnessType::DEUTERANOMALY, "Deuteranomaly"},
   {BlindnessType::PROTANOMALY, "Protanomaly"},
   {BlindnessType::TRITANOMALY, "Tritanomaly"},
   {BlindnessType::LAST, "Last"}
};

struct Color
{
   Color() : r(0), g(0), b(0) {}
   Color(size_t r, size_t g, size_t b) : r(r), g(g), b(b) {}
   double Distance(const Color& other) const;
   void Print() const;
   size_t r, g, b;
};

struct Palette
{
   Palette(const std::string& name) : m_Name(name) {};
   void AddColor(const Color& color) { m_Colors.push_back(color); };
   void Print(bool colors = false) const;
   void Evaluate();
   void Draw() const;
   std::vector<Color> m_Colors;
   std::string m_Name;
   struct PaletteEvaluation
   {
      PaletteEvaluation()
      {
         m_AverageDistance = 0;
         m_MinDistance = 0;
         m_MaxDistance = 0;
         m_ColorRepresentation = 0;
      }
      void Print() const
      {
         std::cout << "Average Distance: " << m_AverageDistance << std::endl;
         std::cout << "Minimum Distance: " << m_MinDistance << std::endl;
         std::cout << "Maximum Distance: " << m_MaxDistance << std::endl;
         std::cout << "Color Representation: " << m_ColorRepresentation << std::endl;
      }
      double m_AverageDistance;
      double m_MinDistance;
      double m_MaxDistance;
      double m_ColorRepresentation;
   };

   PaletteEvaluation m_Evaluation;
};

struct Converter
{
   static Color ConvertColor(const Color& color, const BlindnessType& type);
   static void StandardToLinear(const Color& standard, double* r, double* g, double* b);
   static void LinearToStandard(const double& r, const double& g, const double& b, Color* standard);
   static Color WavelengthToStandard(double wavelength);
   static void ApplyMatrix(const double matrix[], const double& r, const double g, const double b, 
                           double* convertedR, double* convertedG, double* convertedB);
};

class Palettes
{
public:
   void GenerateColorPalette();
   void PrintPalettes() const;
   void PrintPaletteQuality() const;
   void DrawPalette(size_t id, BlindnessType type) const;
private:
   std::map<size_t, std::map<BlindnessType, Palette>> m_Palettes;
private:
   size_t AddPalette(const Palette& palette);

   void GenerateBlindnessPalettes(const size_t id);
   void EvaluateColorPalettes(const size_t id);

   size_t GenerateAllBlack();
   size_t GenerateDefault();
   size_t GenerateVisibleSpectrum();
};

};