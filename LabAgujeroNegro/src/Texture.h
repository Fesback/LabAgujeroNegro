#pragma once
#include <glad/glad.h>
#include <iostream>
#include <string>
#include <stb_image.h>

// carga una img y la sube a la gpu como textura, nos devolvera 0 si falla
inline GLuint cargarTextura(const std::string& ruta)
{
	stbi_set_flip_vertically_on_load(true); // opengl tiene el origen abajo

	int ancho, alto, canales;
	unsigned char* datos = stbi_load(ruta.c_str(), &ancho, &alto, &canales, 3);
	if (!datos) {
		std::cerr << "No se pudo cargar la textura: " << ruta << "\n";
		return 0;
	}

	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, ancho, alto, 0, GL_RGB, GL_UNSIGNED_BYTE, datos);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(datos);
	std::cout << "Textura cargada: " << ruta << " (" << ancho << "x" << alto << ")\n";
	return tex;
}