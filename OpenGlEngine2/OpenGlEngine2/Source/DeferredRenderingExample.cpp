#include "DeferredRenderingExample.h"

#include "FlyCamera.h"

bool DeferredRendering::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_fbx = new FBXFile();
	m_fbx->load("./Res/Models/stanford/Bunny.fbx");

	CreateOpenGLBuffers(m_fbx);
	CreateShaders();
	CreateGBufferShaders();
	CreateCompositeShader();
	CreateLightShader();

	m_lightDirection = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f));

	return true;
}

void DeferredRendering::ShutDown()
{

}

bool DeferredRendering::Update(double dt)
{
	m_camera->Update(dt);

	return true;
}

void DeferredRendering::Render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1920, 1080);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_programID);

	// bind the camera
	int loc = glGetUniformLocation(m_programID, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(m_camera->GetProjectionView()[0][0]));

	loc = glGetUniformLocation(m_programID, "lightDir");
	glUniform3fv(loc, 1, &m_lightDirection[0]);

	// G-Pass: render out the albedo, position and normal
	glEnable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_gpassShader);

	loc = glGetUniformLocation(m_gpassShader, "View");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetView()[0][0]));

	// draw our scene, in this example just the Stanford Bunny
	for (unsigned int i = 0; i < m_fbx->getMeshCount(); ++i) 
	{
		FBXMeshNode* mesh = m_fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}

	// Light Pass: render lights as geometry, sampling position and
	// normals disable depth testing and enable additive blending
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);
	glClear(GL_COLOR_BUFFER_BIT);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	glUseProgram(m_directionLightShader);

	loc = glGetUniformLocation(m_directionLightShader, "positionTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);

	loc = glGetUniformLocation(m_directionLightShader, "normalTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);

	// draw lights as fullscreen quads
	DrawDirectionalLight(glm::vec3(-1), glm::vec3(1));
	glDisable(GL_BLEND);


	// Composite Pass: render a quad and combine albedo and light
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_compositeShader);

	loc = glGetUniformLocation(m_compositeShader, "albedoTexture");
	glUniform1i(loc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);

	loc = glGetUniformLocation(m_compositeShader, "lightTexture");
	glUniform1i(loc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);

	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DeferredRendering::CreateOpenGLBuffers(FBXFile* fbx)
{
	// create the GL VAO/VBO/IBO data for each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		// storage for the opengl data in 3 unsigned int
		unsigned int* glData = new unsigned int[3];

		glGenVertexArrays(1, &glData[0]);
		glBindVertexArray(glData[0]);

		glGenBuffers(1, &glData[1]);
		glGenBuffers(1, &glData[2]);

		glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void DeferredRendering::CleanUpOpenGLBuffers(FBXFile* fbx)
{
	// clean up the vertex data attached to each mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);

		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData[0]);
		glDeleteBuffers(1, &glData[1]);
		glDeleteBuffers(1, &glData[2]);

		delete[] glData;
	}
}

