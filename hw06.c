/*
 *  Image processing
 *
 *  Key bindings:
 *  -          Increase # of passes
 *  +/=        Increase # of passes
 *  m          Toggle shader
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include "CSCIx239.h"
int mode=0;    //  Shader
int N=1;       //  Number of passes
int th=0,ph=0; //  View angles
int fov=57;    //  Field of view (for perspective)
float asp=1;   //  Aspect ratio
float dim=3;   //  Size of world
int obj;       //  Cruiser object
int toon;      //  Toon shader
unsigned int depthbuf=0;  //  Depth buffer
unsigned int img[2];      //  Image textures
unsigned int framebuf[2]; //  Frame buffers
#define MODE 10
int shader[MODE] = {0};   //  Shader programs
const char* text[] = {"No Shader","Contrast","Color Swap","Invert","Greyscale (Average)","Greyscale (Luma)","Red Filter","Green Filter","Blue Filter","Image Shift"};

//
//  Refresh display
//
void display(GLFWwindow* window)
{
   //  Send all output to frame buffer 0
   if (mode) glBindFramebuffer(GL_FRAMEBUFFER,framebuf[0]);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set projection and view
   Projection(fov,asp,dim);
   View(th,ph,fov,dim);


   //  Cube
   Cube(-1,1,0 , 0.5,0.5,0.5 , 0,0 , 0);
   //  Teapot
   glUseProgram(toon);
   float zh = fmod(90*glfwGetTime(),360);
   float lpos[4] = {2*Cos(zh),2,2*Sin(zh),1};
   glLightfv(GL_LIGHT0,GL_POSITION,lpos);
   Teapot(1,1,0 , 0.5 , 90,0 , 4,0);
   glUseProgram(0);
   //  Icosahedron
   Icosahedron(-1,-1,0,0.7 , 0,0 , 0);
   //  Cruiser
   glPushMatrix();
   SetColor(1,1,1);
   glTranslated(1,-1,0);
   glScaled(0.5,0.5,0.5);
   glCallList(obj);
   glPopMatrix();

   //  Draw axes using fixed pipeline (white)
   Axes(2);

   //  Ping-Pong between framebuffers
   if (mode)
   {
      //  Enable shader
      glUseProgram(shader[mode]);
      //  Set screen resolution uniforms
      int id,width,height;
      glfwGetWindowSize(window,&width,&height);
      id = glGetUniformLocation(shader[mode],"dX");
      glUniform1f(id,1.0/width);
      id = glGetUniformLocation(shader[mode],"dY");
      glUniform1f(id,1.0/height);
      //  Identity projection
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      //  Disable depth test & Enable textures
      glDisable(GL_DEPTH_TEST);
      glEnable(GL_TEXTURE_2D);
      //  Copy entire screen
      for (int i=0;i<N;i++)
      {
         //  Output to alternate framebuffers
         //  Final output is to screen
         glBindFramebuffer(GL_FRAMEBUFFER,i==N-1?0:framebuf[(i+1)%2]);
         //  Clear the screen
         glClear(GL_COLOR_BUFFER_BIT);
         //  Input image is from the last framebuffer
         glBindTexture(GL_TEXTURE_2D,img[i%2]);
         //  Redraw the screen
         glBegin(GL_QUADS);
         glTexCoord2f(0,0); glVertex2f(-1,-1);
         glTexCoord2f(0,1); glVertex2f(-1,+1);
         glTexCoord2f(1,1); glVertex2f(+1,+1);
         glTexCoord2f(1,0); glVertex2f(+1,-1);
         glEnd();
      }
      //  Disable textures and shaders
      glDisable(GL_TEXTURE_2D);
      glUseProgram(0);
   }

   //  Display parameters
   SetColor(1,1,1);
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Projection=%s Mode=%s Passes=%d",th,ph,dim,fov>0?"Perpective":"Orthogonal",text[mode],N);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glfwSwapBuffers(window);
}

//
//  Key pressed callback
//
void key(GLFWwindow* window,int key,int scancode,int action,int mods)
{
   //  Discard key releases (keeps PRESS and REPEAT)
   if (action==GLFW_RELEASE) return;

   //  Check for shift
   int shift = (mods & GLFW_MOD_SHIFT);

   //  Exit on ESC
   if (key == GLFW_KEY_ESCAPE)
     glfwSetWindowShouldClose(window,1);
   //  Reset view angle
   else if (key==GLFW_KEY_0)
      th = ph = 0;
   //  Switch shaders
   else if (key==GLFW_KEY_M)
      mode = shift ? (mode+MODE-1)%MODE : (mode+1)%MODE;
   //  Number of passes
   else if ((key==GLFW_KEY_KP_SUBTRACT || key==GLFW_KEY_MINUS) && N>1)
      N --;
   else if (key==GLFW_KEY_KP_ADD || key==GLFW_KEY_EQUAL)
      N++;
   //  Switch between perspective/orthogonal
   else if (key==GLFW_KEY_P)
      fov = fov ? 0 : 57;
   //  Increase/decrease asimuth
   else if (key==GLFW_KEY_RIGHT)
      th += 5;
   else if (key==GLFW_KEY_LEFT)
      th -= 5;
   //  Increase/decrease elevation
   else if (key==GLFW_KEY_UP)
      ph += 5;
   else if (key==GLFW_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key==GLFW_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key==GLFW_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Wrap angles
   th %= 360;
   ph %= 360;
}

//
//  Window resized callback
//
void reshape(GLFWwindow* window,int width,int height)
{
   //  Get framebuffer dimensions (makes Apple work right)
   glfwGetFramebufferSize(window,&width,&height);
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //
   //  Allocate a frame buffer
   //  Typically the same size as the screen (W,H) but can be larger or smaller
   //
   //  Delete old frame buffer, depth buffer and texture
   if (depthbuf)
   {
      glDeleteRenderbuffers(1,&depthbuf);
      glDeleteTextures(2,img);
      glDeleteFramebuffers(2,framebuf);
   }
   //  Allocate two textures, two frame buffer objects and a depth buffer
   glGenFramebuffers(2,framebuf);   
   glGenTextures(2,img);
   glGenRenderbuffers(1,&depthbuf);   
   //  Allocate and size texture
   for (int k=0;k<2;k++)
   {
      glBindTexture(GL_TEXTURE_2D,img[k]);
      glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,NULL);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
      //  Bind frame buffer to texture
      glBindFramebuffer(GL_FRAMEBUFFER,framebuf[k]);
      glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,img[k],0);
      //  Bind depth buffer to frame buffer 0
      if (k==0)
      {
         glBindRenderbuffer(GL_RENDERBUFFER,depthbuf);
         glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT24,width,height);
         glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthbuf);
      }
   }
   //  Switch back to regular display buffer
   glBindFramebuffer(GL_FRAMEBUFFER,0);
   ErrCheck("Framebuffer");
}

//
//  Main program with GLFW event loop
//
int main(int argc,char* argv[])
{
   //  Initialize GLFW
   GLFWwindow* window = InitWindow("Image Processing",1,600,600,&reshape,&key);

   //  Load shaders
   shader[1] = CreateShaderProg(NULL,"contrast.frag");
   shader[2] = CreateShaderProg(NULL,"swap.frag");
   shader[3] = CreateShaderProg(NULL,"inverse.frag");
   shader[4] = CreateShaderProg(NULL,"bwavg.frag");
   shader[5] = CreateShaderProg(NULL,"bwlum.frag");
   shader[6] = CreateShaderProg(NULL,"red.frag");
   shader[7] = CreateShaderProg(NULL,"green.frag");
   shader[8] = CreateShaderProg(NULL,"blue.frag");
   shader[9] = CreateShaderProg(NULL,"shift.frag");
   //  Toon shader for teapot
   toon = CreateShaderProg("toon.vert","toon.frag");
   //  Load object
   obj = LoadOBJ("cruiser.obj");

   //  Event loop
   ErrCheck("init");
   while(!glfwWindowShouldClose(window))
   {
      //  Display
      display(window);
      //  Process any events
      glfwPollEvents();
   }
   //  Shut down GLFW
   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}
