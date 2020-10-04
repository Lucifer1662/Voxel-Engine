#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Program.h"
#include "Buffer.h"
#include <string>
#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "Chunk.h"
#include <soil/SOIL.h>


glm::vec3 cameraPos = { 4.0f, 6.0f, 25.0f };
float delta = 0.05f;
float speed = 5;

void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods) {

	

	switch (key)
	{
	case GLFW_KEY_A:
		cameraPos.x -= delta * speed;
		break;
	case GLFW_KEY_D:
		cameraPos.x += delta * speed;
		break;
	case GLFW_KEY_S:
		cameraPos.z += delta * speed;
		break;
	case GLFW_KEY_W:
		cameraPos.z -= delta * speed;
		break;
	case GLFW_KEY_LEFT_SHIFT:
		cameraPos.y -= delta * speed;
		break;
	case GLFW_KEY_SPACE:
		cameraPos.y += delta * speed;
		break;
	default:
		break;
	}
}



void main() {

	glfwInit();
	GLFWwindow* window = glfwCreateWindow(600, 600, "Voxel Engine", 0, 0);
	glfwMakeContextCurrent(window);
	glewInit();

	std::string vs = R"(
#version 330 core
layout (location = 0) in vec3 pos; // the position variable has attribute position 0
  


void main()
{
    gl_Position = vec4(pos,1); 
}
)";
	std::string fs = R"(

#version 330 core
out vec4 FragColor;

in vec2 uv;
uniform sampler2D blockTexture;



void main()
{
    FragColor = texture(blockTexture, uv);// vec4(uv, 0, 1);//vertexColor+vec4(0.1f);
} 
)";

	std::string gs = R"(
#version 430 core
layout (points) in;
layout (triangle_strip, max_vertices = 50) out;

uniform mat4 camera;
out vec2 uv;
ivec3 position;


layout(std430, binding = 3) buffer chunkData
 {
//32*32*128
     int blockType[100];
 };

struct Block{
	vec4 uvFaces[6];
};

layout(std430, binding = 1) buffer blockTypes
 {
     Block blocks[3];
 };

int toIndex(int x, int y, int z){
	if(x < 0 || x >= 10 || y < 0 || y > 0 || z < 0 || z >= 10)
		return -1;
	return z + x*10;
}

int toIndex(ivec3 pos){
	return toIndex(pos.x, pos.y, pos.z);
}

int toIndex(float x, float y, float z){
	return toIndex(int(x), int(y), int(z));
}


void face(int face, ivec3 offset, vec3 a, vec3 b, vec3 c, vec3 d){
	
	int i = toIndex(position);
	int index = toIndex(position + offset);


	if(blockType[i] == 1)
		return;
	
	if(index != -1){
		if(blockType[index] == 1 ){
			return;
		}
	}

	vec4 uvs = blocks[blockType[i]].uvFaces[face];
	
	
	uv = uvs.xy;
	gl_Position = camera*(gl_in[0].gl_Position + vec4(a,0));
    EmitVertex();

	uv = uvs.zy;
	gl_Position = camera*(gl_in[0].gl_Position + vec4(b,0));
    EmitVertex();

	uv = uvs.xw;
	gl_Position = camera*(gl_in[0].gl_Position + vec4(c,0));
    EmitVertex();

	uv = uvs.zw;
	gl_Position = camera*(gl_in[0].gl_Position + vec4(d,0));
    EmitVertex();

EndPrimitive();
}




void main() {    
	position = ivec3(gl_in[0].gl_Position);

	//int index = toIndex(gl_in[0].gl_Position.x, gl_in[0].gl_Position.y, gl_in[0].gl_Position.z);
	//if(vertexColor.w < 0.001f)
	//	return;
	

	face(0,ivec3(0,0,-1),
	vec3(1,0,0),  vec3(0,0,0),vec3(1,1,0),  vec3(0,1,0));//front

	face(1,ivec3(1,0,0),
	vec3(1,0,1), vec3(1,0,0),vec3(1,1,1),   vec3(1,1,0));//right
	
	face(2,ivec3(0,0,1),
	vec3(0,0,1),  vec3(1,0,1), vec3(0,1,1), vec3(1,1,1));//back

	face(3,ivec3(-1,0,0),
	vec3(0,0,0), vec3(0,0,1), vec3(0,1,0), vec3(0,1,1));//left

	face(4,ivec3(0,1,0),
	vec3(0,1,1), vec3(1,1,1),  vec3(0,1,0),  vec3(1,1,0));//top

	face(5,ivec3(0,-1,0),
	vec3(0,0,0),  vec3(1,0,0), vec3(0,0,1), vec3(1,0,1));//bottom


    
    
} 
)";

	
	glPointSize(4);
	Blocks blocks;
	blocks.BlockTypesLoad();
	Chunk chunk(3, {0,0,0});



	auto p = CreateProgram(vs, gs, fs);
	int posLocation = glGetAttribLocation(p, "pos");

	glVertexAttribPointer(posLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(posLocation);
	glUseProgram(p);
	int cameraLocation = glGetUniformLocation(p, "camera");
	

	glfwSetKeyCallback(window, keyCallback);


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	/* load an image file directly as a new OpenGL texture */
	GLuint tex_2d = SOIL_load_OGL_texture
	(
		"wood.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
	);


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		auto camera = glm::perspective(45.0f, 1.0f, 0.01f, 40.0f) * glm::lookAt(cameraPos, cameraPos + glm::vec3(0, 0.2f, -1), { 0.0f,1.0f,0.0f });

		glUniformMatrix4fv(cameraLocation, 1, GL_FALSE, &camera[0][0]);


		chunk.Render();
		glfwSwapBuffers(window);

	}
	blocks.BlockTypesUnload();
	glfwDestroyWindow(window);

}