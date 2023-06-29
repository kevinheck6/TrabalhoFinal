#include <iostream>
#include <string>
#include <assert.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"

#include "Mesh.h"
#include "Lighting.h"

#include "Bezier.h"

#include <random>
#include <algorithm>

using namespace std;

Mesh suzanne1, suzanne2, suzanne3;
std::vector<Mesh*> suzanneObjects;


struct Vertex
{
	glm::vec3 position;
	glm::vec3 v_color;
};

void setShaderFloat(Shader& shader, const std::string& filename) {
	std::ifstream file(filename);
	if (!file) {
		// Handle file open error
		cout << "Error opening file " << filename << endl;
		return;
	}

	std::string line;
	std::string currentMaterial;

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::string token;
		iss >> token;

		if (token == "newmtl") {
			iss >> currentMaterial;
		}
		else if (token == "Ka" && currentMaterial == "Material") {
			float ka_r, ka_g, ka_b;
			iss >> ka_r >> ka_g >> ka_b;
			shader.setFloat("ka", ka_r);

		}
		else if (token == "Kd" && currentMaterial == "Material") {
			float kd_r, kd_g, kd_b;
			iss >> kd_r >> kd_g >> kd_b;
			shader.setFloat("kd", kd_r);

		}
		else if (token == "Ks" && currentMaterial == "Material") {
			float ks_r, ks_g, ks_b;
			iss >> ks_r >> ks_g >> ks_b;
			shader.setFloat("ks", ks_r);

		}
	}
}

int main();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback_runtime(GLFWwindow* window, int button, int action, int mods);
int loadSimpleObj(string filePath, int& nVertices, glm::vec3 color = glm::vec3(1.0,0.0,0.0));
int generateTexture(string filePath);
void updateCameraDirection(double offsetX, double offsetY);
void performCircularMovement(GLFWwindow* window, float& angle, glm::vec3& lightPos, float radius, float angularSpeed);
vector<glm::vec3> generateControlPointsSet();
GLuint generateControlPointsBuffer(vector <glm::vec3> controlPoints);
std::vector<glm::vec3> generatePointsSet();


const GLuint WIDTH = 1000, HEIGHT = 1000;

bool rotateX=false, rotateY=false, rotateZ=false;

glm::vec3 cameraPos = glm::vec3(0.0, 0.0, 3.0);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0);
float cameraSpeed = 0.05;

bool firstMouse = true;
float lastX = 0.0, lastY = 0.0;
float yaw = -90.0, pitch = 0.0;




