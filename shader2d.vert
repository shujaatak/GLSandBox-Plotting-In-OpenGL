#version 330
in vec2 coord2d;
in vec4 vColor;
out vec4 vVaryingColor;
//uniform highp mat4 matrix;
uniform mat4 projMatrix;
uniform mat4 mvMatrix;
//uniform mat3 normalMatrix;
void main(void)
{
 vVaryingColor = vColor;
// gl_Position = vec4(coord2d,0.0,1.0);
// gl_Position = projMatrix * mvMatrix * vec4(coord2d,0.5f,1.0);
 gl_Position = projMatrix * mvMatrix * vec4(coord2d,0.5f,1.0);
}

/*
static const char *vertexShaderSimple =
        //   "attribute highp vec4 vertex;\n"
        "#version 330\n"
        "in vec4 vertex;\n"
        "uniform highp mat4 matrix;\n"
        "void main(void)\n"
        "{\n"
        "   gl_Position = matrix * vertex;\n"
        "}";
        */
