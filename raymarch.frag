//Frag shader implimentation of raymarching GLSL 1.2
//Zackery Sciallo
//CSCI 4229-001 Computer Graphics
//Fall 2023
#version 120

//import from vertex shader
varying vec2 UVCoords; //UV coordinates
vec2 u_resolution;

//fog vars
float FOG_SCALE = 16;
//glow code
#define MIN_GLOW .4
vec3 BG_COLOR = vec3(.3,.05,.35);
//i like this color
//#define BG_COLOR vec3(.6,.1,.7)

//shader options
uniform bool GLOWENABLED = false;
uniform bool SHADOWSENABLED = false;
uniform int MODE = 2;
uniform bool FANCYCOLOR = false;
uniform bool FOGENABLED = false;
uniform bool DOFOLDS = true;
//camera
vec3 PLAYERVIEW = vec3(0.0);
uniform float PLAYERVIEWX = 0;


int NUMBER_OF_STEPS = 100;

//CAMERA CONTROLS
uniform float CAMERAZ = -15;
//Camera Postion
vec3 cameraPosition = vec3(0.0, 0.0, CAMERAZ);
vec3 lightPosition = vec3(200.0, -500.0, 300.0);

//other vars
#define PI2 6.28318531

//Bad coding but best way to do this rn
//orbit trapping color
vec3 MENGER_COLOR = vec3(0.0);

//TIME DEPENDANT
uniform float MENGERTIME = -1.0;

//------------------------------------------
//          folding functions
//------------------------------------------
void absFold(inout vec3 vector, vec3 folding_point) {
    vector = abs(vector - folding_point) + folding_point;
}

/***************************************************************************************
*    Title: sierPinskiFold, rotX, rotY
*    Author: HackerPoet
*    Date: 2019
*    Code version: 9a10529
*    Availability: https://github.com/HackerPoet/PySpace/blob/master/pyspace/frag.glsl
*
***************************************************************************************/
void sierpinskiFold(inout vec3 vector) {
	vector.xy -= min(vector.x + vector.y, 0.0);
	vector.xz -= min(vector.x + vector.z, 0.0);
	vector.yz -= min(vector.y + vector.z, 0.0);
}
void rotX(inout vec3 z, float s, float c) {
	z.yz = vec2(c*z.y + s*z.z, c*z.z - s*z.y);
}
void rotZ(inout vec3 z, float s, float c) {
	z.xy = vec2(c*z.x + s*z.y, c*z.y - s*z.x);
}
//------------------------------------------
//          distance estimators
//------------------------------------------
//distance to sphere
float distance_from_sphere(vec3 point, vec3 center, float radius)
{
    //normal sphere calculation
    return length(point - center) - radius;
    //infinite sphere calculation
    //return length(mod(point, 1) - center) - radius;
	
}
float distance_from_sphere_inf(vec3 pos) {
    // translate
    //pos = pos + 1. * vec3(0,-0.5*iTime,iTime);
    return (distance(mod(pos, 4.), vec3(2,2,2))-.54321);
}
float distance_from_box(vec3 point, vec3 len)
{
  vec3 q = abs(point) - len;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}
/***************************************************************************************
*    Title: DE (Distance to Meneger)
*    Author: ShroomLab
*    Date: 2020
*    Code version: 0211f67
*    Availability: https://github.com/Shroomisnottaken/3dRaymarcherFramework/blob/master/RaymarchingGLSL/RaymarchingGLSL/GLSLfragmentShader.txt
*
***************************************************************************************/
float distance_to_menger(vec3 p)
{
   float distance = distance_from_box(p,vec3(1.0));
    //fold
   float s = 2.67;
   for( int m=0; m<8; m++ )
   {
      vec3 a = mod( p*s, 2.0 )-1.0;
      s *= 3.0;
      vec3 r = abs(1.0 - 3.0*abs(a));

      float da = max(r.x,r.y);
      float db = max(r.y,r.z);
      float dc = max(r.z,r.x);
      float c = (min(da,min(db,dc))-1.0)/s;

      distance = max(distance,c);

        //coloring
        if (distance > c){
            MENGER_COLOR = vec3( distance, 0.6*da*db*dc, (1.0+float(m))/4.0);
        }
        else{
            MENGER_COLOR  = vec3(distance_from_box(p,vec3(1.0)), 1.0, 0.0);
        }
   }
   return distance;
}

