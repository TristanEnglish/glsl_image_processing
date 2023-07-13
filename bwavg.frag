// Greyscale Average Shader - Image Processing
// Converts colors to greyscale using the averaging method
// Takes rgb values, averages them and sets new rgb values to the average

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
   float gray = (sample(0.0,0.0).r + sample(0.0,0.0).g + sample(0.0,0.0).b) / 3.0;
   gl_FragColor = vec4(vec3(gray),1.0);
}
