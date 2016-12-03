#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h> 

#include "Mesh.h"
#include "Material.h"
#include "OBJReader.h"
#include "ObjAdapter.h"

#include "gl_utils.h"

/*
Trabalho do GB:
-------------- -
1 - Leitor de objeto corrigido para obter dados
2 - Converter(adapter) da malha lida do Obj para uma malha compatível com shader.
3 - Representar informações com VAOs(1 para cada grupo) e seus respectivos VBOs(vértices, texturas e normais).
4 - Deve desenhar com shaders, inclusive com textura e iluminação.
5 - Deve permitir : ou animação, ou edição de cena, ou ser um jogo, ...
6 - Deve permitir salvar e abrir no estado em que a cena foi salva.
*/

// keep track of window size for things like the viewport and the mouse cursor
int g_gl_width = 640;
int g_gl_height = 480;
GLFWwindow* g_window = NULL;

void init(void) {
	Mesh* mesh = OBJReader::readFromPath((char *) "mesa01.obj");
	std::map<std::string, Material*> materialsMap = OBJReader::readMtlFrom(mesh->mtllib);
	// ObjAdapter::
}

void mouse(int button, int state, int x, int y) {
	// draw scene sem textura iluminacao e nada
	// apenas os grupos com uma cor para cada.
	// le a cor do pixel
	// glReadPixels(x, 480 - y, 1, 1, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixel);
}


int main(void)
{
	// all the GLFW and GLEW start-up code is moved to here in gl_utils.cpp
	start_gl();
	/* tell GL to only draw onto a pixel if the shape is closer to the viewer
	than anything already drawn at that pixel */
	glEnable(GL_DEPTH_TEST); /* enable depth-testing */
							 /* with LESS depth-testing interprets a smaller depth value as meaning "closer" */
	glDepthFunc(GL_LESS);


	/* geometry to use. these are 3 xyz points (9 floats total) to make a triangle */
	GLfloat points[] = {
		0.0f,	0.5f,	0.0f,
		0.5f, -0.5f,	0.0f,
		-0.5f, -0.5f,	0.0f
	};
	GLfloat colours[] = {
		1.0f, 0.0f,  0.0f,
		0.0f, 1.0f,  0.0f,
		0.0f, 0.0f,  1.0f
	};
	/* a vertex buffer object (VBO) is created here. this stores an array of
	data on the graphics adapter's memory. in our case - the vertex points */
	GLuint points_vbo;
	glGenBuffers(1, &points_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), points, GL_STATIC_DRAW);

	/* create a second VBO, containing the array of colours.
	note that we could also put them both into a single vertex buffer. in this
	case we would use the pointer and stride parameters of glVertexAttribPointer()
	to describe the different data layouts */
	GLuint colours_vbo;
	glGenBuffers(1, &colours_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), colours, GL_STATIC_DRAW);

	/* create the VAO.
	we bind each VBO in turn, and call glVertexAttribPointer() to indicate where
	the memory should be fetched for vertex shader input variables 0, and 1,
	respectively. we also have to explicitly enable both 'attribute' variables.
	'attribute' is the older name for vertex shader 'in' variables. */
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, points_vbo);
	// "attribute #0 is created from every 3 variables in the above buffer, of type float (i.e. make me vec3s)"
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	/* here we copy the shader strings into GL shaders, and compile them. we
	then create an executable shader 'program' and attach both of the compiled
	shaders. we link this, which matches the outputs of the vertex shader to
	the inputs of the fragment shader, etc. and it is then ready to use */
	char vertex_shader[1024 * 256];
	char fragment_shader[1024 * 256];
	parse_file_into_str("vertex_shader.glsl", vertex_shader, 1024 * 256);
	parse_file_into_str("fragment_shader.glsl", fragment_shader, 1024 * 256);

	
	char log[2048];
	/* GL shader objects for vertex and fragment shader [components] */
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* p = (const GLchar*)vertex_shader;
	glShaderSource(vert_shader, 1, &p, NULL);
	glCompileShader(vert_shader);

	/* check for shader compile errors - very important! */
	int params = -1;
	int maxLength = 1024 * 256;
	int actualLength = 0;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		glGetShaderInfoLog(vert_shader, maxLength, &actualLength, log);
		printf("LOG GL for index %u: \n%s\n", vert_shader, log);
		return 1; /* or exit or something */
	}

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	p = (const GLchar*)fragment_shader;
	glShaderSource(frag_shader, 1, &p, NULL);
	glCompileShader(frag_shader);

	/* check for shader linking errors - very important! */
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &params);
	if (GL_TRUE != params) {
		glGetShaderInfoLog(frag_shader, maxLength, &actualLength, log);
		printf("LOG GL for index %u: \n%s\n", frag_shader, log);
		return 1; /* or exit or something */
	}

	/* GL shader programme object [combined, to link] */
	GLuint shader_programme;
	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, frag_shader);
	glAttachShader(shader_programme, vert_shader);
	glLinkProgram(shader_programme);

	// check for compile errors
	glGetProgramiv(shader_programme, GL_LINK_STATUS, &params);
	if (GL_TRUE != params) {
		fprintf(stderr, "ERROR: could not link shader programme GL index %i\n",
			shader_programme);
		return false;
	}

	// ---

	// init();


	// ---

	// MATRIZ PARA TRANSFORMAÇÂO

	GLfloat matrix[] = {
		1.0f, 0.0f, 0.0f, 0.0f, // first column
		0.0f, 1.0f, 0.0f, 0.0f, // second column
		0.0f, 0.0f, 1.0f, 0.0f, // third column
		0.5f, 0.0f, 0.0f, 1.0f // fourth column
	};

	int matrix_location = glGetUniformLocation(shader_programme, "matrix");
	glUseProgram(shader_programme);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);



	// ----

	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CW); // GL_CCW for counter clock-wise

	/* this loop clears the drawing surface, then draws the geometry described
	by the VAO onto the drawing surface. we 'poll events' to see if the window
	was closed, etc. finally, we 'swap the buffers' which displays our drawing
	surface onto the view area. we use a double-buffering system which means
	that we have a 'currently displayed' surface, and 'currently being drawn'
	surface. hence the 'swap' idea. in a single-buffering system we would see
	stuff being drawn one-after-the-other */
	float speed = 1.0f; // move at 1 unit per second
	float last_position = 0.0f;
	while (!glfwWindowShouldClose(g_window))
	{
		// add a timer for doing animation
		static double previous_seconds = glfwGetTime();
		double current_seconds = glfwGetTime();
		double elapsed_seconds = current_seconds - previous_seconds;
		previous_seconds = current_seconds;



		_update_fps_counter(g_window);
		/* Render here */
		/* wipe the drawing surface clear */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_gl_width, g_gl_height);

		glUseProgram(shader_programme);

		// update the matrix
		// - you could simplify this by just using sin(current_seconds)
		matrix[12] = elapsed_seconds * speed + last_position;
		last_position = matrix[12];
		if (fabs(last_position) > 1.0) {
			speed = -speed;
		}
		//
		// Note: this call is related to the most recently 'used' shader programme
		glUniformMatrix4fv(matrix_location, 1, GL_FALSE, matrix);


		glBindVertexArray(vao);

		/* draw points 0-3 from the currently bound VAO with current in-use shader */
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* update other events like input handling */
		/* Poll for and process events */
		glfwPollEvents();
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(g_window, 1);
		}

		/* Swap front and back buffers */
		/* put the stuff we've been drawing onto the display */
		glfwSwapBuffers(g_window);
	}

	/* close GL context and any other GLFW resources */
	glfwTerminate();
	return 0;
}