void DeferredRendering::CreateShaders()
{
	const char* vsSource = "#version 410\n \
							in vec4 Position;\
							in vec4 Normal;\
							\
							out vec4 vNormal;\
							\
							uniform mat4 ProjectionView;\							void main() \
							{\
								vNormal = Normal;\
								gl_Position = ProjectionView * Position;\
							}";
	
	const char* fsSource = "#version 410\n \
							in vec4 vNormal;\
							\
							out vec4 FragColour;\
							uniform vec3 lightDir;\
							\
							void main()	\
							{\
								float d = max(0, dot(normalize(vNormal.xyz), lightDir));\
								\								FragColour = vec4(d, d, d, 1);\
							}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();

	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void DeferredRendering::CreateDeferredBuffers()
{
	// setup gpass framebuffer
	glGenFramebuffers(1, &m_gpassFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gpassFBO);

	glGenTextures(1, &m_albedoTexture);
	glBindTexture(GL_TEXTURE_2D, m_albedoTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &m_positionTexture);
	glBindTexture(GL_TEXTURE_2D, m_positionTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenTextures(1, &m_normalTexture);
	glBindTexture(GL_TEXTURE_2D, m_normalTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB32F, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &m_gpassDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_gpassDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1280, 720);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_albedoTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, m_positionTexture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, m_normalTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_gpassDepth);

	GLenum gpassTargets[] = { GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, gpassTargets);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// setup light framebuffer
	glGenFramebuffers(1, &m_lightFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_lightFBO);

	glGenTextures(1, &m_lightTexture);
	glBindTexture(GL_TEXTURE_2D, m_lightTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, 1280, 720);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_lightTexture, 0);

	GLenum lightTargets[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, lightTargets);

	status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredRendering::CreateGBufferShaders()
{
	const char* vsSource = "#version 410\n \
						    layout(location = 0) in vec4 Position;\
							layout(location = 1) in vec4 Normal;\
							\
							out vec4 vPosition;\
							out vec4 vNormal;\
							\
							uniform mat4 View;\
							uniform mat4 ProjectionView;\
							\
							void main()\
							 {\
								// first store view-space position and normal\
								vPosition = View * Position;\
								vNormal = normalize(View * Normal);\
								gl_Position = ProjectionView * Position;\
							}";

	const char* fsSource = "#version 410\n \
							in vec4 vPosition;\
							in vec4 vNormal;\
							\
							layout(location = 0) out vec3 gpassAlbedo;\
							layout(location = 1) out vec3 gpassPosition;\
							layout(location = 2) out vec3 gpassNormal;\
							\
							void main() \
							{\
								gpassAlbedo = vec3(1);\
								gpassPosition = vPosition.xyz;\
								gpassNormal = vNormal.xyz;\
							}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_gpassShader = glCreateProgram();

	glAttachShader(m_gpassShader, vertexShader);
	glAttachShader(m_gpassShader, fragmentShader);
	glLinkProgram(m_gpassShader);
	glGetProgramiv(m_gpassShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_gpassShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_gpassShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void DeferredRendering::CreateCompositeShader()
{
	const char* vsSource = "#version 410\n \
						   	layout(location = 0) in vec4 Position;\
							layout(location = 1) in vec2 TexCoord;\
							out vec2 vTexCoord;\
							void main() \
							{\
								vTexCoord = TexCoord;\
								gl_Position = Position;\
							}";

	const char* fsSource = "#version 410\n \
							in vec2 vTexCoord;\
							out vec4 FragColour;\
							uniform sampler2D albedoTexture;\
							uniform sampler2D lightTexture;\
							void main() \
							{\
								vec3 light = texture(lightTexture, vTexCoord).rgb;\
								vec3 albedo = texture(albedoTexture, vTexCoord).rgb;\
								FragColour = vec4(albedo * light, 1);\
							}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_compositeShader = glCreateProgram();

	glAttachShader(m_compositeShader, vertexShader);
	glAttachShader(m_compositeShader, fragmentShader);
	glLinkProgram(m_compositeShader);
	glGetProgramiv(m_compositeShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_compositeShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_compositeShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void DeferredRendering::CreateLightShader()
{
	const char* vsSource = "#version 410\n \
							in vec4 position;\
							in vec2 texCoord;\
							out vec2 fTexCoord;\
							void main() {\
								gl_Position = position;\
								fTexCoord = texCoord;\
							}";

	const char* fsSource = "#version 410\n \
							in vec2 vTexCoord;\
							out vec3 LightOutput;\
							// direction in view-space\
							uniform vec3 lightDirection;\
							uniform vec3 lightDiffuse;\
							uniform sampler2D positionTexture;\
							uniform sampler2D normalTexture;\
							void main() \
							{\
								vec3 normal = normalize(texture(normalTexture, vTexCoord).xyz);\
								vec3 position = texture(positionTexture, vTexCoord).xyz;\
								float d = max(0, dot(normal, -lightDirection));\
								LightOutput = lightDiffuse * d;\
							}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_directionLightShader = glCreateProgram();

	glAttachShader(m_directionLightShader, vertexShader);
	glAttachShader(m_directionLightShader, fragmentShader);
	glLinkProgram(m_directionLightShader);
	glGetProgramiv(m_directionLightShader, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_directionLightShader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_directionLightShader, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void DeferredRendering::DrawDirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse) 
{
	glm::vec4 viewSpaceLight = m_camera->GetView() *

	glm::vec4(glm::normalize(direction), 0);
	int loc = glGetUniformLocation(m_directionLightShader,"lightDirection");
	glUniform3fv(loc, 1, &viewSpaceLight[0]);

	loc = glGetUniformLocation(m_directionLightShader,"lightDiffuse");
	glUniform3fv(loc, 1, &diffuse[0]);
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