//raymarch main loop helpers
//world map (essentially all objects go here)
float world_map(vec3 point)
{
    NUMBER_OF_STEPS = 100;
    //Basic Rendering  -------------------------------------------------------------------------------------
    FOG_SCALE = 16;
    if (MODE == 0){
        if(DOFOLDS){
            //try to fold some space
            sierpinskiFold(point);
            absFold(point, vec3(-1.0));
        }
        return min(distance_from_sphere(point, vec3(0.0), 3), distance_from_sphere(point, vec3(2,4,0), 2));
    }
    //infinite sphere --------------------------------------------------------------------------------------
    if (MODE == 1){
        FOG_SCALE = 128;
        return distance_from_sphere_inf(vec3(point));
        //(sin(5.0 * point.x) * sin(5.0 * point.y) * sin(5.0 * point.z) * 0.25); //a little bit of distortion
    }
    //Menger Sponge ----------------------------------------------------------------------------------------
    //needs more steps than sphere renderings do
    NUMBER_OF_STEPS = 1000;
    if (MODE == 2){
        FOG_SCALE = 12;
        if(DOFOLDS){
            //want this fold number to be between -1.0- -2.0 for cool results
            absFold(point, vec3(MENGERTIME/2,MENGERTIME/4,MENGERTIME/2));
            rotX(point, .2, MENGERTIME*.1);
            absFold(point, vec3(MENGERTIME/8,MENGERTIME,MENGERTIME/8));
            rotZ(point, MENGERTIME/8, .4);
        }
        //return dist
        return distance_to_menger(point);
    }
    //no mode
    return 0;
}

    
//calculate normals (might need work later) NEED TO FIX
vec3 calculate_normal(vec3 point){
    //abitrary, need like .0001 for level of detail
    const float small_step =0.0001;
    float gradientX = world_map(vec3(point.x + small_step, point.y, point.z)) - world_map(vec3(point.x - small_step, point.y, point.z));
    float gradientY = world_map(vec3(point.x, point.y + small_step, point.z)) - world_map(vec3(point.x, point.y - small_step, point.z));
    float gradientZ = world_map(vec3(point.x, point.y, point.z + small_step)) - world_map(vec3(point.x, point.y, point.z - small_step));

    return normalize(vec3(gradientX, gradientY, gradientZ));
}
vec3 shade_soft(vec3 currentPosition){
    vec3 normal = calculate_normal(currentPosition);
    vec3 direction_to_light = normalize(currentPosition - lightPosition);
    //return solid color for now + diffuse intensity from light
    float diffuseIntensity = max(0.1, dot(normal, direction_to_light));
    //change this to be easy to change color
    return vec3(1.0, 1.0, 1.0) * diffuseIntensity;
}

