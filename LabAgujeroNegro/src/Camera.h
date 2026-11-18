#pragma once
#include <glm/glm.hpp>
#include <cmath>

//la camara que orbita alrededor del origen (0,0,0)
// las distancias se mediran en unidades de rs 

struct CamaraOrbital
{
	float yaw = 0.0f; // giro horizontal (radianes)
	float pitch = 0.35f; // giro vertical (radianes)
	float distancia = 8.0f; // distancia al origen
	float fov = 69.0f; // campo de vision vertical (grados)

	void reiniciar() { *this = CamaraOrbital();  }

	void rotar(float dx, float dy)
	{
		const float sensibilidad = 0.005f;
		yaw -= dx * sensibilidad;
		pitch += dy * sensibilidad;
		pitch = glm::clamp(pitch, -1.55f, 1.55f); //esto evita voltear la camara
	}

	// rueda: +1 acerca, -1 alega... cada click cambia la distancia -10%

	void acercar(float rueda)
	{
		distancia *= std::pow(0.9, rueda);
		distancia = glm::clamp(distancia, 1.5f, 100.0f);
	}

	//posicion en coordenadass esfericas -- cartesianas (el eje Y es arriba)
	glm::vec3 posicion() const
	{
		return distancia * glm::vec3(std::cos(pitch)* std::cos(yaw),
								std::sin(pitch),
			std::cos(pitch)* std::sin(yaw));
	}


	//base de la camara: hacia donde mira, derecha y arriba
	glm::vec3 adelante() const { return -glm::normalize(posicion()); }
	glm::vec3 derecha() const { return glm::normalize(glm::cross(adelante(), glm::vec3(0, 1, 0))); }
	glm::vec3 arriba() const { return glm::cross(derecha(), adelante()); }

	float tanMedioFov() const { return std::tan(glm::radians(fov) * 0.5f); }
};