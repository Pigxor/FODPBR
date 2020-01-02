#define STB_IMAGE_IMPLEMENTATION

#include "fodPBR/metaHeader.h"
#include "fodPBR/Camera.h"
#include "fodPBR/Shapes.h"
#include "fodPBR/Texture.h"
#include "fodPBR/Shader.h"
#include "fodPBR/RenderTexture.h"

///Following Tutorial found at "https://learnopengl.com/PBR/Theory"

bool quit = false;
bool cursorLock = true;
float lastTime;
float timet;
bool textured;

int main(int argc, char *argv[])
{
	int choice = 0;
	while (true)
	{
		std::cout << "Press 1 for untextured or 2 for textured - ";
		std::cin >> choice;
		if (choice == 1 || choice == 2)
		{
			break;
		}
	}
	switch (choice)
	{
	case 1:
		textured = false;
		break;
	case 2:
		textured = true;
		break;
	default:
		break;
	}


	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception();
	}

	float timet;
	SDL_Window *window = SDL_CreateWindow("FODPBRDemo",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		windowWidth, windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	if (!SDL_GL_CreateContext(window))
	{
		throw std::exception();
	}
	if (glewInit() != GLEW_OK)
	{
		throw std::exception();
	}
	//
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_SetWindowGrab(window, SDL_TRUE);
	SDL_ShowCursor(0);
	lastTime = SDL_GetTicks();
	//

	//Cam1
	Camera camera;

	//Create the balls and cubemap
	Shape ball(true);
	Shape utBall(true);

	Shape cube(false);

	Shape quad(false);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	
	//Create all the shader objects
	Shader pbrTextured("../shaders/PBR.vs", "../shaders/PBR.fs");
	Shader utPBR("../shaders/utPBR.vs", "../shaders/utPBR.fs");
	Shader cubemapShader("../shaders/cubemap.vs", "../shaders/cubemap.fs");
	Shader irradianceShader("../shaders/irradiance.vs", "../shaders/irradiance.fs");
	Shader prefilter("../shaders/prefilter.vs", "../shaders/prefilter.fs");
	Shader brdf("../shaders/brdf.vs", "../shaders/brdf.fs");

	Shader skybox("../shaders/skyShader.vs", "../shaders/skyShader.fs");

	skybox.setActiveShader();
	skybox.setUniformInt("environmentMap", 0);

	Tex skyBoxTex("../models/ForestSky.hdr", true);


	unsigned int captureFBO;
	unsigned int captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	//make space in the memory to create some textures
	Tex cubeMap({ 1024,1024 }, 0); 
	Tex irCubeMap({ 32,32}, 0);
	Tex pfCubeMap({ 128,128 }, 1);
	Tex brdfTex({ 512,512 }, 3);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = //All the positions for the camera to 'look at' to form a cubemap
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	cubemapShader.setActiveShader();
	cubemapShader.setUniformInt("equirectangularMap", 0);
	cubemapShader.setUniformMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyBoxTex.getID());
	//Use the captures to create the appropriate textures
	glViewport(0, 0, 1024, 1024); 
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		cubemapShader.setUniformMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap.getID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.render();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	irradianceShader.setActiveShader();
	irradianceShader.setUniformInt("environmentMap", 0);
	irradianceShader.setUniformMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getID());
	glViewport(0, 0, 32, 32);
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.setUniformMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irCubeMap.getID(), 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		cube.render();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	prefilter.setActiveShader();
	prefilter.setUniformInt("environmentMap", 0);
	prefilter.setUniformMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.getID());

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilter.setUniformFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilter.setUniformMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, pfCubeMap.getID(), mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cube.render();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfTex.getID(), 0);
	glViewport(0, 0, 512, 512);
	brdf.setActiveShader();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	quad.renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	//Assign all the uniforms for the shaders
	pbrTextured.setActiveShader();
	pbrTextured.setUniformInt("albedoMap", 0);
	pbrTextured.setUniformInt("normalMap", 1);
	pbrTextured.setUniformInt("metallicMap", 2);
	pbrTextured.setUniformInt("roughnessMap", 3);
	pbrTextured.setUniformInt("aoMap", 4);
	pbrTextured.setUniformInt("irradianceMap", 5);
	pbrTextured.setUniformInt("prefilterMap", 6);
	pbrTextured.setUniformInt("brdfMap", 7);

	//Assign all the uniforms for the shaders
	utPBR.setActiveShader();
	utPBR.setUniformInt("irradianceMap", 0);
	utPBR.setUniformInt("prefilterMap", 1);
	utPBR.setUniformInt("brdfMap", 2);
	utPBR.setUniformVec3("albedo", { 1.0f, 1.0f,1.0f });
	utPBR.setUniformFloat("ao", 1.0f);

	//Textures for the textured balls
	Tex albedoMap("../models/albedoMap.png", false);
	Tex normalMap("../models/normalMap.png", false);
	Tex metallicMap("../models/metallicMap.png", false);
	Tex aoMap("../models/aoMap.png", false);
	Tex rufMap("../models/roughnessMap.png", false);

	//Assign the textures to the balls
	ball.addTex(albedoMap);
	ball.addTex(normalMap);
	if (textured)
	{
		ball.addTex(metallicMap);
		ball.addTex(rufMap);
	}
	ball.addTex(aoMap);
	ball.addTex(irCubeMap);
	ball.addTex(pfCubeMap);
	ball.addTex(brdfTex);

	//Give the 'untextured' balls the textures for ibl
	utBall.addTex(irCubeMap);
	utBall.addTex(pfCubeMap);
	utBall.addTex(brdfTex);

	//Assign the cubemap texture
	cube.addTex(cubeMap);

	//Four point lights 
	glm::vec3 lightPositions[] = {
		glm::vec3(-10.0f, 10.0f, 10.0f),
		glm::vec3(10.0f,  10.0f, 10.0f),
		glm::vec3(-10.0f, -10.0f, 10.0f),
		glm::vec3(10.0f, -10.0f, 10.0f),
	};
	glm::vec3 lightColors[] = {
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f),
		glm::vec3(300.0f, 300.0f, 300.0f)
	};
	//

	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	glViewport(0, 0, windowWidth, windowHeight);

	while (!quit) //'Game' loop
	{
		glm::mat4 view = camera.getView();
		glm::mat4 proj = camera.getProjection();
		glm::vec3 pos = camera.getPos();
		//Delta Time
		timet = SDL_GetTicks();
		float diff = timet - lastTime;
		float deltaTime = diff / 1000.0f;
		lastTime = timet;
		float idealTime = 1.0f / 60.0f;
		if (idealTime > deltaTime)
		{
			SDL_Delay((idealTime - deltaTime)*1000.0f);
		}
		//
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ball.bindTex();
		utBall.bindTex();

		if (textured)		///Texture Spheres
		{
	
			pbrTextured.setActiveShader();

			pbrTextured.setUniformVec3("camPos", pos);
			pbrTextured.setUniformMat4("view", view);
			glm::mat4 model = glm::mat4(1.0f);

			
			for (int row = 0; row < nrRows; ++row)
			{
				for (int col = 0; col < nrColumns; ++col)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(float)(col - (nrColumns / 2)) * spacing,
						(float)(row - (nrRows / 2)) * spacing,
						-8.0f
					));

					pbrTextured.setUniformMat4("projection", proj);
					pbrTextured.setUniformMat4("model", model);
					ball.render();
				}
			}

			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{

				pbrTextured.setUniformVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
				pbrTextured.setUniformVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.5f));
				pbrTextured.setUniformMat4("model", model);
				ball.render();
			}
		}
		

		if (!textured)	///Untextured Spheres
		{
			utPBR.setActiveShader();
			utPBR.setUniformVec3("camPos", pos);
			utPBR.setUniformMat4("view", view);


			glm::mat4 model = glm::mat4(1.0f);

			
			for (int row = 0; row < nrRows; ++row)
			{
				utPBR.setUniformFloat("metallic", (float)row / (float)nrRows);
				for (int col = 0; col < nrColumns; ++col)
				{
					utPBR.setUniformFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(float)(col - (nrColumns / 2)) * spacing,
						(float)(row - (nrRows / 2)) * spacing,
						-8.0f
					));
					utPBR.setUniformMat4("projection", proj);
					utPBR.setUniformMat4("model", model);
					utBall.render();
				}
			}


			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{

				utPBR.setUniformVec3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
				utPBR.setUniformVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.5f));
				utPBR.setUniformMat4("model", model);
				utBall.render(); 
			}
		}
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skybox.setActiveShader();
		skybox.setUniformMat4("view", view);
		skybox.setUniformMat4("projection", proj);
		cube.bindTex();
		cube.render();

		//
		camera.onUpdate();
		SDL_Event event = { 0 };
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		while (SDL_PollEvent(&event))
		{
			if (state[SDL_SCANCODE_ESCAPE] || (event.type == SDL_QUIT))
			{
				quit = true;
			}
		}
		if (state[SDL_SCANCODE_L])
		{
			if (cursorLock == true)
			{
				cursorLock = false;
			}
		}
		if (state[SDL_SCANCODE_K])
		{
			if (cursorLock == false)
			{
				cursorLock = true;
			}
		}
		if (cursorLock == false)
		{
			SDL_ShowCursor(1);
			SDL_SetWindowGrab(window, SDL_FALSE);
		}
		if (cursorLock == true)
		{
			SDL_ShowCursor(0);
			SDL_SetWindowGrab(window, SDL_TRUE);
			SDL_WarpMouseInWindow(window, (windowWidth / 2), (windowHeight / 2));
		}
		SDL_GL_SwapWindow(window);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
