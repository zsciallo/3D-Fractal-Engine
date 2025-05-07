/*
 * CSCI 4229 Final
 * Zackery Sciallo
 * Ray Marched Fractals
 * 
 *  Shaders - OpenGL2 Style
 *
 *  Key bindings:
 * r/R         Reload shader
 * c/C         Change Color mode
 * s/S         Enable Shadows
 * f/F         Enable Fog
 * g/G         Enable Glow
 *  ESC        Exit
 */
#include "CSCIx229.h"
#include <stdbool.h>
int zoom = 4;      //ZOOM TO SCREEN
int axes=1;       //  Display axes
int mode=1;       //  Shader mode
int move=1;       //  Move light
int roll=1;       //  Rolling brick texture
int proj=1;       //  Projection type
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int pi=0;         //  Pi texture
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
int zh=90;        //  Light azimuth
float Ylight=2;   //  Light elevation
int shader[]  = {0,0,0,0,0,0,0,0,0,0}; //  Shader programs
const char* text[] = {"Sphere Folds", "Sphere Garden", "Menger Sponge"};
#define MODE 10
float X=0,Y=0,Z=1; //  Mandelbrot X,Y,Z
//  Transformation matrixes
float ProjectionMatrix[16];
float ViewMatrix[16];
//  Set lighting parameters using uniforms
float Position[4];

//rendering options
bool glow = FALSE;
bool shadows = FALSE;
bool fancyColor = FALSE;
bool fog = FALSE;
int shaderMode = 2;
bool doFolds = TRUE;

//camera stuff
float cameraZ = -15.0;
float cameraAngleX = 0;

//time stuff
float mengerIsolation = -1;

/*
 * Set color
 */
void SetColor(float R,float G,float B)
{
   float color[] = {R,G,B,1.0};
   glColor3f(R,G,B);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,color);
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void canvas()
{
   //  Front
   SetColor(1,1,1);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,+1);
   glTexCoord2f(0,0); glVertex3f(-1.6*zoom,-.9*zoom,0);
   glTexCoord2f(1,0); glVertex3f(+1.6*zoom,-.9*zoom,0);
   glTexCoord2f(1,1); glVertex3f(+1.6*zoom,+.9*zoom,0);
   glTexCoord2f(0,1); glVertex3f(-1.6*zoom,+.9*zoom,0);
   glEnd();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (proj)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }
   //Use Shader
   glUseProgram(shader[mode]);
   float time = roll ? 0.001*glutGet(GLUT_ELAPSED_TIME) : 0;
   int id = glGetUniformLocation(shader[mode],"Xcenter");
   glUniform1f(id,X);
   id = glGetUniformLocation(shader[mode],"Ycenter");
   glUniform1f(id,Y);
   id = glGetUniformLocation(shader[mode],"Zoom");
   glUniform1f(id,Z);
   id = glGetUniformLocation(shader[mode],"time");
   glUniform1f(id,time);

   //  Draw the canvas
   canvas();
   glDisable(GL_TEXTURE_2D);

   //  Revert to fixed pipeline
   glUseProgram(0);

   //  Display parameters
   glColor3f(1.0, 0.0, 0.0);
   glWindowPos2i(5,5);
   Print("Shadows=%i, Glow=%i, Fog=%i, Fancy Color=%i, Mode=%s, Folds=%i",
     shadows,glow,fog,fancyColor,text[shaderMode],doFolds);
   if (mode==4)
   {
      glWindowPos2i(5,25);
      Print("X=%f Y=%f Z=%f Mag=%f",X,Y,Z,1/Z);
   }
   //glClear(GL_COLOR_BUFFER_BIT);
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Get elapsed (wall) time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/10000.0;
   //  Calculate number between - 0-2
   mengerIsolation = 0.0 - 2*sin(t);

   //apply to shader
   glUseProgram(shader[1]);
   GLint isolationLocation = glGetUniformLocation(shader[1], "MENGERTIME"); 
   glUniform1f(isolationLocation, mengerIsolation);

   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
      //CAMERA CONTROLS
   switch (key) {
      //forward for now
      case GLUT_KEY_UP:
         glUseProgram(shader[1]);
         cameraZ += .05;
         GLint zLocation = glGetUniformLocation(shader[1], "CAMERAZ"); 
         glUniform1f(zLocation, cameraZ);
         break;
      //backward
      case GLUT_KEY_DOWN:
         glUseProgram(shader[1]);
         cameraZ -= .05;
         zLocation = glGetUniformLocation(shader[1], "CAMERAZ"); 
         glUniform1f(zLocation, cameraZ);
         break;
      case GLUT_KEY_LEFT:
         glUseProgram(shader[1]);
         //cameraAngleX -= 5;
         GLint xLocation = glGetUniformLocation(shader[1], "PLAYERVIEWX"); 
         glUniform1f(xLocation, cameraAngleX);
         break;
      case GLUT_KEY_RIGHT:
         glUseProgram(shader[1]);
         //cameraAngleX += 5;
         xLocation = glGetUniformLocation(shader[1], "PLAYERVIEWX"); 
         glUniform1f(xLocation, cameraAngleX);
         break;
      default:
         break;
   }
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project(proj?fov:0,asp,dim);
}

