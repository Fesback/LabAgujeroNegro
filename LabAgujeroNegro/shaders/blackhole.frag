#version 450 core

in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;
uniform vec3 uCamPos;
uniform vec3 uCamForward;
uniform vec3 uCamRight;
uniform vec3 uCamUp;
uniform float uTanFov;

const float PI = 3.14159265359;

vec3 cielo(vec3 d)
{
	float lon = atan(d.z, d.x);
	float lat = asin(clamp(d.y, -1.0, 1.0));
	float paso = PI / 12.0;
	float c = mod(floor(lon / paso) + floor(lat / paso), 2.0);

	vec3 col = mix(vec3(0.02, 0.02, 0.08), vec3(0.07, 0.07, 0.20), c);

	col += vec3(0.6, 0.3, 0.1) * smoothstep(0.01, 0.0, abs(lat));
	return col;
}

void main() 
{
	vec2 p = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;	

	vec3 ro = uCamPos;
	vec3 rd = normalize(uCamForward + (p.x * uCamRight + p.y * uCamUp) * 2.0 * uTanFov);

	float b = dot(ro, rd);
	float c = dot(ro, ro) - 1.0;
	float h = b * b - c;

	vec3 col;
	float t = -b - sqrt(max(h, 0.0));
	if (h > 0.0 && t > 0.0) {
		vec3 n = normalize(ro + t * rd);
		float luz = max(dot(n, normalize(vec3(1.0, 1.0, 0.5))), 0.0);
		col = vec3(0.05) + 0.9 * luz * vec3(0.8, 0.5, 0.3);
	} else {
		col = cielo(rd);
	}


	FragColor = vec4(col, 1.0);	
}