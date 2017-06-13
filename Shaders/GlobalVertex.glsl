#version 41
#extension GL_ARB_shading_language_include : require

out gl_PerVertex {
    vec4 gl_Position;
    float gl_PointSize;
    float gl_ClipDistance[];
};