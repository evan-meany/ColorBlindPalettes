#include <iostream>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace color
{

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
   void Print() const;
   std::vector<Color> m_Colors;
   std::string m_Name;
};

class Palettes
{
public:
   void AddPalette(const Palette& palette);
   void GenerateColorPalette();
   void EvaluateColourPalettes();
   void PrintPalettes() const;
   void PrintPaletteQuality() const;

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

void Color::Print() const
{
   std::cout << "(" << r << ", " << g << ", " << b << ")";
}

void Palette::Print() const
{
   std::cout << "Palette: " << m_Name << std::endl;
   for (const auto& color : m_Colors)
   {
      color.Print();
      std::cout << std::endl;
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

void Palettes::PrintPalettes() const
{
   for (const auto& palettePair : m_Palettes)
   {
      palettePair.second.Print();
   }
}

void Palettes::PrintPaletteQuality() const
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