#include <iostream>
#include "Palette.h"

#include "GLEW/glew.h" 
#include "GLFW/glfw3.h"

static int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{

}

static void Testing()
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

   // Define buffer and fill with data
   unsigned int buffer; // id of buffer
   glGenBuffers(1, &buffer);
   glBindBuffer(GL_ARRAY_BUFFER, buffer); // select or 'bind' buffer
   float positions[6] = { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f };
   glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

   // index, size, type, normalize?, stride, pointer
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
   glEnableVertexAttribArray(0);

   while (!glfwWindowShouldClose(window))
   {
      // clear screen
      glClear(GL_COLOR_BUFFER_BIT);

      // draw triangle
      glDrawArrays(GL_TRIANGLES, 0, 3);

      glfwSwapBuffers(window);

      /* Poll for and process events */
      glfwPollEvents();
   }

   glfwTerminate();
}

int main(int argc, char* argv[])
{
   color::Palettes palettes;
   palettes.GenerateColorPalette();
   palettes.PrintPaletteQuality();

   //Testing();

   return 0;
}