int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Visualizador 3D!", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	const GLubyte* renderer = glGetString(GL_RENDERER); 
	const GLubyte* version = glGetString(GL_VERSION); 
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	Shader shader("Hello3D.vs", "Hello3D.fs");

	GLuint texID = generateTexture("../../3D_models/Suzanne/cube.png");

	int nVertices;
	GLuint VAO = loadSimpleObj("../../3D_Models/Cube/cube.obj", nVertices);


	GLuint VAO2 = loadSimpleObj("../../3D_Models/Cube/cube.obj", nVertices);
	GLuint VAO3 = loadSimpleObj("../../3D_Models/Cube/cube.obj", nVertices, glm::vec3(1.0, 1.0, 0.0));

	Mesh suzanne1, suzanne2, suzanne3;
	suzanne1.initialize(VAO, nVertices, &shader, texID ,glm::vec3(-1.75, 0.0, 0.0), glm::vec3(1.0, 1.0, 1.0), true);
	suzanne2.initialize(VAO2, nVertices, &shader, texID, glm::vec3(1, 0.0, 0.0));
	suzanne3.initialize(VAO3, nVertices, &shader, texID, glm::vec3(0, 0.0, 0.0));

	suzanneObjects.push_back(&suzanne1);
	suzanneObjects.push_back(&suzanne2);
	suzanneObjects.push_back(&suzanne3);

	
	glUseProgram(shader.ID);

	glm::mat4 model = glm::mat4(1); //matriz identidade;
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	//
	model = glm::rotate(model, /*(GLfloat)glfwGetTime()*/glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	//Definindo a matriz de view (posição e orientação da câmera)
	glm::mat4 view = glm::lookAt(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	//Definindo a matriz de projeção perpectiva
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));


	shader.setFloat("ka", 0.4);
	shader.setFloat("kd", 0.5);
	shader.setFloat("ks", 0.5);
	shader.setFloat("q", 10);


	//Lighting lighting(glm::vec3(-2.0f, 10.0f, 3.0f), 1.0f, 1.0f, 1.0f); Usando a classe Lighting

	shader.setVec3("lightPos", -2.0f, 10.0f, 3.0f);
	shader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);


	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(shader.ID, "colorBuffer"), 0);

	glm::vec3 lightPos(-2.0f, 10.0f, 3.0f); // Initial light position
	float angle = 0.0f; // Initial angle for circular movement

	glEnable(GL_DEPTH_TEST);



	std::vector<glm::vec3> controlPoints = generateControlPointsSet();
	//GLuint VAO5 = generateControlPointsBuffer(controlPoints);

	std::vector<glm::vec3> uniPoints = generatePointsSet();
	GLuint VAOUni = generateControlPointsBuffer(uniPoints);

	Bezier bezier;
	bezier.setControlPoints(uniPoints);
	bezier.setShader(&shader);
	bezier.generateCurve(10);

	int nbCurvePoints = bezier.getNbCurvePoints();
	int i = 0;


	// Loop da aplicação - "game loop"
	while (!glfwWindowShouldClose(window))
	{
		// Check for input events and call corresponding callback functions
		glfwPollEvents();

		// Clear color buffer
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		// Change view matrix (camera position and orientation)
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

		// Send camera position to shader
		shader.setVec3("cameraPos", cameraPos.x, cameraPos.y, cameraPos.z);

		

		suzanne1.update();
		suzanne1.draw();
		suzanne2.update();
		suzanne2.draw();
		suzanne3.update();
		suzanne3.draw();

		mouse_button_callback_runtime(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, GLFW_MOD_SHIFT);

		setShaderFloat(shader, "../../3D_Models/Cube/cube.mtl");
		suzanne2.update();
		suzanne2.draw();

		setShaderFloat(shader, "../../3D_Models/Cube/cube.mtl");
		suzanne3.update();
		suzanne3.draw();


		// Call the circular movement function
		float radius = 5.0f; // Adjust the radius as desired
		float angularSpeed = 30.0f; // Adjust the speed as desired
		performCircularMovement(window, angle, lightPos, radius, angularSpeed);

		//lighting.performCircularMovement(window); AQUII
		//lighting.setLightUniforms(shader);  AQUIII

		// Set the light position in the shader
		
		shader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);


		glm::vec3 pointOnCurve = bezier.getPointOnCurve(i);

		suzanne3.move(pointOnCurve);

		i = (i + 1) % nbCurvePoints;
		
		for (Mesh* object : suzanneObjects)
		{
			object->update();
			object->draw();
			object->updateAnimation();
		}
		// Swap screen buffers
		glfwSwapBuffers(window);
	}
	// Pede pra OpenGL desalocar os buffers talvez tirar?
	glDeleteVertexArrays(1, &VAO);

	glfwTerminate();
	return 0;
}

//PARA A CURVA DE BEZIER
vector<glm::vec3> generateControlPointsSet()
{
	vector <glm::vec3> controlPoints;

	controlPoints.push_back(glm::vec3(-0.6, -0.4, 0.0));
	controlPoints.push_back(glm::vec3(-0.4, -0.6, 0.0));
	controlPoints.push_back(glm::vec3(-0.2, -0.2, 0.0));
	controlPoints.push_back(glm::vec3(0.0, 0.0, 0.0));
	controlPoints.push_back(glm::vec3(0.2, 0.2, 0.0));
	controlPoints.push_back(glm::vec3(0.4, 0.6, 0.0));
	controlPoints.push_back(glm::vec3(0.6, 0.4, 0.0));

	return controlPoints;
}

//Para a curva de Bezier
GLuint generateControlPointsBuffer(vector <glm::vec3> controlPoints)
{
	GLuint VBO, VAO;

	//Geração do identificador do VBO
	glGenBuffers(1, &VBO);

	//Faz a conexão (vincula) do buffer como um buffer de array
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Envia os dados do array de floats para o buffer da OpenGl
	glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(GLfloat) * 3, controlPoints.data(), GL_STATIC_DRAW);

	//Geração do identificador do VAO (Vertex Array Object)
	glGenVertexArrays(1, &VAO);

	// Vincula (bind) o VAO primeiro, e em seguida  conecta e seta o(s) buffer(s) de vértices
	// e os ponteiros para os atributos 
	glBindVertexArray(VAO);

	//Atributo posição (x, y, z)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Observe que isso é permitido, a chamada para glVertexAttribPointer registrou o VBO como o objeto de buffer de vértice 
	// atualmente vinculado - para que depois possamos desvincular com segurança
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Desvincula o VAO (é uma boa prática desvincular qualquer buffer ou array para evitar bugs medonhos)
	glBindVertexArray(0);

	return VAO;
}


