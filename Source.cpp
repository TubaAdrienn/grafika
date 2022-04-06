#include <array>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;

std::vector<glm::vec3> s_vertices = {};
std::vector<glm::vec3> s_colors = {};
glm::vec2 point1 = glm::vec2(-1.0f, 0.75f);
glm::vec2 point2 = glm::vec2(0.25f, -1.0f);

#define		numVBOs			2
#define		numVAOs			1
GLuint		VBO[numVBOs];
GLuint		VAO[numVAOs];

int			window_width	= 600;
int			window_height	= 600;
char		window_title[]	= "Bead 1";
int			circleFragmentation = 25;

GLboolean	keyboard[512]	= {GL_FALSE};
GLFWwindow	*window			= nullptr;
GLuint		renderingProgram;

GLuint		XoffsetLocation;
GLuint		YoffsetLocation;
GLuint		X1;
GLuint		Y1;
GLuint		X2;
GLuint		Y2;

float a = point2.y - point1.y;
float b = point2.x - point1.x;
float c = a * point2.y - b * point1.y;

float		x = 0.00f;
float		y = 0.00f;
float		incrementX = 0.02f;
float		incrementY = 0.01f;


bool checkOpenGLError() {
	bool	foundError	= false;
	int		glErr		= glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;

		foundError	= true;
		glErr		= glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int		length			= 0;
	int		charsWritten	= 0;
	char	*log			= nullptr;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
	
	if (length > 0) {
		log = (char*)malloc(length);
		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int		length			= 0;
	int		charsWritten	= 0;
	char	*log			= nullptr;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);
	
	if (length > 0) {
		log = (char*)malloc(length);
		glGetProgramInfoLog(prog, length, &charsWritten, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) {
	ifstream	fileStream(filePath, ios::in);
	string		content;
	string		line;
	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}

	fileStream.close();
	return content;
}

GLuint createShaderProgram() {
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char	*vertShaderSrc = vertShaderStr.c_str();
	const char	*fragShaderSrc = fragShaderStr.c_str();
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}
	GLuint		vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return vfProgram;
}


void generateCirclePoints(glm::vec2 start_point, GLfloat rad, GLint num_segment) {
	GLfloat x, y;
	GLfloat alpha = 0.0f;
	GLfloat full_circle = 2.0f * M_PI;

	s_vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	s_colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

	for (int i = 0; i <= num_segment; i++) {
		float theta = 2.0f * M_PI * alpha;
		float x = start_point.x + rad * cosf(theta);
		float y = start_point.y + rad * sinf(theta);
		s_vertices.push_back(glm::vec3(x, y, 0.0f));
		s_colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
		alpha += 1.0f / num_segment;
	}

	x = start_point.x + rad * cosf(0);
	y = start_point.y + rad * sinf(0);
	s_vertices.push_back(glm::vec3(x, y, 0.0f));
	s_colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

}

bool detectCollision(float x, float y) {
	for (int i = 0; i <= circleFragmentation; i++) {
		if (s_vertices.at(i).x + x > 1 || s_vertices.at(i).x + x < -1) return true;
		if (s_vertices.at(i).y + y > 1 || s_vertices.at(i).y + y < -1) return true;
	}
	return false;
}

void init(GLFWwindow* window) {
	renderingProgram = createShaderProgram();

	generateCirclePoints(glm::vec2(0.0f, 0.0), 0.2f, circleFragmentation);
	s_vertices.push_back(glm::vec3(point1.x, point1.y, 0.5f));
	s_vertices.push_back(glm::vec3(point2.x, point2.y, 0.5f));
	s_colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));
	s_colors.push_back(glm::vec3(1.0f, 1.0f, 1.0f));

	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, s_vertices.size() * sizeof(glm::vec3), s_vertices.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0); 
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, s_colors.size()*sizeof(glm::vec3), s_colors.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(1);

	XoffsetLocation = glGetUniformLocation(renderingProgram, "offsetX");
	YoffsetLocation = glGetUniformLocation(renderingProgram, "offsetY");
	X1 = glGetUniformLocation(renderingProgram, "x1");
	X2 = glGetUniformLocation(renderingProgram, "x2");
	Y1 = glGetUniformLocation(renderingProgram, "y1");
	Y2 = glGetUniformLocation(renderingProgram, "y2");
	glProgramUniform1f(renderingProgram, X1, point1.x);
	glProgramUniform1f(renderingProgram, X2, point2.x);
	glProgramUniform1f(renderingProgram, Y1, point1.y);
	glProgramUniform1f(renderingProgram, Y2, point2.y);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(renderingProgram);
	glClearColor(0.0, 0.0, 0.0, 1.0);
}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO[0]);

	x += incrementX;
	if (detectCollision(x, 0)) incrementX *= -1;
	glProgramUniform1f(renderingProgram, XoffsetLocation, x);

	y += incrementY;
	if (detectCollision(0,y)) incrementY *= -1;
	glProgramUniform1f(renderingProgram, YoffsetLocation, y); 

	glPointSize(10.0f);
	glDrawArrays(GL_TRIANGLE_FAN, 0, s_vertices.size()-2);

	glDrawArrays(GL_LINES, s_vertices.size()-2, 2);

	glBindVertexArray(0);
}

void cleanUpScene() {
	glDeleteProgram(renderingProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	window_width	= width;
	window_height	= height;
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		keyboard[key] = GL_TRUE;
	else if (action == GLFW_RELEASE)
		keyboard[key] = GL_FALSE;

	if (key == GLFW_KEY_V && action == GLFW_PRESS) {
		float original = y;
		if (incrementY < 0) y += 0.2f;
		if (incrementY > 0) y -= 0.2f;
		if (detectCollision(0, y)) y = original;

		//y += 0.1f;
		//if (detectCollision(0, y)) y = original;
	}

}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos) {

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

}

int main(void) {
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);

	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);

	glfwSwapInterval(1);

	glfwSetWindowSizeLimits(window, 400, 400, 800, 800);
	glfwSetWindowAspectRatio(window, 1, 1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	cleanUpScene();

	return EXIT_SUCCESS;
}