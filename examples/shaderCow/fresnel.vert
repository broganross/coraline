#version 120

varying vec4 V;			// vertex position
varying vec4 E;			// eye position
varying vec3 N;			// surface normal

void main()
{
    V = gl_ModelViewMatrix*gl_Vertex;
    E = gl_ProjectionMatrixInverse*vec4(0,0,-1,0);
    N = normalize(gl_NormalMatrix*gl_Normal);

    gl_Position = ftransform();
} 