std::vector<glm::vec3> generatePointsSet()
{
	float vertices[] = {
		0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.0, 0.0, 0.0,
		-0.5, 0.5, 0.0,
		-0.5, 0.5, 0.0,
		-0.5, 0.5, 0.0
	};

	std::vector<glm::vec3> uniPoints;

	for (int i = 0; i < 9 * 3; i += 3)
	{
		glm::vec3 point;
		point.x = vertices[i];
		point.y = vertices[i + 1];
		point.z = 0.0;

		uniPoints.push_back(point);
	}

	return uniPoints;
}



void performCircularMovement(GLFWwindow* window, float& angle, glm::vec3& lightPos, float radius, float angularSpeed)
{
	// Get the current time
	float currentTime = glfwGetTime();

	// Calculate new light position for circular movement
	lightPos.x = sin(angle) * radius;
	lightPos.z = cos(angle) * radius;

	// Increase the angle for the circular movement based on elapsed time
	float deltaTime = currentTime - angle;
	angle = currentTime;
	angle += glm::radians(angularSpeed) * deltaTime;
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	bool keyPress = (action == GLFW_PRESS);

	if (key == GLFW_KEY_ESCAPE && keyPress)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (keyPress)
	{
		rotateX = (key == GLFW_KEY_X);
		rotateY = (key == GLFW_KEY_Y);
		rotateZ = (key == GLFW_KEY_Z);
	}

	switch (key)
	{
	case GLFW_KEY_W:
		cameraPos += cameraSpeed * cameraFront;
		break;

	case GLFW_KEY_S:
		cameraPos -= cameraSpeed * cameraFront;
		break;

	case GLFW_KEY_A:
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;

	case GLFW_KEY_D:
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		break;
	}
}


void mouse_button_callback_runtime(GLFWwindow* window, int button, int action, int mods) {
	static int selectedObjectIndex = 0;
	Mesh* previousSelectedObject = suzanneObjects[selectedObjectIndex];
	

	static double lastObjectChangeTime = glfwGetTime();
	const double objectChangeDelay = 0.5;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		double currentTime = glfwGetTime();
		if (currentTime - lastObjectChangeTime >= objectChangeDelay) {
			// Cycle through the objects using the right mouse button
			selectedObjectIndex = (selectedObjectIndex + 1) % suzanneObjects.size();
			// Verify that the selectedObjectIndex is within the valid range
			if (selectedObjectIndex >= 0 && selectedObjectIndex < suzanneObjects.size()) {
				printf("Selected object: %d\n", selectedObjectIndex);
				previousSelectedObject->setSelected(false);
				suzanneObjects[selectedObjectIndex]->setSelected(true);
				suzanneObjects[selectedObjectIndex]->animateSize(1.1f, 1.0f);
			}
			lastObjectChangeTime = currentTime;
		}
	}

	Mesh* currentSelectedObject = suzanneObjects[selectedObjectIndex];

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// Rotate the selected object continuously
		currentSelectedObject->rotateContinuous(0.5f, glm::vec3(1.0f, 1.0f, 0.0f));
		currentSelectedObject->update();
		currentSelectedObject->draw();
	}

	// Movement handling for arrow keys
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		// Move the selected object to the left
		currentSelectedObject->move(glm::vec3(-0.1f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		// Move the selected object to the right
		currentSelectedObject->move(glm::vec3(0.1f, 0.0f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		// Move the selected object up
		currentSelectedObject->move(glm::vec3(0.0f, 0.1f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		// Move the selected object down
		currentSelectedObject->move(glm::vec3(0.0f, -0.1f, 0.0f));
	}
	if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS) {
		// Move the selected object in the negative Z direction
		currentSelectedObject->move(glm::vec3(0.0f, 0.0f, -0.1f));
	}
	if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS) {
		// Move the selected object in the positive Z direction
		currentSelectedObject->move(glm::vec3(0.0f, 0.0f, 0.1f));
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		// Stop the rotation of the selected object
		currentSelectedObject->stopRotateContinuous();
	}

	// Update and draw the selected object
	currentSelectedObject->update();
	currentSelectedObject->draw();

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		// Rotate the selected object continuously
		currentSelectedObject->rotateContinuous(1.0f, glm::vec3(1.0f, 1.0f, 0.0f));
	}
}



void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	static bool firstMouse = true;
	static double lastX = xpos;
	static double lastY = ypos;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double offsetX = xpos - lastX;
	double offsetY = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	offsetX *= cameraSpeed;
	offsetY *= cameraSpeed;

	updateCameraDirection(offsetX, offsetY);
}





