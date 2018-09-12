/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt
*/

#include <iostream>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Camera.h"
#include "Timeline.hpp"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;
shared_ptr<Shape> skySphere;


#define DRAW_LINES false
#define DRAW_GREY false
// When doing a full implementation, this should be a member variable in a planet class
// Currently, this value is not passed through to the atmosphere, just copied from the value in the shader. Change that in implementation for variable planet sizes
#define PLANET_RADIUS 6372e3

#define WATERSIZE 1
#define W_RESOLUTION (2.0f * MESHSIZE)

// This should be extracted to its own header
class Sun {
public:
	glm::vec3 position;
	glm::vec3 color;

	// Change the position of the sun over time
	void update(float dt) {
		time += dt * 0.1f;
		//Position the sun in the sky far away and have it move based on the time
		position = vec3(0.0f, cos(time), sin(time)) * 1000000.0f;
	}
protected:
	float time = 0.0f;
};


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> heightshader, progWater, progAtmos, worldSphereShader;

	// Contains vertex information for OpenGL
	GLuint TerrainVertexArrayID;
	GLuint WaterVertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint TerrainPosID, TerrainTexID, IndexBufferIDBox;
	GLuint WaterPosID, WaterTexID, WaterIndexBufferIDBox;

	//texture data
	GLuint GrassTexture, SnowTexture, SandTexture, CliffTexture;
	GLuint GrassNormal, SnowNormal, SandNormal, CliffNormal;
	//This should be removed and code using this should have update(float dt) methods instead
	float time = 1.0;
	FPcamera mycam;

	shared_ptr<Shape> atmosQuad, worldSphere;
	Sun sun;
	const float pi = 3.14159265f;
	bool drawSphereToggle = false;


	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			 update(pi * 10);
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			drawSphereToggle = !drawSphereToggle;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{

	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	void init_mesh()
	{
		//generate the VAO
		glGenVertexArrays(1, &TerrainVertexArrayID);
		glBindVertexArray(TerrainVertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &TerrainPosID);
		glBindBuffer(GL_ARRAY_BUFFER, TerrainPosID);

		// Size of the net mesh squared (grid) times 4 (verticies per rectangle)
		vec3 *vertices = new vec3[MESHSIZE * MESHSIZE * 4];

		for (int x = 0; x < MESHSIZE; x++)
			for (int z = 0; z < MESHSIZE; z++)
			{
				vertices[x * 4 + z * MESHSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) * RESOLUTION + vec3(x, 0, z) * RESOLUTION;
				vertices[x * 4 + z * MESHSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) * RESOLUTION + vec3(x, 0, z) * RESOLUTION;
				vertices[x * 4 + z * MESHSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) * RESOLUTION + vec3(x, 0, z) * RESOLUTION;
				vertices[x * 4 + z * MESHSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) * RESOLUTION + vec3(x, 0, z) * RESOLUTION;
			}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * MESHSIZE * MESHSIZE * 4, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//tex coords
		float t = RESOLUTION / 100;

		vec2 *tex = new vec2[MESHSIZE * MESHSIZE * 4];
		for (int x = 0; x < MESHSIZE; x++)
			for (int y = 0; y < MESHSIZE; y++)
			{
				tex[x * 4 + y * MESHSIZE * 4 + 0] = vec2(0.0, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 1] = vec2(t, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 2] = vec2(t, t) + vec2(x, y) * t;
				tex[x * 4 + y * MESHSIZE * 4 + 3] = vec2(0.0, t) + vec2(x, y) * t;
			}
		glGenBuffers(1, &TerrainTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, TerrainTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * MESHSIZE * MESHSIZE * 4, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//free(tex);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);

		GLuint *elements = new GLuint[MESHSIZE * MESHSIZE * 6];
		int ind = 0;
		for (int i = 0; i < MESHSIZE * MESHSIZE * 6; i += 6, ind += 4)
		{
			elements[i + 0] = ind + 0;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + 2;
			elements[i + 3] = ind + 0;
			elements[i + 4] = ind + 2;
			elements[i + 5] = ind + 3;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * MESHSIZE * MESHSIZE * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}


	void init_water()
	{
		//generate the VAO
		glGenVertexArrays(1, &WaterVertexArrayID);
		glBindVertexArray(WaterVertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &WaterPosID);
		glBindBuffer(GL_ARRAY_BUFFER, WaterPosID);

		// Size of the net mesh squared (grid) times 4 (verticies per rectangle)
		vec3 *vertices = new vec3[WATERSIZE * WATERSIZE * 4];

		for (int x = 0; x < WATERSIZE; x++)
			for (int z = 0; z < WATERSIZE; z++)
			{
				vertices[x * 4 + z * WATERSIZE * 4 + 0] = vec3(0.0, 0.0, 0.0) * W_RESOLUTION + vec3(x, 0, z) * W_RESOLUTION;
				vertices[x * 4 + z * WATERSIZE * 4 + 1] = vec3(1.0, 0.0, 0.0) * W_RESOLUTION + vec3(x, 0, z) * W_RESOLUTION;
				vertices[x * 4 + z * WATERSIZE * 4 + 2] = vec3(1.0, 0.0, 1.0) * W_RESOLUTION + vec3(x, 0, z) * W_RESOLUTION;
				vertices[x * 4 + z * WATERSIZE * 4 + 3] = vec3(0.0, 0.0, 1.0) * W_RESOLUTION + vec3(x, 0, z) * W_RESOLUTION;
			}
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * WATERSIZE * WATERSIZE * 4, vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//tex coords
		float t = W_RESOLUTION / 100;

		vec2 *tex = new vec2[WATERSIZE * WATERSIZE * 4];
		for (int x = 0; x < WATERSIZE; x++)
			for (int y = 0; y < WATERSIZE; y++)
			{
				tex[x * 4 + y * WATERSIZE * 4 + 0] = vec2(0.0, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * WATERSIZE * 4 + 1] = vec2(t, 0.0) + vec2(x, y) * t;
				tex[x * 4 + y * WATERSIZE * 4 + 2] = vec2(t, t) + vec2(x, y) * t;
				tex[x * 4 + y * WATERSIZE * 4 + 3] = vec2(0.0, t) + vec2(x, y) * t;
			}
		glGenBuffers(1, &WaterTexID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, WaterTexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * WATERSIZE * WATERSIZE * 4, tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//free(tex);

		glGenBuffers(1, &WaterIndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WaterIndexBufferIDBox);

		GLuint *elements = new GLuint[WATERSIZE * WATERSIZE * 6];
		int ind = 0;
		for (int i = 0; i < WATERSIZE * WATERSIZE * 6; i += 6, ind += 4)
		{
			elements[i + 0] = ind + 0;
			elements[i + 1] = ind + 1;
			elements[i + 2] = ind + 2;
			elements[i + 3] = ind + 0;
			elements[i + 4] = ind + 2;
			elements[i + 5] = ind + 3;
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * WATERSIZE * WATERSIZE * 6, elements, GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//initialize the net mesh
		init_mesh();
		init_water();

		string resourceDirectory = "../resources";

		InitTextures(resourceDirectory);

		InitWorldSphere(resourceDirectory);

		InitAtmosphereQuad(resourceDirectory);

		InitTextureLocations();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	}

	void InitTextures(const std::string &resourceDirectory)
	{
		// Grass texture
		InitTexture((resourceDirectory + "/grass.jpg").c_str(), GrassTexture);

		// Grass normal map
		InitTexture((resourceDirectory + "/grass_normal.png").c_str(), GrassNormal);

		// Snow texture
		InitTexture((resourceDirectory + "/snow.jpg").c_str(), SnowTexture);

		// Snow normal map
		InitTexture((resourceDirectory + "/snow_normal.png").c_str(), SnowNormal);

		// Sand texture
		InitTexture((resourceDirectory + "/sand.jpg").c_str(), SandTexture);

		// Sand normal map
		InitTexture((resourceDirectory + "/sand_normal.png").c_str(), SandNormal);

		// Cliff texture
		InitTexture((resourceDirectory + "/cliff.jpg").c_str(), CliffTexture);

		// Cliff normal map
		InitTexture((resourceDirectory + "/cliff_normal.png").c_str(), CliffNormal);
	}

	void InitAtmosphereQuad(const std::string &resourceDirectory)
	{
		// Initialize mesh.
		atmosQuad = make_shared<Shape>();
		atmosQuad->loadMesh(resourceDirectory + "/quad.obj");
		atmosQuad->resize();
		atmosQuad->init();
	}

	void InitWorldSphere(const std::string &resourceDirectory)
	{
		// Initialize mesh.
		worldSphere = make_shared<Shape>();
		worldSphere->loadMesh(resourceDirectory + "/HighResSphere.obj");
		worldSphere->resize();
		worldSphere->init(true);
	}

	void InitTextureLocations()
	{
		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint GrassTextureLocation, SnowTextureLocation, SandTextureLocation, CliffTextureLocation, SkyTextureLocation, NightTextureLocation;
		GLuint GrassNormalLocation, SnowNormalLocation, SandNormalLocation, CliffNormalLocation;

		GrassTextureLocation = glGetUniformLocation(heightshader->pid, "grassSampler");
		GrassNormalLocation = glGetUniformLocation(heightshader->pid, "grassNormal");
		SnowTextureLocation = glGetUniformLocation(heightshader->pid, "snowSampler");
		SnowNormalLocation = glGetUniformLocation(heightshader->pid, "snowNormal");
		SandTextureLocation = glGetUniformLocation(heightshader->pid, "sandSampler");
		SandNormalLocation = glGetUniformLocation(heightshader->pid, "sandNormal");
		CliffTextureLocation = glGetUniformLocation(heightshader->pid, "cliffSampler");
		CliffNormalLocation = glGetUniformLocation(heightshader->pid, "cliffNormal");
		// Then bind the uniform samplers to texture units:
		glUseProgram(heightshader->pid);
		glUniform1i(GrassTextureLocation, 0);
		glUniform1i(SnowTextureLocation, 1);
		glUniform1i(SandTextureLocation, 2);
		glUniform1i(CliffTextureLocation, 3);
		glUniform1i(CliffNormalLocation, 4);
		glUniform1i(SnowNormalLocation, 5);
		glUniform1i(GrassNormalLocation, 6);
		glUniform1i(SandNormalLocation, 7);

	}

	void InitTexture(const char * filepath, GLuint & texture)
	{
		int width, height, channels;
		unsigned char * data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		InitProgs(resourceDirectory);
	}

	// Initilize the shaders
	void InitProgs(const std::string & resourceDirectory)
	{
		InitHeightShader(resourceDirectory);

		InitWaterShader(resourceDirectory);

		InitAtmosphereShader(resourceDirectory);

		InitWorldSphereShader(resourceDirectory);
	}

	void InitHeightShader(const std::string & resourceDirectory)
	{
		// Initialize the GLSL program.
		heightshader = std::make_shared<Program>();
		heightshader->setVerbose(true);
		heightshader->setShaderNames(resourceDirectory + "/height_vertex.glsl", resourceDirectory + "/height_frag.glsl", resourceDirectory + "/tesscontrol.glsl", resourceDirectory + "/tesseval.glsl");
		if (!heightshader->init())
		{
			std::cerr << "Heightmap shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		heightshader->addUniform("P");
		heightshader->addUniform("V");
		heightshader->addUniform("M");
		heightshader->addUniform("camoff");
		heightshader->addUniform("uSunPos");
		heightshader->addUniform("campos");
		heightshader->addUniform("time");
		heightshader->addUniform("resolution");
		heightshader->addUniform("meshsize");
		heightshader->addUniform("drawGrey");
		heightshader->addAttribute("vertPos");
		heightshader->addAttribute("vertTex");
	}

	void InitWorldSphereShader(const std::string & resourceDirectory)
	{
		// Initialize the GLSL program.
		worldSphereShader = std::make_shared<Program>();
		worldSphereShader->setVerbose(true);
		worldSphereShader->setShaderNames(resourceDirectory + "/PhongVert.glsl", resourceDirectory + "/PhongFrag.glsl");
		if (!worldSphereShader->init())
		{
			std::cerr << "World Sphere shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		worldSphereShader->addUniform("P");
		worldSphereShader->addUniform("V");
		worldSphereShader->addUniform("M");
		worldSphereShader->addUniform("uSunPos");
		worldSphereShader->addUniform("campos");
		worldSphereShader->addAttribute("vertPos");
		worldSphereShader->addAttribute("vertNor");
		worldSphereShader->addAttribute("vertTex");
	}

	void InitAtmosphereShader(const std::string & resourceDirectory)
	{
		// Initialize the GLSL program.
		progAtmos = std::make_shared<Program>();
		progAtmos->setVerbose(true);
		progAtmos->setShaderNames(resourceDirectory + "/atmosphere_vert.glsl", resourceDirectory + "/atmosphere_frag.glsl");
		if (!progAtmos->init())
		{
			std::cerr << "Skybox shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		progAtmos->addUniform("M");
		progAtmos->addUniform("V");
		progAtmos->addUniform("P");
		progAtmos->addUniform("campos");
		progAtmos->addUniform("uSunPos");
		progAtmos->addAttribute("vertPos");
		progAtmos->addAttribute("vertTex");
	}

	void InitWaterShader(const std::string & resourceDirectory)
	{
		// Initialize the GLSL program.
		progWater = std::make_shared<Program>();
		progWater->setVerbose(true);
		progWater->setShaderNames(resourceDirectory + "/water_vertex.glsl", resourceDirectory + "/water_fragment.glsl");
		if (!progWater->init())
		{
			std::cerr << "Water shaders failed to compile... exiting!" << std::endl;
			int hold;
			cin >> hold;
			exit(1);
		}
		progWater->addUniform("P");
		progWater->addUniform("V");
		progWater->addUniform("M");
		progWater->addUniform("camoff");
		progWater->addUniform("campos");
		progWater->addUniform("time");
		progWater->addAttribute("vertPos");
		progWater->addAttribute("vertTex");
	}

	//Go therough the animated objects in the scene and update them
	void update(float dt) 
	{
		mycam.update(windowManager->getHandle(), dt);
		sun.update(dt);
	}

	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		glm::mat4 V, P; //View and Perspective matrix

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		
		InitRender(width, height);

		InitMatricies(V, P, width, height);

		vec3 offset = setOffset();

		DrawAtmosphere(P);

		if (drawSphereToggle) {
			DrawSphere(P, V);
		}
		else {
			if (!DRAW_LINES) {
				DrawWater(P, V, offset);
			}

			DrawTerrain(P, V, offset);
		}

		heightshader->unbind();
		glBindVertexArray(0);

	}

	glm::vec3 setOffset() 
	{
		vec3 offset = -mycam.getLocation();
		offset.x = ((int)(offset.x / RESOLUTION)) * RESOLUTION;
		offset.y = 0;
		offset.z = ((int)(offset.z / RESOLUTION)) * RESOLUTION;
		return offset;
	}

	void DrawWater(const glm::mat4 &P, const glm::mat4 &V, const glm::vec3 &offset)
	{
		// Draw the Water -----------------------------------------------------------------
		if (DRAW_GREY == false || DRAW_LINES == false) {
			progWater->bind();

			//glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
			glm::mat4 M = getModelMatrix(); 
			glUniformMatrix4fv(progWater->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			glUniformMatrix4fv(progWater->getUniform("V"), 1, GL_FALSE, &V[0][0]);
			glUniformMatrix4fv(progWater->getUniform("P"), 1, GL_FALSE, &P[0][0]);

			glUniform3fv(progWater->getUniform("camoff"), 1, &offset[0]);
			glUniform3fv(progWater->getUniform("campos"), 1, &mycam.getLocation()[0]);
			glUniform1f(progWater->getUniform("time"), time);
			glBindVertexArray(WaterVertexArrayID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, WaterIndexBufferIDBox);
			//glDisable(GL_DEPTH_TEST);
			// Must use gl_patches w/ tessalation
			//glPatchParameteri(GL_PATCH_VERTICES, 3.0f);
			glDrawElements(GL_TRIANGLES, MESHSIZE*MESHSIZE * 6, GL_UNSIGNED_INT, (void*)0);
			//glEnable(GL_DEPTH_TEST);
			progWater->unbind();
		}
	}

	void DrawTerrain(const glm::mat4 &P, const glm::mat4 &V, const glm::vec3 &offset)
	{
		// GL POLYGON _________________________________________________
		if (DRAW_LINES) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		// ____________________________________________________________
		// Draw the terrain -----------------------------------------------------------------
		heightshader->bind();

		glm::mat4 M = getModelMatrix();
		glUniformMatrix4fv(heightshader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(heightshader->getUniform("P"), 1, GL_FALSE, &P[0][0]);


		glUniform3fv(heightshader->getUniform("camoff"), 1, &offset[0]);
		glUniform3fv(heightshader->getUniform("uSunPos"), 1, &sun.position[0]);
		glUniform3fv(heightshader->getUniform("campos"), 1, &mycam.getLocation()[0]);
		glUniform1f(heightshader->getUniform("time"), time);
		glUniform1i(heightshader->getUniform("meshsize"), MESHSIZE);
		glUniform1f(heightshader->getUniform("resolution"), RESOLUTION);
		glUniform1i(heightshader->getUniform("drawGrey"), DRAW_GREY);
		glBindVertexArray(TerrainVertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GrassTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, SnowTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, SandTexture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, CliffTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, CliffNormal);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, SnowNormal);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, GrassNormal);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, SandNormal);

		glPatchParameteri(GL_PATCH_VERTICES, 3);
		glDrawElements(GL_PATCHES, MESHSIZE*MESHSIZE * 6, GL_UNSIGNED_INT, (void*)0);

		//The terrain is the only part we want lines for
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void DrawSphere(const glm::mat4 &P, const glm::mat4 &V) {
		worldSphereShader->bind();

		//For this implementation, we place the top of the world at (0,0,0), so the planet needs to be translated down to match this value
		glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -PLANET_RADIUS, 0.0f));
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(PLANET_RADIUS));
		glm::mat4 M = T * S;
		glUniformMatrix4fv(worldSphereShader->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(worldSphereShader->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(worldSphereShader->getUniform("P"), 1, GL_FALSE, &P[0][0]);

		glUniform3fv(worldSphereShader->getUniform("campos"), 1, &mycam.getLocation()[0]);
		glUniform3f(worldSphereShader->getUniform("uSunPos"), sun.position.x, sun.position.y, sun.position.z);
		worldSphere->draw(worldSphereShader, false, true);
		worldSphereShader->unbind();
	}

	void DrawAtmosphere(const glm::mat4 &P)
	{
		// Draw the sky
		progAtmos->bind();

		glm::mat4 V = SetAtmosphereView();
		//This is positioning the quad at the near plane of the perspective view frustum 
		glm::mat4 M = calcualteFrustamNearBounds(mycam.getFOV(), mycam.getNearDist(), mycam.getAspect());

		//send the matrices to the shaders 
		glUniformMatrix4fv(progAtmos->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniformMatrix4fv(progAtmos->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(progAtmos->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniform3fv(progAtmos->getUniform("campos"), 1, &mycam.getLocation()[0]);
		glUniform3fv(progAtmos->getUniform("uSunPos"), 1, &sun.position[0]);

		glDisable(GL_DEPTH_TEST);
		atmosQuad->draw(progAtmos);

		glEnable(GL_DEPTH_TEST);

		progAtmos->unbind();
	}

	// This takes a unit quad and places it at the near plane 
	//      of the view frustum with the same dimensions
	// This does not rotate the plane with the camera
	glm::mat4 calcualteFrustamNearBounds(float fovy, float near, float aspect) {
		float hypot = near / sin(fovy * 2 * pi / 360);
		float angle = 90.0f - fovy;
		//side is half the height of the near plane
		float side = sin(angle * 2 * pi / 360) * hypot;

		auto S = glm::scale(glm::mat4(1.0f), glm::vec3(side * aspect, side, 1.0f));
		auto T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, near));
		return S * T;
	}

	// Returns the rotation matrix to match the camera's rotation for the atmosphere plane
	// Assumes two degrees of freedom for the camera, pitch and yaw. Roll is not supported currently.
	glm::mat4 SetAtmosphereView()
	{
		auto R = glm::rotate(glm::mat4(1.0f), -mycam.getTheta(), glm::vec3(0.0f, 1.0f, 0.0f));
		R *= glm::rotate(glm::mat4(1.0f), mycam.getPhi(), glm::vec3(1.0f, 0.0f, 0.0f));
		return R;
	}

	glm::mat4 getModelMatrix() {
		float centerOffset = -MESHSIZE * RESOLUTION / 2.0f;
		return glm::translate(glm::mat4(1.0f), glm::vec3(centerOffset, 2.0f, centerOffset));
	}

	// Sets the View (V) and Perspective (P) matricies
	void InitMatricies(glm::mat4 &V, glm::mat4 &P, int width, int height)
	{
		V = mycam.getView();
		P = mycam.getPerspective(width / (float)height);
	}

	void InitRender(int width, int height)
	{
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		setTime();
	}

	void setTime()
	{
		if (DRAW_LINES == false) {
			time += 0.004f;
		}
	}

};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}
	Timeline<double> time;
	time.setSource(&glfwGetTime);

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
		// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	time.reset();
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		application->update((float) time.elapsed());
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
