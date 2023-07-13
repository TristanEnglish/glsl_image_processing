// Greyscale Luminance Shader - Image Processing
// Converts colors to greyscale using luminance/luma method, rather than averaging
// Humans perceive green more strongly than red, and red more strongly than blue, so these colors are weighted more heavily.

#version 120

uniform float dX;
uniform float dY;
uniform sampler2D img;

vec4 sample(float dx,float dy)
{
   return texture2D(img,gl_TexCoord[0].st+vec2(dx,dy));
}

void main()
{
   float gray = 0.21 * sample(0.0,0.0).r + 0.72 * sample(0.0,0.0).g + 0.07 * sample(0.0,0.0).b;
   gl_FragColor = vec4(vec3(gray),1.0);
}
