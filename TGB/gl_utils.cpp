#include "gl_utils.h"


void glfw_error_callback(int error, const char* description) {
	fputs(description, stderr);
	// gl_log_err("%s\n", description);
}
void glfw_window_size_callback(GLFWwindow* window, int width, int height) {
	g_gl_width = width;
	g_gl_height = height;
	printf("width %i height %i\n", width, height);
	/* update any perspective matrices used here */
}
void _update_fps_counter(GLFWwindow* window) {
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25) {
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle(window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//}



bool start_gl()
{
	glfwSetErrorCallback(glfw_error_callback);
	/* Initialize the library */
	if (!glfwInit()){
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return -1;
	}

	/* We must specify 3.2 core if on Apple OS X  */
#ifdef __APPLE__
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

	/* Create a windowed mode window and its OpenGL context */
	g_window = glfwCreateWindow(g_gl_width, g_gl_height, "Hello World", NULL, NULL);
	if (!g_window)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return -1;
	}

	
	// na inicialização da aplicação, junto com as outras definições de callback
	// note que o ponteiro da janela também deve ser informado
	glfwSetWindowSizeCallback(g_window, glfw_window_size_callback);
	/* Make the window's context current */
	glfwMakeContextCurrent(g_window);

	glfwWindowHint(GLFW_SAMPLES, 4);

	//glfwSetKeyCallback(window, key_callback);
	/* start GLEW extension handler */
	glewExperimental = GL_TRUE;
	glewInit();

	// obtenção de versão suportada da OpenGL e renderizador
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	printf("Renderer: %s\n", renderer);
	printf("OpenGL (versão suportada) %s\n", version);
	return true;
}

/* copy a shader from a plain text file into a character array */
bool parse_file_into_str(const char* file_name, char* shader_str, int max_len) {
	FILE* file = fopen(file_name, "r");
	if (!file) {
		fprintf(stderr, "ERROR: opening file for reading: %s\n", file_name);
		return false;
	}
	size_t cnt = fread(shader_str, 1, max_len - 1, file);
	if ((int)cnt >= max_len - 1) {
		fprintf(stderr, "WARNING: file %s too big - truncated.\n", file_name);
	}
	if (ferror(file)) {
		fprintf(stderr, "ERROR: reading shader file %s\n", file_name);
		fclose(file);
		return false;
	}
	// append \0 to end of file string
	shader_str[cnt] = 0;
	fclose(file);
	return true;
}
