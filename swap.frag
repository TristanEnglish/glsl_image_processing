// Swap Color Shader - Image Processing
// Swaps colors in rgb to gbr

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
   gl_FragColor = vec4(sample(0.0,0.0).gbra);
}
