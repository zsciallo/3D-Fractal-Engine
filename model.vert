#version 120
//output to frag
varying vec2 UVCoords;



void main()
{
   //set UV coords
   UVCoords = gl_Vertex.xy * 0.5 + 0.5;

   //  Return fixed transform coordinates for this vertex
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
