#include "Palette.h"
#include "Data.h"

// need to include glew.h before any other opengl
// need to define GLEW_STATIC
#include "GLEW/glew.h" 
#include "GLFW/glfw3.h"

#include <type_traits>
#include <limits>
#include <iostream>
#include <cstdlib>
#include <cfloat>

template<typename T>
constexpr size_t size(const T&) noexcept {
    return static_cast<std::size_t>(std::numeric_limits<T>::max()) + 1;
}

namespace color {

// Color
double Color::Distance(const Color& other) const
{
   double distance = sqrt(pow(abs(int(r - other.r)), 2) + pow(abs(int(g - other.g)), 2) + pow(abs(int(b - other.b)), 2));
   return distance;
}
void Color::Print() const
{
   std::cout << "(" << r << ", " << g << ", " << b << ")";
}

// Palette
void Palette::Print(bool colors) const
{
   std::cout << "Palette: " << m_Name << std::endl;
   m_Evaluation.Print();
   if (colors)
   {
      for (const auto& color : m_Colors)
      {
         color.Print();
         std::cout << std::endl;
      }
   }
}
void Palette::Evaluate()
{
   m_Evaluation.m_MinDistance = DBL_MAX;
   m_Evaluation.m_MaxDistance = 0;
   m_Evaluation.m_ColorRepresentation = 0;

   double sumDistance = 0;
   for (auto color = m_Colors.begin(); color != m_Colors.end(); color++)
   {
      for (auto otherColor = std::next(color); otherColor != m_Colors.end(); otherColor++)
      {
         double distance = color->Distance(*otherColor);
         sumDistance += distance;
         if (distance < m_Evaluation.m_MinDistance) { m_Evaluation.m_MinDistance = distance; }
         if (distance > m_Evaluation.m_MaxDistance) { m_Evaluation.m_MaxDistance = distance; }
      }
   }

   size_t paletteSize = m_Colors.size();
   m_Evaluation.m_AverageDistance = sumDistance / (paletteSize * (paletteSize - 1) / 2);
}
void Palette::Draw() const
{
   GLFWwindow* window;

   /* Initialize the library */
   if (!glfwInit())
      return;

   /* Create a windowed mode window and its OpenGL context */
   window = glfwCreateWindow(640, 640, "Color Palette", NULL, NULL);
   if (!window)
   {
      glfwTerminate();
      return;
   }

   /* Make the window's context current */
   glfwMakeContextCurrent(window);

   // Need to call glewInit after creating context ^
   if (glewInit() != GLEW_OK)
      std::cout << "Error" << std::endl;

   // Print the opengl version
   std::cout << glGetString(GL_VERSION) << std::endl;

   /* Set background color */
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   /* Disable smoothing to make the borders sharper */
   glDisable(GL_LINE_SMOOTH);

   /* Draw the color palette */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 640, 640, 0, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(32.0f, 32.0f, 0.0f);

   size_t num_colors = 256;
   size_t colors_per_row = 16;
   size_t color_size = 32;
   for (size_t i = 0; i < num_colors; ++i) {
      size_t row = i / colors_per_row;
      size_t col = i % colors_per_row;
      float r = float(m_Colors[i].r) / 255.0;
      float g = float(m_Colors[i].g) / 255.0;
      float b = float(m_Colors[i].b) / 255.0;
      glColor3f(r, g, b);
      glRectf(col * color_size, row * color_size, (col + 1) * color_size - 2, (row + 1) * color_size - 2);
   }

   /* Swap front and back buffers */
   glfwSwapBuffers(window);

   /* Loop until the user closes the window */
   while (!glfwWindowShouldClose(window))
   {
      /* Poll for and process events */
      glfwPollEvents();
   }

   glfwTerminate();
}

// Converter
Color Converter::ConvertColor(const Color& color, const BlindnessType& type)
{
   double linearR, linearG, linearB;
   Converter::StandardToLinear(color, &linearR, &linearG, &linearB);
   double convertedLinearR, convertedLinearG, convertedLinearB;

   switch (type)
   {
   case BlindnessType::PROTANOPIA:
   {
      Converter::ApplyMatrix(PROTANOPIA_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);
      break;
   }
   case BlindnessType::DEUTERANOPIA:
   {
      Converter::ApplyMatrix(DEUTERANOPIA_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);
      break;
   }
   case BlindnessType::TRITANOPIA:
   {
      Converter::ApplyMatrix(TRITANOPIA_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);
      break;
   }
   case BlindnessType::DEUTERANOMALY:
   {
      Converter::ApplyMatrix(DEUTERANOMALY_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);      
      break;
   }
   case BlindnessType::PROTANOMALY:
   {
      Converter::ApplyMatrix(PROTANOMALY_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);      
      break;
   }
   case BlindnessType::TRITANOMALY:
   {
      Converter::ApplyMatrix(TRITANOMALY_MATRIX, linearR, linearG, linearB,
                             &convertedLinearR, &convertedLinearG, &convertedLinearB);      
      break;
   }
   
   default:
   {
      convertedLinearR = linearR;
      convertedLinearG = linearG;
      convertedLinearB = linearB;
      break;
   }
   }

   Color convert;
   Converter::LinearToStandard(convertedLinearR, convertedLinearG, convertedLinearB, &convert);

   return convert;
}
void Converter::StandardToLinear(const Color& standard, double* r, double* g, double* b)
{
   // Convert RGB values to the 0-1 range
   double R_ = standard.r / 255.0;
   double G_ = standard.g / 255.0;
   double B_ = standard.b / 255.0;

   // Apply gamma correction to the RGB values
   double gamma = 2.2;
   R_ = pow(R_, gamma);
   G_ = pow(G_, gamma);
   B_ = pow(B_, gamma);

   // Convert gamma-corrected RGB values to linear RGB values
   double a = 0.055;
   if (R_ <= 0.04045) { *r = R_ / 12.92; } 
   else { *r = pow((R_ + a) / (1.0 + a), 2.4); }

   if (G_ <= 0.04045) { *g = G_ / 12.92; } 
   else { *g = pow((G_ + a) / (1.0 + a), 2.4); }

   if (B_ <= 0.04045) { *b = B_ / 12.92; } 
   else { *b = pow((B_ + a) / (1.0 + a), 2.4); }
}
void Converter::LinearToStandard(const double& r, const double& g, const double& b, Color* standard)
{
   // Convert linear RGB values to sRGB values
   double a = 0.055;
   if (r <= 0.0031308) {
      standard->r = round(255.0 * r * 12.92);
   } else {
      standard->r = round(255.0 * ((1.0 + a) * pow(r, 1.0 / 2.4) - a));
   }
   if (g <= 0.0031308) {
      standard->g = round(255.0 * g * 12.92);
   } else {
      standard->g = round(255.0 * ((1.0 + a) * pow(g, 1.0 / 2.4) - a));
   }
   if (b <= 0.0031308) {
      standard->b = round(255.0 * b * 12.92);
   } else {
      standard->b = round(255.0 * ((1.0 + a) * pow(b, 1.0 / 2.4) - a));
   }
}
Color Converter::WavelengthToStandard(double wavelength)
{
   wavelength = wavelength / 1000000;

   double x = (0.37 * wavelength) / ((0.37 * wavelength) + 0.63);
   double y = (0.20 * wavelength) / ((0.20 * wavelength) + 0.80);
   double z = 1 - x - y;

   // Y is luminance factor
   double Y = 1;
   double X = Y / y * x;
   double Z = Y / y * z;

   double r, g, b;
   ApplyMatrix(TRISTIMULUS_MATRIX, X, Y, Z, &r, &g, &b);

   r *= 255;
   g *= 255;
   b *= 255;
   r = std::round(r);
   g = std::round(g);
   b = std::round(b);
   if (r > 255) { r = 255; }
   if (g > 255) { g = 255; }
   if (b > 255) { b = 255; }
   if (r < 0) { r = 0; }
   if (g < 0) { g = 0; }
   if (b < 0) { b = 0; }

   Color color((size_t)r, (size_t)g, (size_t)b);
   return color;
}
void Converter::ApplyMatrix(const double matrix[], const double& r, const double g, const double b, 
                            double* convertedR, double* convertedG, double* convertedB)
{
   *convertedR = matrix[0]*r + matrix[1]*g + matrix[2]*b;
   *convertedG = matrix[3]*r + matrix[4]*g + matrix[5]*b;
   *convertedB = matrix[6]*r + matrix[7]*g + matrix[8]*b;
}

// Public Palettes
void Palettes::GenerateColorPalette()
{
   // Generate NORMAL palette and add to m_Palettes
   //size_t id = GenerateAllBlack();
   //size_t id = GenerateDefault();
   size_t id = GenerateVisibleSpectrum();

   // Generate all BlindnessType palettes from NORMAL palette
   GenerateBlindnessPalettes(id);
   // Evaluate all BlindnessType palettes at id and store in m_PaletteQuality
   EvaluateColorPalettes(id);

   DrawPalette(id, BlindnessType::NORMAL);
   //DrawPalette(id, BlindnessType::DEUTERANOMALY);
   //DrawPalette(id, BlindnessType::DEUTERANOPIA);
   //DrawPalette(id, BlindnessType::TRITANOMALY);
   //DrawPalette(id, BlindnessType::TRITANOPIA);
   //DrawPalette(id, BlindnessType::PROTANOPIA);
   //DrawPalette(id, BlindnessType::PROTANOMALY);
}
void Palettes::PrintPalettes() const
{
   for (const auto& [id, paletteMap]: m_Palettes)
   {
      for (const auto& [type, palette] : paletteMap)
      {
         palette.Print();
      }
   }
}
void Palettes::PrintPaletteQuality() const
{
   for (const auto& [id, paletteMap] : m_Palettes)
   {
      std::cout << "************************" << std::endl;
      for (const auto& [type, palette] : paletteMap)
      {
         palette.Print();
      }
      std::cout << "************************" << std::endl;
   }
}
void Palettes::DrawPalette(size_t id, BlindnessType type) const
{
   auto lookup = m_Palettes.find(id);
   if (lookup == m_Palettes.end()) { return; }
   auto lookup2 = lookup->second.find(type);
   if (lookup2 == lookup->second.end()) { return; }

   lookup2->second.Draw();
}

// Private Palettes
size_t Palettes::AddPalette(const Palette& palette)
{
   static size_t id = 0;
   std::map<BlindnessType, Palette> map;
   map.insert({BlindnessType::NORMAL, palette});
   m_Palettes.insert({id, map});
   return id++;
}
void Palettes::GenerateBlindnessPalettes(const size_t id)
{
   auto lookup = m_Palettes.find(id);
   if (lookup == m_Palettes.end())
   {
      std::cout << "Failed to find palette from ID provided" << std::endl;
      return;
   }

   auto lookup2 = lookup->second.find(BlindnessType::NORMAL);
   if (lookup2 == lookup->second.end())
   {
      std::cout << "Failed to find palette from ID provided" << std::endl;
      return;
   }

   auto palette = lookup2->second;
   const size_t NUM_VALUES = static_cast<size_t>(BlindnessType::LAST);

   for (size_t i = 0; i < NUM_VALUES; i++) 
   {
      BlindnessType type = static_cast<BlindnessType>(i);
      Palette blindPalette(BlindnessTypeNames.at(type));

      for (const auto& color : palette.m_Colors)
      {
         blindPalette.AddColor(Converter::ConvertColor(color, type));
      }

      lookup->second.insert({type, blindPalette});
   }
}
void Palettes::EvaluateColorPalettes(const size_t id)
{
   auto lookup = m_Palettes.find(id);
   if (lookup == m_Palettes.end())
   {
      std::cout << "Failed to find palette from id provided" << std::endl; 
      return;
   }

   for (auto& [type, palette] : lookup->second)
   {
      palette.Evaluate();
   }
}
size_t Palettes::GenerateAllBlack()
{
   Palette palette(std::string("All Black"));

   for (size_t i = 0; i < 256; i++)
   {
      Color color;
      palette.AddColor(color);
   }

   return AddPalette(palette);
}
size_t Palettes::GenerateDefault()
{
   return AddPalette(PaletteData::DefaultPalette());
}
size_t Palettes::GenerateVisibleSpectrum()
{
   return AddPalette(PaletteData::VisibleSpectrumPalette());
}

}