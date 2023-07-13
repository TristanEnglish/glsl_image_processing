// Shift Image Shader - Image Processing
// Shifts image by 100 pixels to the right

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
   gl_FragColor = sample(-100*dX,0.0);
}
