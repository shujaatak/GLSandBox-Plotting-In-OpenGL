#version 330
//in vec2 coord2d;
in vec3 coord3d;
in vec4 vColor;
out vec4 vVaryingColor;
//uniform highp mat4 matrix;
uniform mat4 projMatrix;
uniform mat4 mvMatrix;
//uniform mat3 normalMatrix;
void main(void)
{
    vVaryingColor = vColor;
//    vVaryingColor = vec4(1.0f,1.0f,1.0f,1.0f);
//    gl_Position = projMatrix * mvMatrix * vec4(coord2d,0.0f,1.0);
      gl_Position = projMatrix * mvMatrix * vec4(coord3d,1.0f);
//          gl_Position = projMatrix * mvMatrix * coord3d;
}