void updateCameraDirection(double offsetX, double offsetY)
{
	static double pitch = 0.0;
	static double yaw = -90.0;

	pitch += offsetY;
	yaw += offsetX;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}



bool openObjFile(const string& filePath, ifstream& inputFile) {
	inputFile.open(filePath);
	return inputFile.is_open();
}

void readVertices(ifstream& inputFile, const glm::vec3& color, vector<Vertex>& vertices) {
	string line;

	while (getline(inputFile, line)) {
		istringstream iss(line);
		string word;
		iss >> word;

		if (word == "v") {
			Vertex v;
			iss >> v.position.x >> v.position.y >> v.position.z;
			v.v_color = color;
			vertices.push_back(v);
		}
	}
}

void readTexCoords(ifstream& inputFile, vector<glm::vec2>& texCoords) {
	string line;

	while (getline(inputFile, line)) {
		istringstream iss(line);
		string word;
		iss >> word;

		if (word == "vt") {
			glm::vec2 vt;
			iss >> vt.s >> vt.t;
			texCoords.push_back(vt);
		}
	}
}

void readNormals(ifstream& inputFile, vector<glm::vec3>& normals) {
	string line;

	while (getline(inputFile, line)) {
		istringstream iss(line);
		string word;
		iss >> word;

		if (word == "vn") {
			glm::vec3 vn;
			iss >> vn.x >> vn.y >> vn.z;
			normals.push_back(vn);
		}
	}
}

void readFaces(ifstream& inputFile, const vector<Vertex>& vertices, const vector<glm::vec2>& texCoords, const vector<glm::vec3>& normals, vector<int>& indices, vector<GLfloat>& vertbuffer) {
	string line;

	while (getline(inputFile, line)) {
		istringstream iss(line);
		string word;
		iss >> word;

		if (word == "f") {
			for (int i = 0; i < 3; i++) {
				string token;
				iss >> token;
				int pos = token.find("/");
				int index = atoi(token.substr(0, pos).c_str()) - 1;
				indices.push_back(index);
				const Vertex& vertex = vertices[index];
				vertbuffer.push_back(vertex.position.x);
				vertbuffer.push_back(vertex.position.y);
				vertbuffer.push_back(vertex.position.z);
				vertbuffer.push_back(vertex.v_color.r);
				vertbuffer.push_back(vertex.v_color.g);
				vertbuffer.push_back(vertex.v_color.b);

				token = token.substr(pos + 1);
				pos = token.find("/");
				int indexT = atoi(token.substr(0, pos).c_str()) - 1;

				vertbuffer.push_back(texCoords[indexT].s);
				vertbuffer.push_back(texCoords[indexT].t);

				token = token.substr(pos + 1);
				int indexN = atoi(token.c_str()) - 1;

				vertbuffer.push_back(normals[indexN].x);
				vertbuffer.push_back(normals[indexN].y);
				vertbuffer.push_back(normals[indexN].z);
			}
		}
	}
}

GLuint createVertexBuffer(const vector<GLfloat>& vertbuffer) {
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertbuffer.size() * sizeof(GLfloat), vertbuffer.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return VBO;
}

GLuint createVertexArray(const GLuint VBO) {
	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return VAO;
}

int loadSimpleObj(string filePath, int& nVertices, glm::vec3 color) {
	ifstream inputFile;
	if (!openObjFile(filePath, inputFile)) {
		cout << "Não foi possível abrir o arquivo " << filePath << endl;
		return -1;
	}

	vector<Vertex> vertices;
	vector<int> indices;
	vector<glm::vec2> texCoords;
	vector<glm::vec3> normals;
	vector<GLfloat> vertbuffer;

	readVertices(inputFile, color, vertices);
	inputFile.clear();
	inputFile.seekg(0, ios::beg);

	readTexCoords(inputFile, texCoords);
	inputFile.clear();
	inputFile.seekg(0, ios::beg);

	readNormals(inputFile, normals);
	inputFile.clear();
	inputFile.seekg(0, ios::beg);

	readFaces(inputFile, vertices, texCoords, normals, indices, vertbuffer);

	inputFile.close();

	nVertices = vertbuffer.size() / 11;

	GLuint VBO = createVertexBuffer(vertbuffer);
	GLuint VAO = createVertexArray(VBO);

	return VAO;
}



int generateTexture(string filePath)
{
	GLuint texID;

	// Gera o identificador da textura na memória
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3) //jpg, bmp
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,
				data);
		}
		else //png
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
				data);
		}
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

