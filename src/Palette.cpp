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
#include <algorithm>

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
   // Calculate the maximum possible color distance
   const double maxColorDistance = sqrt(pow(255, 2) * 3);

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
   if (paletteSize > 1) {
      m_Evaluation.m_AverageDistance = sumDistance / (paletteSize * (paletteSize - 1) / 2);
   }
   else {
      m_Evaluation.m_AverageDistance = 0; // or another appropriate value for palettes with 0 or 1 color
   }

   // Normalize distances
   m_Evaluation.m_MinDistance /= maxColorDistance;
   m_Evaluation.m_MaxDistance /= maxColorDistance;
   m_Evaluation.m_AverageDistance /= maxColorDistance;

   // Calculate the total evaluation
   const double weightMinDistance = 1.0; // Adjust these weights as needed
   const double weightMaxDistance = 1.0;
   const double weightAverageDistance = 1.0;

   m_Evaluation.m_TotalEvaluation = (weightMinDistance * m_Evaluation.m_MinDistance) +
      (weightMaxDistance * m_Evaluation.m_MaxDistance) +
      (weightAverageDistance * m_Evaluation.m_AverageDistance);
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

   size_t num_colors = VULCAN_PALETTE_SIZE;
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

void Palette::AppendToDrawing(size_t offsetX, size_t offsetY) const 
{
   size_t num_colors = VULCAN_PALETTE_SIZE;
   size_t colors_per_row = 16;
   size_t color_width = 20;
   size_t color_height = 20; // This can be the same as color_width or different based on preference

   glPushMatrix();
   glTranslatef((float)offsetX, (float)offsetY, 0.0f);

   for (size_t i = 0; i < num_colors; ++i) {
      size_t row = i / colors_per_row;
      size_t col = i % colors_per_row;
      float r = float(m_Colors[i].r) / 255.0;
      float g = float(m_Colors[i].g) / 255.0;
      float b = float(m_Colors[i].b) / 255.0;

      glColor3f(r, g, b);
      glRectf(col * color_width, row * color_height, (col + 1) * color_width - 2, (row + 1) * color_height - 2);
   }

   glPopMatrix();
}

