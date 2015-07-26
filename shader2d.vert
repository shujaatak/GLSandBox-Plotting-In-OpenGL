// 2D Shader
#version 330
in vec2 coord2d;
in vec4 vColor;
out vec4 vVaryingColor;
uniform highp mat4 matrix;
void main(void)
{
    //"   gl_Position = matrix * vec4(coord2d,0.0,1.0);
 vVaryingColor = vColor;
gl_Position = vec4(coord2d,0.0,1.0);
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
