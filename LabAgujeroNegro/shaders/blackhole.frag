#version 450 core

in vec2 vUV;
out vec4 FragColor;

uniform vec2 uResolution;
uniform vec3 uCamPos;
uniform vec3 uCamForward;
uniform vec3 uCamRight;
uniform vec3 uCamUp;
uniform float uTanFov;
uniform sampler2D  uStarMap;
uniform float uExposicion;

const float PI = 3.14159265359;

void main() 
{
	vec2 p = (gl_FragCoord.xy - 0.5 * uResolution) / uResolution.y;	

	vec3 ro = uCamPos;
	vec3 rd = normalize(uCamForward + (p.x * uCamRight + p.y * uCamUp) * 2.0 * uTanFov);

	//coordenadas de textura a partir de la direccion del rayo
	float lon = atan(rd.z, rd.x);
	float lat = asin(clamp(rd.y, -1.0, 1.0));
	vec2 uv = vec2(lon / (2.0 * PI) + 0.5, lat / PI + 0.5);


	// derivadas calculadas fuera de cualquier  if de lo contrario quedan indefinicas
	// en la costura (lon = +-PI) u salta de 1  a 0 y el mipmap elegiria un nivel minimo, se ve como una linea
	vec2 uvB = vec2(fract(uv.x + 0.5) - 0.5, uv.y);
	vec2 ddx = (fwidth(uv.x) < fwidth(uvB.x)) ? dFdx(uv) : dFdx(uvB);
	vec2 ddy = (fwidth(uv.x) < fwidth(uvB.x)) ? dFdy(uv) : dFdy(uvB);

	float b = dot(ro, rd);
	float c = dot(ro, ro) - 1.0;
	float h = b * b - c;
	float t = -b - sqrt(max(h, 0.0));

	vec3 col;
	if (h > 0.0 && t > 0.0) {
		vec3 n = normalize(ro + t * rd);
		float luz = max(dot(n, normalize(vec3(1.0, 1.0, 0.5))), 0.0);
		col = vec3(0.05) + 0.9 * luz * vec3(0.8, 0.5, 0.3);
	} else {
		col = textureGrad(uStarMap, uv, ddx, ddy).rgb * uExposicion;
	}


	FragColor = vec4(col, 1.0);	
}