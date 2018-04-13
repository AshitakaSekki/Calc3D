#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opengl/shader_m.h>
#include <opengl/camera.h>
#include "Vector3.h"

#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"

#include <iostream>
#include <string>
#include <cctype>
#include <sstream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

void rotation(float xoffset, float yoffset);

const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

std::string equation1, equation2, equation3;
const int N = 1; //方程组维度

float Limitaion = 5.0f; //矩形框边界

float argumentedMatrix[N][4];	//增广矩阵

float sortedVertices[4 * 3]; //排序后三角形顶点（最多4个三角形，每个三角形3个顶点）


template <class Type>
Type stringToNum(const std::string& str)
{
	std::istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}





class Input
{
public:
	Input();

	//将输入的数学格式的线性方程组转化为增广矩阵
	void convert2matrix(std::string row1, std::string row2, std::string row3)
	{
		std::string row; //暂时保存方程组字符串
		std::string temp; //暂时存储string类型的数字
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

			std::cout << argumentedMatrix[i][0] << '\t';
			std::cout << argumentedMatrix[i][1] << '\t';
			std::cout << argumentedMatrix[i][2] << '\t';
			std::cout << argumentedMatrix[i][3] << '\t' << std::endl;
		}
	}
};

Input::Input()
{
	std::cout << "请输入方程组" << std::endl;
	std::cout << "方程1：";
	std::cin >> equation1;
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
			std::cout << vertices.intersection[i][j] << ',';

		}
		std::cout << std::endl;
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
	glfwSwapInterval(1); //开启垂直同步
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	//设置imgui
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfwGL3_Init(window, true);

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Shader coordFrame("coordFrame.vs", "coordFrame.fs");

	//equation();

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

	glm::vec3 cubePositions(0.0f, 0.0f, 0.0f);

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

	coordFrame.use();
	coordFrame.setInt("texture1", 0);
	coordFrame.setInt("texture2", 1);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glfwPollEvents();
		ImGui_ImplGlfwGL3_NewFrame();

		processInput(window);

		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
		{
			static float f = 0.0f;
			static int counter = 0;
			ImGui::Text("Hello, world!");                           // Display some text (you can use a format string too)
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
			ImGui::Checkbox("Another Window", &show_another_window);

			if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
				counter++;
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
		if (show_another_window)
		{
			ImGui::Begin("Another Window", &show_another_window);
			ImGui::Text("Hello from another window!");
			if (ImGui::Button("Close Me"))
				show_another_window = false;
			ImGui::End();
		}

		// 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
		if (show_demo_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_demo_window);
		}


		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// render the triangle
		coordFrame.use();

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//glm::mat4 projection = glm::ortho(0.0f, (float)SCR_WIDTH, (float)SCR_HEIGHT, 0.0f, 0.1f, 100.0f);
		coordFrame.setMat4("projection", projection);

		int a;

		glm::mat4 view = camera.GetViewMatrix();
		coordFrame.setMat4("view", view);

		glBindVertexArray(VAO);

		glm::mat4 model;
		model = glm::translate(model, cubePositions);
		coordFrame.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		//glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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

	float xoffset = lastX - xpos;
	float yoffset = ypos - lastY; // reversed since y-coordinates rang from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		camera.ProcessMouseDrag(UP, yoffset);
		camera.ProcessMouseDrag(RIGHT, xoffset);
	}

}

void rotation(float xoffset, float yoffset)
{
	
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
	std::cout << "success" << std::endl;
}