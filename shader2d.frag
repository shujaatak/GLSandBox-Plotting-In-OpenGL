#version 330
out vec4 vFragColor;
in vec4 vVaryingColor;
void main(void)
{
vFragColor = vVaryingColor;
 //gl_FragColor = color;
}

/*
static const char *fragmentShaderSimple =
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "   gl_FragColor = color;\n"
        "}";
*/
