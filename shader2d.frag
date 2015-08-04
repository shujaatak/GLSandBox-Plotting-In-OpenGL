#version 330
out vec4 vFragColor;
in vec4 vVaryingColor;
void main(void)
{
vFragColor = vVaryingColor;
 //gl_FragColor = color;
//    vFragColor = vec4(1.0,1.0,1.0,1.0);
}

/*
static const char *fragmentShaderSimple =
        "uniform mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "   gl_FragColor = color;\n"
        "}";
*/
