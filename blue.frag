// Blue Color Shader - Image Processing
// Only shows colors in the blue channel

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
   gl_FragColor = sample(0.0,0.0) * vec4(0.0,0.0,1.0,1.0);
}