/*
 *  Read text file
 */
char* ReadText(char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal("Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal("Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal("Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj,char* file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file,buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal("Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal("Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal("Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type,char* file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file);
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file);
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(char* VertFile,char* FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER,VertFile);
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER,FragFile);
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}
/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //reload shader
   else if (ch == 'r' || ch=='R')
   {
     shader[1] = CreateShaderProg("model.vert","raymarch.frag");
   }
   //toggle glow
   if (ch == 'g' || ch == 'G'){
      glUseProgram(shader[1]);
      GLint glowLocation = glGetUniformLocation(shader[1], "GLOWENABLED");
      glow = !glow;
      glUniform1i(glowLocation, glow);
   }
   //toggle shadows
   if (ch == 's' || ch == 'S'){
      glUseProgram(shader[1]);
      GLint shadowLocation = glGetUniformLocation(shader[1], "SHADOWSENABLED");
      shadows = !shadows;
      glUniform1i(shadowLocation, shadows);
   }
   //toggle color
   if (ch == 'c' || ch == 'C'){
      glUseProgram(shader[1]);
      GLint colorLocation = glGetUniformLocation(shader[1], "FANCYCOLOR");
      fancyColor = !fancyColor;
      glUniform1i(colorLocation, fancyColor);
   }
   //toggle fog
   if (ch == 'f' || ch == 'F'){
      glUseProgram(shader[1]);
      GLint fogLocation = glGetUniformLocation(shader[1], "FOGENABLED");
      fog = !fog;
      glUniform1i(fogLocation, fog);
   }
   //toggle folds
   if (ch == 'q' || ch == 'Q'){
      glUseProgram(shader[1]);
      GLint foldLocation = glGetUniformLocation(shader[1], "DOFOLDS");
      doFolds = !doFolds;
      glUniform1i(foldLocation, doFolds);
   }
   //change shader mode
   if (ch == 't' || ch == 'T'){
      glUseProgram(shader[1]);
      GLint modeLocation = glGetUniformLocation(shader[1], "MODE");
      shaderMode++;
      shaderMode = shaderMode % 3;
      glUniform1i(modeLocation, shaderMode);
   }

   //  Reproject
   Project(proj?fov:0,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}
/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(1600,900);
   glutCreateWindow("Ray Marched Fractals - Zackery Sciallo");
#ifdef USEGLEW
   //  Initialize GLEW
   if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
#endif
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Load object
   //  Create Shader Programs
   shader[1] = CreateShaderProg("model.vert","raymarch.frag");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
