#include <glad/glad.h>
#include <GLFW/glfw3.h>	
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Camera.h"


static std::string leerArchivo(const std::string& ruta)
{
	std::ifstream f(ruta);
	if (!f) {
		std::cerr << "No se pudo abrir: " << ruta << "\n";
		return "";
	}

	std::stringstream ss;
	ss << f.rdbuf();
	return ss.str();	
}


static GLuint compilarShader(GLenum tipo, const std::string& fuente)
{
	GLuint id = glCreateShader(tipo);
	const char* src = fuente.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint ok = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
	if (!ok) {
		char log[1024];
		glGetShaderInfoLog(id, sizeof(log), nullptr, log);
		std::cerr << "Errror compilando shader:\n" << log << "\n";
	}
	return id;
}

static GLuint crearPrograma(const std::string& rutaVert, const std::string& rutaFrag)
{
	GLuint vs = compilarShader(GL_VERTEX_SHADER, leerArchivo(rutaVert));
	GLuint fs = compilarShader(GL_FRAGMENT_SHADER, leerArchivo(rutaFrag));	

	GLuint prog = glCreateProgram();
	glAttachShader(prog, vs);
	glAttachShader(prog, fs);
	glLinkProgram(prog);

	GLint ok = 0;
	glGetProgramiv(prog, GL_LINK_STATUS, &ok);
	if (!ok) {
		char log[1024];
		glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
		std::cerr << "Error enlazando programa:\n" << log << "\n";
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
	return prog;
}

// envia un vec3 de GLM a un uniform del sahder
static void enviarVec3(GLuint programa, const char* nombre, const glm::vec3& v)
{
	glUniform3f(glGetUniformLocation(programa, nombre), v.x, v.y, v.z);
}

int main() 
{

	// inicio glfw
	if (!glfwInit()) {
		std::cerr << "No se pudo iniciar GLFW\n";
		return -1;
	}


	// pedimos contexto opengl en perfil core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// creamos ventana
	GLFWwindow* ventana = glfwCreateWindow(1280, 720, "Laboratorio de Agujero Negro", nullptr, nullptr);
	if (!ventana) {
		std::cerr << "No se pudo crear la centana\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(ventana);
	glfwSwapInterval(1); // esto sincroniza con el moni

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "No se pudo cargar GLAD\n";
		return -1;
	}

	std::cout << "OpenGL: " << glGetString(GL_VERSION) << "\n";
	std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

	//Dear gui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(ventana, true);
	ImGui_ImplOpenGL3_Init("#version  450");

	GLuint programa = crearPrograma("shaders/fullscreen.vert", "shaders/blackhole.frag");
	GLuint vao;
	glGenVertexArrays(1, &vao);

	CamaraOrbital camara;
	float brillo = 1.0f; // parametro controlado por el slider

	while (!glfwWindowShouldClose(ventana))
	{
		glfwPollEvents();
		if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(ventana, true);

		glClearColor(0.2f, 0.2f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		int ancho, alto;
		glfwGetFramebufferSize(ventana, &ancho, &alto);
		if (ancho == 0 || alto == 0) continue;
		glViewport(0, 0, ancho, alto);

		//inicio del cuadro de interfaz
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		//entrada de mouse en caso el cursor NO este sobre el panel
		ImGuiIO& io = ImGui::GetIO();
		if (!io.WantCaptureMouse) {
			if (io.MouseDown[0])
				camara.rotar(io.MouseDelta.x, io.MouseDelta.y);
			if (io.MouseWheel != 0.0f)
				camara.acercar(io.MouseWheel);
		}

		//Panel de control
		ImGui::Begin("Panel de control");
		ImGui::Text("FPS: %.1f", io.Framerate);
		ImGui::Separator();
		ImGui::Text("Arrastra con el mouse: girar");
		ImGui::Text("Rieda: acercar / alejar");
		ImGui::SliderFloat("Distancia", &camara.distancia, 1.5f, 100.0f);
		ImGui::SliderFloat("Campo de vision", &camara.fov, 30.0f, 120.0f);
		if (ImGui::Button("Reiniciar camara"))
			camara.reiniciar();
		ImGui::End();

		// Escena

		glClearColor(0.02f, 0.02f, 0.08f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(programa);
		glUniform2f(glGetUniformLocation(programa, "uResolution"), (float)ancho, (float)alto);
		enviarVec3(programa, "uCamPos", camara.posicion());
		enviarVec3(programa, "uCamForward", camara.adelante());
		enviarVec3(programa, "uCamRight", camara.derecha());
		enviarVec3(programa, "uCamUp", camara.arriba());
		glUniform1f(glGetUniformLocation(programa, "uTanFov"), camara.tanMedioFov());
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Interfaz encima de la escena
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(ventana);


	
	}


	//limpiamos
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glDeleteVertexArrays(1, &vao);
	glDeleteProgram(programa);
	glfwDestroyWindow(ventana);
	glfwTerminate();
	return 0;


}