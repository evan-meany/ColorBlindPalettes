#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <type_traits>

template<typename T>
constexpr std::size_t size(const T&) noexcept {
    return static_cast<std::size_t>(std::max_value<T>) + 1;
}

namespace color
{

enum class BlindnessType : size_t
{
   NORMAL, DEUTERANOPIA, PROTANOPIA, TRITANOPIA,
   Deuteranomaly, Protanomaly
};

struct Color
{
   Color() : r(0), g(0), b(0) {}
   Color(size_t r, size_t g, size_t b) : r(r), g(g), b(b) {}
   double Distance(const Color& other) const;
   void Prsize_t() const;
   size_t r, g, b;
};

struct Palette
{
   Palette(const std::string& name) : m_Name(name) {};
   void AddColor(const Color& color) { m_Colors.push_back(color); };
   void Prsize_t() const;
   std::vector<Color> m_Colors;
   std::string m_Name;
};

struct BlindnessPalettes
{
   BlindnessPalettes(const Palette& palette);
   void GenerateBlindPalette(const BlindnessType type);
   std::unordered_map<BlindnessType, Palette> m_Palettes;
};

struct ColorConverter
{
   static Color ConvertColor(Color color);
   static Color ConvertDeuteranopia(Color& color)
   {
      Color convert;
      convert.r = (0.625 * color.r) + (0.375 * color.g);
      convert.g = (0.625 * color.r) + (0.375 * color.g);
      convert.b = color.b;
      return convert;
   }
   static Color ConvertProtanopia(Color& color)
   {
      Color convert;
      convert.r = (0.567 * color.r) + (0.433 * color.g);
      convert.g = (0.567 * color.r) + (0.375 * color.g);
      convert.b = color.b;
      return convert;
   }
   static Color ConvertTritanopia(Color& color)
   {
      Color convert;
      convert.r = (0.625 * color.r) + (0.375 * color.g);
      convert.g = (0.625 * color.r) + (0.375 * color.g);
      convert.b = color.b;
      return convert;
   }
   static Color ConvertDeuteranomaly(Color& color)
   {
      Color convert;
      convert.r = (0.625 * color.r) + (0.375 * color.g);
      convert.g = (0.625 * color.r) + (0.375 * color.g);
      convert.b = color.b;
      return convert;
   }
   static Color ConvertProtanomaly(Color& color)
   {
      Color convert;
      convert.r = (0.625 * color.r) + (0.375 * color.g);
      convert.g = (0.625 * color.r) + (0.375 * color.g);
      convert.b = color.b;
      return convert;
   }

   double normalToLinearSingle(double normalValue) {
      double linearValue;
      if (normalValue <= 10.0) {
         linearValue = normalValue / 3294.6;
      } else {
         linearValue = pow((normalValue + 14.025) / 269.025, 2.4);
      }
      return linearValue;
   }
   double linearToNormalSingle(double linearValue) {
      double normalValue;
      if (linearValue <= 0.0031308) {
         normalValue = linearValue * 3294.6;
      } else {
         normalValue = 269.025 * pow(linearValue, 1.0 / 2.4) - 14.025;
      }
      return normalValue;
   }
   void normalToLinearRGB(Color& color) {
      color.r = (size_t)(normalToLinearSingle(color.r) * 255.0 + 0.5);
      color.g = (size_t)(normalToLinearSingle(color.g) * 255.0 + 0.5);
      color.b = (size_t)(normalToLinearSingle(color.b) * 255.0 + 0.5);
   }
   void linearToNormalRGB(Color& color) {
      color.r = (size_t)(linearToNormalSingle(color.r / 255.0) + 0.5);
      color.g = (size_t)(linearToNormalSingle(color.g / 255.0) + 0.5);
      color.b = (size_t)(linearToNormalSingle(color.b / 255.0) + 0.5);
   }

};

class Palettes
{
public:
   void AddPalette(const Palette& palette);
   void GenerateColorPalette();
   void EvaluateColourPalettes();
   void Prsize_tPalettes() const;
   void Prsize_tPaletteQuality() const;

private:
   std::unordered_map<size_t, Palette> m_Palettes;
   std::unordered_map<size_t, double> m_PaletteQuality;
private:
   double DetermineQuality(const Palette& palette) const;
};

double Color::Distance(const Color& other) const
{
   double distance = sqrt(pow(r - other.r, 2) + pow(g - other.g, 2) + pow(b - other.b, 2));
   return distance;
}

void Color::Prsize_t() const
{
   std::cout << "(" << r << ", " << g << ", " << b << ")";
}

void Palette::Prsize_t() const
{
   std::cout << "Palette: " << m_Name << std::endl;
   for (const auto& color : m_Colors)
   {
      color.Prsize_t();
      std::cout << std::endl;
   }
}

BlindnessPalettes::BlindnessPalettes(const Palette& palette)
{
   using underlying_t = std::underlying_type_t<BlindnessType>;
   for (underlying_t i = 0; i < static_cast<underlying_t>(size(BlindnessType{})); ++i)
   {
      BlindnessType type = static_cast<BlindnessType>(i);
      GenerateBlindPalette(type);
   }
}

void GenerateBlindPalette(const BlindnessType type)
{
   switch(type)
   {

   case BlindnessType::NORMAL:
   {
      break;
   }
   case BlindnessType::DEUTERANOPIA:
   {

      break;
   }
   case BlindnessType::PROTANOPIA:
   {
      break;
   }
   case BlindnessType::TRITANOPIA:
   {
      break;
   }
   default:
   {
      break;
   }

   }
}

void Palettes::AddPalette(const Palette& palette)
{
   static size_t id = 0;
   m_Palettes.insert(std::make_pair(id++, palette));
}

void Palettes::GenerateColorPalette()
{
   Palette palette(std::string("All Black"));
   
   for (size_t i = 0; i < 256; i++)
   {
      Color color;
      palette.AddColor(color);
   }

   AddPalette(palette);
}

void Palettes::EvaluateColourPalettes()
{
   for (const auto& palettePair : m_Palettes)
   {
      double quality = DetermineQuality(palettePair.second);
      m_PaletteQuality.insert(std::make_pair(palettePair.first, quality));
   }
}

void Palettes::Prsize_tPalettes() const
{
   for (const auto& palettePair : m_Palettes)
   {
      palettePair.second.Prsize_t();
   }
}

void Palettes::Prsize_tPaletteQuality() const
{
   for (const auto& palettePair : m_PaletteQuality)
   {
      std::cout << "Palette: " << m_Palettes.at(palettePair.first).m_Name << std::endl;
      std::cout << "Quality: " << palettePair.second << std::endl;
   }
}

double Palettes::DetermineQuality(const Palette& palette) const
{
   double sumDistance = 0;
   for (auto color = palette.m_Colors.begin(); color != palette.m_Colors.end(); color++)
   {
      for (auto otherColor = std::next(color); otherColor != palette.m_Colors.end(); otherColor++)
      {
         sumDistance += color->Distance(*otherColor);
      }
   }

   size_t paletteSize = palette.m_Colors.size();
   double quality = sumDistance / (paletteSize * (paletteSize - 1) / 2);
   return quality;
}

};