#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

using namespace std;

string equation1, equation2, equation3;
const int N = 1; //方程组维度

float Limitaion = 5.0f; //矩形框边界

float argumentedMatrix[N][4];	//增广矩阵

float sortedVertices[4 * 3]; //排序后三角形顶点（最多4个三角形，每个三角形3个顶点）

class Vector3;
float dot(Vector3 v1, Vector3 v2);
float cosAngle(Vector3 v1, Vector3 v2);

template <class Type>
Type stringToNum(const string& str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

//三维向量
class Vector3
{
public:
	Vector3();
	Vector3(float x, float y, float z);
	Vector3(float beginPoint[3], float endPoint[3]);
	Vector3(float point[3]);
	float x, y, z, length; //坐标及模长

private:
	void Length()
	{
		length = sqrt(x * x + y * y + z * z);
	}
};
//无参数构造函数
Vector3::Vector3()
{

}

//用坐标构造向量
Vector3::Vector3(float _x, float _y, float _z) :x(_x), y(_y), z(_z)
{
	Length();
}
//用起始点构造向量
Vector3::Vector3(float beginPoint[3], float endPoint[3])
{
	x = endPoint[0] - beginPoint[0];
	y = endPoint[1] - beginPoint[1];
	z = endPoint[2] - beginPoint[2];
	Length();
}
//点坐标
Vector3::Vector3(float point[3]) :x(point[0]), y(point[1]), z(point[2])
{
	length = 0;
}

class Input
{
public:
	Input();

	//将输入的数学格式的线性方程组转化为增广矩阵
	void convert2matrix(string row1, string row2, string row3)
	{
		string row; //暂时保存方程组字符串
		string temp; //暂时存储string类型的数字
		int i = 0; //外循环
		int j = 0; //内循环
		for (i = 0; i < N; i++)
		{
			switch (i)
			{
			case 0:
				row = row1;
				break;
			case 1:
				row = row2;
				break;
			case 2:
				row = row3;
				break;
			default:
				break;
			}

			for (auto c : row)
			{
				//判断是否为未知数
				if (isalpha(c))
				{
					//当系数绝对值为1时
					if (temp == "-" || temp == "+" || temp == "")
					{
						temp += '1';
					}
					//将转化为float类型的数字保存在矩阵中
					argumentedMatrix[i][j++] = stringToNum<float>(temp);
					temp = "";
				}
				else if (c != '=')
				{
					temp += c;
				}
			}
			//把等号后面的常数保存到矩阵
			argumentedMatrix[i][j] = stringToNum<float>(temp);
			temp = "";

			//将内循环复位
			j = 0;

			cout << argumentedMatrix[i][0] << '\t';
			cout << argumentedMatrix[i][1] << '\t';
			cout << argumentedMatrix[i][2] << '\t';
			cout << argumentedMatrix[i][3] << '\t' << endl;
		}
	}
};

Input::Input()
{
	cout << "请输入方程组" << endl;
	cout << "方程1：";
	cin >> equation1;
	//cout << "方程2：";
	//cin >> equation2;
	//cout << "方程3：";
	//cin >> equation3;
}

class Vertex
{
public:
	float intersection[6][3]; //交点
	int counter = 0; //记录有效交点数量
	int findIntersections(float coefficients[4])
	{
		//找到pX面的顶点
		if (findValidinter(limit, 0.0f, limit, 1, coefficients))
			counter++;
		if (findValidinter(limit, 0.0f, -limit, 1, coefficients))
			counter++;
		if (findValidinter(limit, limit, 0.0f, 2, coefficients))
			counter++;
		if (findValidinter(limit, -limit, 0.0f, 2, coefficients))
			counter++;
		//找到X面的顶点
		if (findValidinter(0.0f, limit, limit, 0, coefficients))
			counter++;
		if (findValidinter(0.0f, limit, -limit, 0, coefficients))
			counter++;
		if (findValidinter(0.0f, -limit, -limit, 0, coefficients))
			counter++;
		if (findValidinter(0.0f, -limit, limit, 0, coefficients))
			counter++;
		//找到nX面的顶点
		if (findValidinter(-limit, 0.0f, limit, 1, coefficients))
			counter++;
		if (findValidinter(-limit, 0.0f, -limit, 1, coefficients))
			counter++;
		if (findValidinter(-limit, limit, 0.0f, 2, coefficients))
			counter++;
		if (findValidinter(-limit, -limit, 0.0f, 2, coefficients))
			counter++;

		return counter;
	}
private:
	float limit = Limitaion;
	bool findValidinter(float x, float y, float z, int index, float coefcts[4])
	{
		bool isValid = false; //保存交点是否有效
		float inter[3]; //交点坐标
		float variables[3] = { x,y,z };

		if (index == 0) {
			inter[index + 1] = y;
			inter[index + 2] = z;
			inter[index] = (coefcts[3] - coefcts[index + 1] * variables[index + 1] - coefcts[index + 2] * variables[index + 2]) / coefcts[index];
		}
		else if (index == 1) {
			inter[index - 1] = x;
			inter[index + 1] = z;
			inter[index] = (coefcts[3] - coefcts[index - 1] * variables[index - 1] - coefcts[index + 1] * variables[index + 1]) / coefcts[index];
		}
		else if (index == 2) {
			inter[index - 2] = x;
			inter[index - 1] = y;
			inter[index] = (coefcts[3] - coefcts[index - 2] * variables[index - 2] - coefcts[index - 1] * variables[index - 1]) / coefcts[index];
		}
		if (inter[index] <= limit && inter[index] >= -limit) {
			if (counter == 0)
			{
				isValid = true;
				intersection[counter][0] = inter[0];
				intersection[counter][1] = inter[1];
				intersection[counter][2] = inter[2];
			}
			else
			{
				for (int i = 0; i < counter; i++) 
				{
					int j = 0;
					while (true)
					{
						if (intersection[i][j] == inter[j])
						{
							if (j == 2)
							{
								isValid = false;
								return isValid;
							}
							else
							{
								j++;
							}
						}
						else 
							break;
					}
				}
				isValid = true;
				intersection[counter][0] = inter[0];
				intersection[counter][1] = inter[1];
				intersection[counter][2] = inter[2];
			}
		}
		return isValid;
	}
};



void equation()
{
	Input input;
	input.convert2matrix(equation1, equation2, equation3);

	Vertex vertices;
	int num; //有效交点的数量
	num = vertices.findIntersections(argumentedMatrix[0]);

	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			cout << vertices.intersection[i][j] << ',';

		}
		cout << endl;
	}
}


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Cal3D", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("7.4.camera.vs", "7.4.camera.fs");

	equation();

	float ver[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float *vertices = ver;

	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 180, vertices, GL_DYNAMIC_DRAW);

	//position attributes
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//texture attributes
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	
	unsigned int texture1, texture2;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannles;
	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannles, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	data = stbi_load("resources/textures/awesomeface.png", &width, &height, &nrChannles, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	ourShader.use();
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render the triangle
		ourShader.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		ourShader.setMat4("projection", projection);

		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("view", view);

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++)
		{
			glm::mat4 model;
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5f*deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates rang from bottom to top
	
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}