//RAYMARCH MAIN LOOPdistance_from_sphere
vec3 ray_march(vec3 cameraPosition, vec3 rd)
{
    //fog stuff
    vec3 MOD_BG_COLOR = vec3(BG_COLOR.x *1 * 2,BG_COLOR.y * 1/2,BG_COLOR.z * 1);
    float total_distance_traveled = 0.0;

    //main rendering loop params
    const float MINIMUM_HIT_DISTANCE = 0.0001;
    const float MAXIMUM_TRACE_DISTANCE = 10000.0;

    //glow code - keep track of how close we ever got, and if we never hit the object apply a color based on this
    float closest_distance = 1;

    //for orbitraping coloring
    float sumZ = 0;

    for (int i = 0; i < NUMBER_OF_STEPS; ++i)
    {
        vec3 currentPosition = cameraPosition + total_distance_traveled * rd;


        //eventually this will look at the 'closest object to itself and then safely move'
        float distance_to_closest = world_map(currentPosition);

        //glow calculations
        if(distance_to_closest < closest_distance){closest_distance = distance_to_closest;}

        if (distance_to_closest < MINIMUM_HIT_DISTANCE) 
        {
            //ADD EFFECTS TO SHAPES

            vec3 out_col = vec3(1.0);
            //COOL XYZ COORD COLORING, DONT CHANGE
            if(FANCYCOLOR){
                // float SCALE = 10;
                // //R
                // float rCol = abs(clamp(currentPosition.x * SCALE, 0, 1)); // the number of steps taken
                // //G
                // float gCol = abs(clamp(currentPosition.y * SCALE, 0, 1));
                // //B
                // float bCol = abs(clamp(currentPosition.z * SCALE, 0, 1));
                // out_col = out_col + vec3(rCol,gCol,bCol)*.4;

                out_col = mix(vec3(0.0), vec3(0.4*abs(currentPosition.x+ currentPosition.y), 0.4*abs(currentPosition.y), 0.4*abs(currentPosition.z)), .5);

                //out_col = out_col * (vec3(0.2*abs(currentPosition.x+ currentPosition.y), 0.2*abs(currentPosition.y), 0.2*abs(currentPosition.z))*.5);

                //out_col = vec3(MENGER_COLOR.y,0,0);
            }
            //Ambient Oclusion
            out_col = out_col - vec3((float(i)/float(NUMBER_OF_STEPS)));
            //orbit trapping color
            //normalize distance
            // float orbit_distance = distance(currentPosition,TRANSFORMEDPOINT);
            // float normalized_orbit = (orbit_distance-0)/(1000-0);
            // out_col = vec3(1.0,1.0,normalized_orbit);

            //shade soft (light)
            if (SHADOWSENABLED){
                //out_col = out_col * shade_soft(currentPosition);
                out_col = mix(out_col, shade_soft(currentPosition),.5);
            }
            //GLOW
            if (GLOWENABLED){
                //float glow_value = max(MIN_GLOW, 1.0 - closest_distance/2);
                //out_col = out_col + vec3(0,BG_COLOR.y,1 - BG_COLOR.z*glow_value);
            }
            //FOG (basically just mix bg color in based on distance)
            if(FOGENABLED){
                float fog_ratio = total_distance_traveled/FOG_SCALE;
                out_col = mix(MOD_BG_COLOR, out_col, clamp(1.0-fog_ratio,0,1));
            }
            //RETURN FINAL COLOR OF SHAPES
            return out_col;
            
        }

        if (total_distance_traveled > MAXIMUM_TRACE_DISTANCE)
        {
            break;
        }
        total_distance_traveled += distance_to_closest;
    }
       //didn't hit anything
    if (GLOWENABLED){
        //glow code
        float glow_value = max(MIN_GLOW, 1.0 - closest_distance/2);
        return vec3(BG_COLOR.x *glow_value * 2,BG_COLOR.y * glow_value/2,BG_COLOR.z * glow_value);
    }
    //no glow
    return vec3(0.0);
    
    
}
void main()
{

    //BG depending on if glow is enabled or not
    if(GLOWENABLED){BG_COLOR = vec3(.3,.05,.35);}
    else{BG_COLOR = vec3(0.0);}


    PLAYERVIEW.x = PLAYERVIEWX;

    //setup for main loop
    u_resolution.x = 1600;
    u_resolution.y = 900;
    //vec2 uv = (fragCoord - 0.5*iResolution.xy) / iResolution.y;
    vec2 uv = (gl_FragCoord.xy-.5*u_resolution.xy + PLAYERVIEW.xy)/u_resolution.y;
    vec3 rayDirection = normalize(vec3(uv.xy,1));

    //raymarch
    vec3 shadeColor = ray_march(cameraPosition, rayDirection); 

   //set the color of the pixel
   //set red for now
   gl_FragColor = vec4(shadeColor, 1.0);
}
