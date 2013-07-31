#version 120
// Fresnel example fragment shader

// lighting parameters in view space
varying vec4 V;			// vertex position
varying vec4 E;			// eye position
varying vec3 N;			// surface normal

// direction of brightest point in environment map
uniform sampler2D env;

// lighting parameters -- could pass in as uniform
uniform float reflectance;		// Fresnel reflectance at zero angle
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float es;

uniform vec3 envPos;
uniform vec3 lightPos;

void
main()
{
    // lighting vectors
    vec3 Nn = normalize(N);	// normal
    vec3 In = normalize(V.xyz*E.w - E.xyz*V.w); // -view
    vec3 Hn = normalize(lightPos-In);	// half way between view & light

    vec3 R = reflect(In,Nn);
    vec3 RH = normalize(R-In);
    float fresnel = reflectance + (1.-reflectance)*pow(1.+dot(In,RH),5.0);

    // color
    float diff = max(0.,dot(Nn,lightPos));
    float spec = pow(max(0.,dot(Nn,Hn)),es);
    vec4 col = vec4((ambient + diffuse*diff + specular*spec).xyz * fresnel, 1.0);

    
    
    vec4 env = texture2D(env, .5+.5*normalize(R+vec3(envPos.x,envPos.y,envPos.z)).xy);

    gl_FragColor = mix(col,env,fresnel);
}
