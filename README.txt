/*
* CSCI 4229 Final
* Zackery Sciallo
* Ray Marched Fractals
* 
*  Shaders - OpenGL2 Style
*
*  Key bindings:
* r/R         Reload shader	(don't use)
* c/C         Change Color mode
* s/S         Enable Shadows
* f/F         Enable Fog
* g/G         Enable Glow
* ESC	      Exit
* Up/Down     Move through scene
* 
*RECOMMENDED SETTINGS TO MAKE PRETTY SCENES: 
*MENGER SPONGE1: Fog, Glow, Folds 
*MENGER SPONGE2: Fog, Fancy Color, Shadows 
*
*The following functions were borrowed from other authors. The primary purpose of these functions is to fold space.
*While working on this project, I was unfamiliar with how these transformations worked hence the credit to these authors.
*Distance to menger is a particuluarly interesting function - at just 8 folds in space we are already rendering nearly 1 million cubes. 
/***************************************************************************************
*    Title: sierPinskiFold, rotX, rotY
*    Author: HackerPoet
*    Date: 2019
*    Code version: 9a10529
*    Availability: https://github.com/HackerPoet/PySpace/blob/master/pyspace/frag.glsl
*    Location in code: raymarch.frag line 59
***************************************************************************************/
/***************************************************************************************
*    Title: DE (Distance to Meneger)
*    Author: ShroomLab
*    Date: 2020
*    Code version: 0211f67
*    Availability: https://github.com/Shroomisnottaken/3dRaymarcherFramework/blob/master/RaymarchingGLSL/RaymarchingGLSL/GLSLfragmentShader.txt
*    Location in code: raymarch.frag line 100
*    distance_from_box(point,vec3) is my own function, as well as the coloring logic from 117-124
***************************************************************************************/
*/