// Converter
Color Converter::ConvertColor(const Color& color, const BlindnessType& type)
{
   if (type == BlindnessType::NORMAL) { return color; }

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
   double r = 0.0, g = 0.0, b = 0.0;

   if (wavelength >= 380.0 && wavelength < 440.0) 
   {
      r = -(wavelength - 440.0) / (440.0 - 380.0);
      g = 0.0;
      b = 1.0;
   }
   else if (wavelength >= 440.0 && wavelength < 490.0) 
   {
      r = 0.0;
      g = (wavelength - 440.0) / (490.0 - 440.0);
      b = 1.0;
   }
   else if (wavelength >= 490.0 && wavelength < 510.0) 
   {
      g = 1.0;
      b = -(wavelength - 510.0) / (510.0 - 490.0);
   }
   else if (wavelength >= 510.0 && wavelength < 580.0) 
   {
      r = (wavelength - 510.0) / (580.0 - 510.0);
      g = 1.0;
      b = 0.0;
   }
   else if (wavelength >= 580.0 && wavelength < 645.0) 
   {
      r = 1.0;
      g = -(wavelength - 645.0) / (645.0 - 580.0);
      b = 0.0;
   }
   else if (wavelength >= 645.0 && wavelength <= 750.0) 
   {
      r = 1.0;
      g = 0.0;
      b = 0.0;
   }
   else 
   {
      r = 0.0;
      g = 0.0;
      b = 0.0;
   }

   // Adjust intensity
   double factor = (wavelength >= 380.0 && wavelength < 420.0) ? 0.3 + 0.7 * (wavelength - 380.0) / (420.0 - 380.0) :
      (wavelength >= 645.0 && wavelength <= 750.0) ? 0.3 + 0.7 * (750.0 - wavelength) / (750.0 - 645.0) : 1.0;

   r = round(255.0 * (r * factor));
   g = round(255.0 * (g * factor));
   b = round(255.0 * (b * factor));

   return Color(r, g, b);
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
   size_t id = GenerateAllBlack();
   GenerateBlindnessPalettes(id);
   EvaluateColorPalettes(id);

   //DrawPalette(id, BlindnessType::NORMAL);

   id = GenerateDefault();
   GenerateBlindnessPalettes(id);
   EvaluateColorPalettes(id);

   //DrawPalette(id, BlindnessType::NORMAL);
   DrawPalette(id, BlindnessType::DEUTERANOMALY);

   id = GenerateVisibleSpectrum();
   GenerateBlindnessPalettes(id);
   EvaluateColorPalettes(id);

   //DrawPalette(id, BlindnessType::NORMAL);
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

void Palettes::DrawPalettes(size_t id) const
{
   auto lookup = m_Palettes.find(id);
   if (lookup == m_Palettes.end()) { return; }

   GLFWwindow* window;
   size_t color_width = 20;
   size_t color_height = 20;
   size_t colors_per_row = 16;
   size_t palette_width = colors_per_row * color_width;
   size_t palette_height = (VULCAN_PALETTE_SIZE / colors_per_row) * color_height;
   size_t space_between_palettes = 10;

   /* Initialize the library */
   if (!glfwInit()) {
      std::cerr << "Failed to initialize GLFW" << std::endl;
      return;
   }

   /* Create a windowed mode window and its OpenGL context */
   window = glfwCreateWindow(palette_width * lookup->second.size() + space_between_palettes * (lookup->second.size() - 1),
                             palette_height, "Color Palettes", NULL, NULL);
   if (!window) {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return;
   }

   /* Make the window's context current */
   glfwMakeContextCurrent(window);

   // Need to call glewInit after creating context ^
   if (glewInit() != GLEW_OK) {
      std::cerr << "Failed to initialize GLEW" << std::endl;
      glfwTerminate();
      return;
   }

   // Print the OpenGL version
   std::cout << glGetString(GL_VERSION) << std::endl;

   /* Set background color */
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   /* Disable smoothing to make the borders sharper */
   glDisable(GL_LINE_SMOOTH);

   /* Configure Projection and ModelView matrices */
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(0, 640 * lookup->second.size(), 640, 0, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();

   // Draw each palette
   size_t i = 0;
   for (const auto& palette : lookup->second)
   {
      palette.second.AppendToDrawing(palette_width * i + space_between_palettes * i, 0);
      i++;
   }

   /* Swap front and back buffers */
   glfwSwapBuffers(window);

   /* Loop until the user closes the window */
   while (!glfwWindowShouldClose(window)) {
      /* Poll for and process events */
      glfwPollEvents();
   }

   glfwTerminate();
}

// Private Palettes
size_t Palettes::AddPalette(const Palette& palette)
{
   static size_t id = 0;
   std::unordered_map<BlindnessType, Palette> map;
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

   for (size_t i = 0; i < VULCAN_PALETTE_SIZE; i++)
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

// Function to generate a random color
Color GenerateRandomColor() 
{
   // Assuming the range of each color component is 0-255
   size_t r = rand() % VULCAN_PALETTE_SIZE;
   size_t g = rand() % VULCAN_PALETTE_SIZE;
   size_t b = rand() % VULCAN_PALETTE_SIZE;
   return Color(r, g, b);
}

// Function to generate a random palette of a given size
Palette GenerateRandomPalette(const std::string& name, size_t paletteSize) 
{
   Palette palette(name);
   for (size_t i = 0; i < paletteSize; ++i) 
   {
      palette.AddColor(GenerateRandomColor());
   }
   return palette;
}

PalettesGA::PalettesGA(const BlindnessType type, const size_t size)
{
   m_Type = type;
   m_PopulationSize = size;
   m_Palettes.reserve(m_PopulationSize);

   for (size_t i = 0; i < m_PopulationSize; i++)
   {
      auto palette1 = GenerateRandomPalette("", VULCAN_PALETTE_SIZE);
      Palette palette2("");
      for (const auto& color : palette1.m_Colors)
      {
         palette2.AddColor(Converter::ConvertColor(color, m_Type));
      }
      m_Palettes.push_back(std::make_pair(palette1, palette2));
   }
}

struct HSV {
   double h, s, v;
};

HSV RGBtoHSV(const Color& color) {
   double r = color.r / 255.0;
   double g = color.g / 255.0;
   double b = color.b / 255.0;
   double cmax = std::max({ r, g, b });
   double cmin = std::min({ r, g, b });
   double diff = cmax - cmin;
   double h = -1, s = -1;

   // Hue calculation
   if (cmax == cmin) h = 0;
   else if (cmax == r) h = fmod((60 * ((g - b) / diff) + 360), 360);
   else if (cmax == g) h = fmod((60 * ((b - r) / diff) + 120), 360);
   else if (cmax == b) h = fmod((60 * ((r - g) / diff) + 240), 360);

   // Saturation calculation
   if (cmax == 0) s = 0;
   else s = (diff / cmax) * 100;

   // Value calculation
   double v = cmax * 100;

   return HSV{ h, s, v };
}

bool CompareColors(const Color& a, const Color& b) {
   HSV hsvA = RGBtoHSV(a);
   HSV hsvB = RGBtoHSV(b);
   return hsvA.h < hsvB.h;
}

void SortPaletteROYGBIV(std::vector<Color>& palette) {
   std::sort(palette.begin(), palette.end(), CompareColors);
}

void PalettesGA::RunGA(const size_t numGenerations, const double mutationRate, const double crossoverRate) 
{
   for (size_t gen = 0; gen < numGenerations; gen++)
   {
      std::cout << "Generation: " << gen << std::endl;

      // Evaluate fitness of each palette
      EvaluatePopulation();

      // Selection
      double averageDistance;
      std::vector<Palette> selectedPalettes = SelectParents(averageDistance);

      std::cout << "Average Distance : " << averageDistance << std::endl;

      // Crossover
      std::vector<Palette> newGeneration;
      while (newGeneration.size() < m_PopulationSize) 
      {
         size_t idx1 = rand() % selectedPalettes.size();
         size_t idx2 = rand() % selectedPalettes.size();
         if (static_cast<double>(rand()) / RAND_MAX < crossoverRate) 
         {
            Palette child = Crossover(selectedPalettes[idx1], selectedPalettes[idx2]);
            newGeneration.push_back(child);
         }
      }

      // Mutation
      for (auto& palette : newGeneration) 
      {
         if (static_cast<double>(rand()) / RAND_MAX < mutationRate) 
         {
            Mutate(palette, mutationRate);
         }
      }

      // Update the population with the new generation
      m_Palettes.clear();
      for (const auto palette1 : newGeneration)
      {
         Palette palette2("");
         for (const auto& color : palette1.m_Colors)
         {
            palette2.AddColor(Converter::ConvertColor(color, m_Type));
         }
         m_Palettes.push_back(std::make_pair(palette1, palette2));
      }
   }

   double averaageDistance;
   std::vector<Palette> selectedPalettes = SelectParents(averaageDistance);
   // Update the population with the new generation
   m_Palettes.clear();
   for (const auto palette1 : selectedPalettes)
   {
      Palette palette2("");
      for (const auto& color : palette1.m_Colors)
      {
         palette2.AddColor(Converter::ConvertColor(color, m_Type));
      }
      m_Palettes.push_back(std::make_pair(palette1, palette2));
   }
   auto best = m_Palettes.front().second;
   SortPaletteROYGBIV(best.m_Colors);
   best.Draw();
}

void PalettesGA::EvaluatePopulation()
{
   for (auto& palettes : m_Palettes)
   {
      palettes.second.Evaluate();
   }
}

std::vector<Palette> PalettesGA::SelectParents(double& averageDistance)
{
   // Sort the palettes based on average color distance
   std::sort(m_Palettes.begin(), m_Palettes.end(), [](const std::pair<Palette, Palette>& a, const std::pair<Palette, Palette>& b) {
      return a.second.m_Evaluation.m_TotalEvaluation > b.second.m_Evaluation.m_TotalEvaluation; });

   averageDistance = m_Palettes.front().second.m_Evaluation.m_TotalEvaluation;

   // Select the top palettes as parents
   // For simplicity, let's take the top half
   size_t numberOfParents = m_PopulationSize / 2;
   std::vector<Palette> selectedParents;

   for (size_t i = 0; i < numberOfParents; i++) 
   {
      selectedParents.push_back(m_Palettes[i].first);
   }

   return selectedParents;
}

Palette PalettesGA::Crossover(const Palette& parent1, const Palette& parent2) 
{
   // Assuming parent1 and parent2 have the same number of colors
   size_t paletteSize = parent1.m_Colors.size();

   // Choose a random crossover point
   size_t crossoverPoint = rand() % paletteSize;

   // Create a new palette with the name "Crossover"
   Palette child("");

   // Add colors from the first parent up to the crossover point
   for (size_t i = 0; i < crossoverPoint; i++) 
   {
      child.AddColor(parent1.m_Colors[i]);
   }

   // Add colors from the second parent from the crossover point to the end
   for (size_t i = crossoverPoint; i < paletteSize; i++) 
   {
      child.AddColor(parent2.m_Colors[i]);
   }

   return child;
}

void PalettesGA::Mutate(Palette& palette, const double mutationRate) 
{
   // Iterate through each color in the palette
   for (Color& color : palette.m_Colors) 
   {
      // Check if this color should be mutated
      if (static_cast<double>(rand()) / RAND_MAX < mutationRate) 
      {
         // Randomly adjust the color
         color.r = (color.r + (rand() % 51 - 25)) % 256; // Adjust R and keep within range
         color.g = (color.g + (rand() % 51 - 25)) % 256; // Adjust G and keep within range
         color.b = (color.b + (rand() % 51 - 25)) % 256; // Adjust B and keep within range

         // Ensure color values stay within the 0-255 range
         color.r = (color.r < 0) ? 0 : (color.r > 255 ? 255 : color.r);
         color.g = (color.g < 0) ? 0 : (color.g > 255 ? 255 : color.g);
         color.b = (color.b < 0) ? 0 : (color.b > 255 ? 255 : color.b);
      }
   }
}

}