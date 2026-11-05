#version 450 core

in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;
uniform float uTime;

void main() 
{
	vec2 p = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;	

	vec3 col = mix(vec3(0.02, 0.02, 0.08), vec3(0.15, 0.05, 0.02), vUV.y);

	float r  = length(p);
	float borde = smoothstep(0.22, 0.20, r);
	float brillo = exp(-abs(r - 0.22) * 30.0);
	col += brillo * vec3(1.0, 0.6, 0.2) * (0.6 + 0.4 * sin(uTime * 2.0));
	col += (1.0 - borde);

	FragColor = vec4(col, 1.0);	
}