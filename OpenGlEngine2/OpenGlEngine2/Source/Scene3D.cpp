#include "Scene3D.h"

#include "FlyCamera.h"

bool Scene3D::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_gui = new GUIBar(m_window);

	m_rows = 257;
	m_cols = 257;

	m_timer = 0;

	m_tree = new FBXFile();
	m_tree->load("./Res/Models/tree/TreeLowPoly.fbx", FBXFile::UNITS_METER, true, false, true);
	m_tree->initialiseOpenGLTextures();

	m_pyro = new FBXFile();
	m_pyro->load("./Res/characters/Pyro/pyro.fbx", FBXFile::UNITS_METER, true, true, true);
	m_pyro->initialiseOpenGLTextures();

	//m_dragon = new FBXFile();
	//m_dragon->load("./Res/Models/stanford/Dragon.fbx");

	CreateOpenGLBuffers(m_tree);
	CreateOpenGLBuffersAnimated(m_pyro);
	GenerateOpenGLBuffersTerrain();

	CreateShadowFrameBuffer();

	GenerateGrid(m_rows, m_cols);
	//GeneratePerlinValues();
	Regenerate();

	GenerateAllShaders();

	return true;
}
void Scene3D::ShutDown()
{

}

//----generalfunctions------
bool Scene3D::Update(double dt)
{
	//-----ANimated Object Updates
	m_timer += dt;

	skeleton = m_pyro->getSkeletonByIndex(0);
	skeleton->updateBones();

	animation = m_pyro->getAnimationByIndex(0);
	skeleton->evaluate(animation, m_timer);

	for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; bone_index++)
	{
		skeleton->m_nodes[bone_index]->updateGlobalTransform();
	}


	//shadowMapping
	m_lightDirection = glm::normalize(glm::vec3(m_gui->m_lightDir.x*-1, m_gui->m_lightDir.y*-1, m_gui->m_lightDir.z*-1));

	glm::mat4 lightProjection = glm::ortho<float>(-10, 10, -10, 10, -10, 10);
	glm::mat4 lightView = glm::lookAt(m_lightDirection, glm::vec3(0), glm::vec3(0, 1, 0));

	m_lightMatrix = lightProjection * lightView;


	//----Camera nad Regenration key-------
	m_camera->Update(dt);

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		//GeneratePerlinValues();
		Regenerate();
	}
	return true;
}
void Scene3D::Render()
{
	RenderShadowGeometry();
	RenderObject(m_object_program, m_tree);
	RenderAnimatedObject(m_animated_program, m_pyro);
	RenderTerrain();

	unsigned int indicesSize = (m_rows - 1) * (m_cols - 1) * 6;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_N) == GLFW_PRESS)
	{
		for (unsigned int i = 0; i < 257 * 257; i++)
		{
			Gizmos::addLine(aoVertices[i].position.xyz, (aoVertices[i].normal.xyz + aoVertices[i].position.xyz), glm::vec4(1, 0, 0, 1));
		}
	}


	m_gui->Draw();
	glClearColor(m_gui->m_clearColour.r, m_gui->m_clearColour.g, m_gui->m_clearColour.b, 1);
}

unsigned int Scene3D::loadShader(unsigned int type, const char* path)
{
	FILE* file = fopen(path, "rb");
	if (file == nullptr)
		return 0;

	// read the shader source
	fseek(file, 0, SEEK_END);

	unsigned int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char* source = new char[length + 1];
	memset(source, 0, length + 1);
	fread(source, sizeof(char), length, file);
	fclose(file);

	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, 0);
	glCompileShader(shader);

	delete[] source;

	return shader;
}
void Scene3D::GenerateAllShaders()
{
	GenerateShadowMapShaders();
	//GenerateShaders(m_shadowGenProgram, "./Res/Shaders/shadowMap.vert", "./Res/Shaders/shadowMap.frag");
	GenerateTerrainShaders();
	//GenerateShaders(m_useTerrainProgram, "./Res/Shaders/terrain.vert", "./Res/Shaders/terrain.frag");
	GenerateOjectShaders();
	//GenerateShaders(m_object_program, "./Res/Shaders/ObjectShader.vert", "./Res/Shaders/ObjectShader.frag");
	GenerateAnimatedObjectShaders();
}

//-----shadowMapFunctions-----
void Scene3D::GenerateShadowMapShaders()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Res/Shaders/shadowMap.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Res/Shaders/shadowMap.frag");

	int success = GL_FALSE;
	m_shadowGenProgram = glCreateProgram();

	glAttachShader(m_shadowGenProgram, vs);
	glAttachShader(m_shadowGenProgram, fs);
	glLinkProgram(m_shadowGenProgram);
	glGetProgramiv(m_shadowGenProgram, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_shadowGenProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_shadowGenProgram, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}
void Scene3D::CreateShadowFrameBuffer()
{
	// setup shadow map buffer
	glGenFramebuffers(1, &m_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);

	glGenTextures(1, &m_shadowFBODepth);
	glBindTexture(GL_TEXTURE_2D, m_shadowFBODepth);

	// texture uses a 16-bit depth component format
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// attached as a depth attachment to capture depth not colour
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_shadowFBODepth, 0);

	// no colour targets are used
	glDrawBuffer(GL_NONE);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("Framebuffer Error!\n");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Scene3D::RenderShadowGeometry()
{
	// shadow pass: bind our shadow map target and clear the depth
	glBindFramebuffer(GL_FRAMEBUFFER, m_shadowFBO);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(m_shadowGenProgram);

	// bind the light matrix
	int loc = glGetUniformLocation(m_shadowGenProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_lightMatrix[0][0]));

	//draw all shadow-casting geometry-----------------------------\

	//-----------tree
	for (unsigned int i = 0; i < m_tree->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_tree->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;
		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
	//-----pyro
	for (unsigned int i = 0; i < m_pyro->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = m_pyro->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}

	////-----terrain
	glBindVertexArray(m_terrainVao);
	m_indexCount = (m_rows - 1) * (m_cols - 1) * 6;
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	// final pass: bind back-buffer and clear colour and depth
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1920, 1080);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//next use shaders to render objects
}

//-----Terrain Generation-----
void Scene3D::GenerateOpenGLBuffersTerrain()
{
	data = stbi_load("./Res/textures/terrain/grass2.jpg", &imageWidth, &imageHeight, &imageFormat, STBI_default);	glGenTextures(1, &m_grass_texture);
	glBindTexture(GL_TEXTURE_2D, m_grass_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./Res/textures/terrain/water.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_water_texture);
	glBindTexture(GL_TEXTURE_2D, m_water_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./Res/textures/terrain/pebbles.png", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &m_rocks_texture);
	glBindTexture(GL_TEXTURE_2D, m_rocks_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	//Also doing textures for my objects in here
	data = stbi_load("./Res/Models/tree/Tree_normals.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &object_texture_normal);
	glBindTexture(GL_TEXTURE_2D, object_texture_normal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	stbi_image_free(data);

	data = stbi_load("./Res/textures/tree/Tree_Texture.tga", &imageWidth, &imageHeight, &imageFormat, STBI_default);

	glGenTextures(1, &object_texture_diffuse);
	glBindTexture(GL_TEXTURE_2D, object_texture_diffuse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

}
void Scene3D::CleanUpOpenGlBuffersTerrain()
{

}
void Scene3D::GenerateGrid(unsigned int rows, unsigned int cols)
{
	aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r)
	{
		for (unsigned int c = 0; c < cols; ++c)
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);
			aoVertices[r * cols + c].texCoord = glm::vec2((float)c / cols, (float)r / rows);

			glm::vec3 colour = glm::vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = glm::vec4(colour, 1);

		}
	}

	// defining index count based off quad count (2 triangles per quad)
	auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
	unsigned int index = 0;
	for (unsigned int r = 0; r < (rows - 1); ++r)
	{
		for (unsigned int c = 0; c < (cols - 1); ++c)
		{
			// triangle 1
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			// triangle 2
			auiIndices[index++] = r * cols + c;
			auiIndices[index++] = (r + 1) * cols + (c + 1);
			auiIndices[index++] = r * cols + (c + 1);
		}
	}

	// create and bind buffers to a vertex array object
	glGenBuffers(1, &m_terrainVbo);
	glGenBuffers(1, &m_terrainIbo);

	glGenVertexArrays(1, &m_terrainVao);	glBindVertexArray(m_terrainVao);	glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4) * 2));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_terrainIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void Scene3D::GenerateTerrainShaders()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Res/Shaders/terrain.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Res/Shaders/terrain.frag");

	int success = GL_FALSE;
	m_useTerrainProgram = glCreateProgram();

	glAttachShader(m_useTerrainProgram, vs);
	glAttachShader(m_useTerrainProgram, fs);
	glLinkProgram(m_useTerrainProgram);
	glGetProgramiv(m_useTerrainProgram, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_useTerrainProgram, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_useTerrainProgram, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}
void Scene3D::GeneratePerlinValues()
{
	int dims = 257;
	perlin_data = new float[dims * dims];
	float scale = (1.0f / dims) * m_gui->m_scaleMultiplier;
	int octaves = m_gui->m_octaves;

	for (int x = 0; x < dims; x++)
	{
		for (int y = 0; y < dims; y++)
		{
			float amplitude = m_gui->m_amplitude;
			float persistence = m_gui->m_persistence;
			perlin_data[y* dims + x] = 0;

			for (int o = 0; o < octaves; o++)
			{
				float freq = powf(2, (float)o);
				float perlin_sample = glm::perlin(glm::vec2((float)x, (float)y) * scale * freq) * 0.5f + 0.5f;

				perlin_data[y * dims + x] += perlin_sample * amplitude;
				amplitude *= persistence;
			}
		}
	}

	glGenTextures(1, &m_perlin_texture);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dims, dims, 0, GL_RED, GL_FLOAT, perlin_data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
void Scene3D::RenderTerrain()
{
	//render terrain
	glUseProgram(m_useTerrainProgram);

	//bind camera
	int loc = glGetUniformLocation(m_useTerrainProgram, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(m_camera->GetProjectionView()[0][0]));

	//bind shadowmap values
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	lightMatrix = textureSpaceOffset * m_lightMatrix;

	//bind terrain textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);
	loc = glGetUniformLocation(m_useTerrainProgram, "perlin_texture");
	glUniform1i(loc, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_grass_texture);
	loc = glGetUniformLocation(m_useTerrainProgram, "m_grass_texture");
	glUniform1i(loc, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_water_texture);
	loc = glGetUniformLocation(m_useTerrainProgram, "m_water_texture");
	glUniform1i(loc, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_rocks_texture);
	loc = glGetUniformLocation(m_useTerrainProgram, "m_rocks_texture");
	glUniform1i(loc, 3);


	//shadow and lighting for terrain
	loc = glGetUniformLocation(m_useTerrainProgram, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(m_useTerrainProgram, "lightDir");
	glUniform3fv(loc, 1, &m_lightDirection[0]);

	loc = glGetUniformLocation(m_useTerrainProgram, "shadowMap");
	glUniform1i(loc, 4);

	loc = glGetUniformLocation(m_useTerrainProgram, "shadowBias");
	glUniform1f(loc, 0.01f);

	//bind shadow map texture
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_shadowFBODepth);

	int lightDirection = glGetUniformLocation(m_useTerrainProgram, "LightDir");
	int lightColour = glGetUniformLocation(m_useTerrainProgram, "LightColour");
	int SpecPow = glGetUniformLocation(m_useTerrainProgram, "SpecPow");
	int cameraPosition = glGetUniformLocation(m_useTerrainProgram, "cameraPos");

	glUniform3f(lightDirection, m_gui->m_lightDir.x, m_gui->m_lightDir.y, m_gui->m_lightDir.z);
	glUniform3f(lightColour, m_gui->m_lightColour.x, m_gui->m_lightColour.y, m_gui->m_lightColour.z);
	glUniform1f(SpecPow, m_gui->m_specular);
	glUniformMatrix4fv(cameraPosition, 1, GL_FALSE, (float*)&m_camera->GetProjectionView());

	glBindVertexArray(m_terrainVao);
	m_indexCount = (m_rows - 1) * (m_cols - 1) * 6;
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
}

void Scene3D::GenerateNormal(Vertex* vert1, Vertex* vert2, Vertex* vert3)
{
	//calculate face normal
	glm::vec3 d1(vert3->position - vert1->position);
	glm::vec3 d2(vert2->position - vert1->position);

	glm::vec3 crossProduct = glm::cross(d1, d2);

	glm::vec3 normal = glm::normalize(crossProduct);

	vert1->normal = normal;
	vert2->normal = normal;
	vert3->normal = normal;
}
void Scene3D::Regenerate()
{
	GeneratePerlinValues();
	unsigned int perlinSize = m_rows*m_cols;

	//loop through perlin_data
	for (unsigned int i = 0; i < perlinSize; i++)
	{
		aoVertices[i].position.y = perlin_data[i] * 50;
		aoVertices[i].position.y -= 35;
		aoVertices[i].position.x -= 124;
		aoVertices[i].position.z -= 124;
	}

	//generate new normals
	unsigned int indicesSize = (m_rows - 1) * (m_cols - 1) * 6;
	//loop through auIndices
	for (unsigned int i = 0; i < indicesSize; i += 3)
	{
		Vertex* vertex1 = &aoVertices[auiIndices[i+2]];
		Vertex* vertex2 = &aoVertices[auiIndices[i+1]];
		Vertex* vertex3 = &aoVertices[auiIndices[i]];

		GenerateNormal(vertex1, vertex2, vertex3);
	}

	//Gizmos::addSphere(aoVertices[1].position.xyz, 10, 10, 10, glm::vec4(1, 0, 0, 1));

	// Update GPU data
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_rows * m_cols * sizeof(Vertex), aoVertices);

	// Clean up
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainVbo);
	glBindTexture(GL_TEXTURE_2D, 0);

}

//-----ObjectLoading---------
void Scene3D::CreateOpenGLBuffers(FBXFile* fbx)
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

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals
		glEnableVertexAttribArray(2); //tangents
		glEnableVertexAttribArray(3); //texcoords

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}
void Scene3D::CleanUpOpenGlBuffers(FBXFile* fbx)
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
void Scene3D::GenerateOjectShaders()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Res/Shaders/ObjectShader.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Res/Shaders/ObjectShader.frag");

	int success = GL_FALSE;
	m_object_program = glCreateProgram();

	glAttachShader(m_object_program, vs);
	glAttachShader(m_object_program, fs);
	glLinkProgram(m_object_program);
	glGetProgramiv(m_object_program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_object_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_object_program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}
void Scene3D::RenderObject(unsigned int program, FBXFile* fbx)
{
	////object loading-------------------------------------
	glUseProgram(program);

	//bind camera
	int loc = glGetUniformLocation(program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE,
		&(m_camera->GetProjectionView()[0][0]));

	//bind shadowmap values
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	lightMatrix = textureSpaceOffset * m_lightMatrix;

	//shadow and lighting for object1
	loc = glGetUniformLocation(program, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(program, "lightDir");
	glUniform3fv(loc, 1, &m_lightDirection[0]);

	loc = glGetUniformLocation(program, "shadowBias");
	glUniform1f(loc, 0.01f);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_shadowFBODepth);
	loc = glGetUniformLocation(program, "shadowMap");
	glUniform1i(loc, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, object_texture_normal);
	loc = glGetUniformLocation(program, "normal");
	glUniform1i(loc, 5);

	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, object_texture_diffuse);
	loc = glGetUniformLocation(program, "diffuse");
	glUniform1i(loc, 6);

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i) {
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);

		int m_global = glGetUniformLocation(program, "global");
		glUniformMatrix4fv(m_global, 1, GL_FALSE, &(mesh->m_globalTransform)[0][0]);

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES,
			(unsigned int)mesh->m_indices.size(),
			GL_UNSIGNED_INT, 0);
	}
}
//------Animated Object Loading-------------------------------------
void Scene3D::CreateOpenGLBuffersAnimated(FBXFile* fbx)
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

		glEnableVertexAttribArray(0); //position
		glEnableVertexAttribArray(1); //normals
		glEnableVertexAttribArray(2); //tangents
		glEnableVertexAttribArray(3); //texcoords
		glEnableVertexAttribArray(4); //weights
		glEnableVertexAttribArray(5); //indices

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::NormalOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), (void*)FBXVertex::TangentOffset);
		glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::WeightsOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (void*)FBXVertex::IndicesOffset);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}
void Scene3D::RenderAnimatedObject(unsigned int program, FBXFile* fbx)
{
	// use our texture program
	glUseProgram(program);

	// bind the camera
	int loc = glGetUniformLocation(program, "ProjectionView");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &(m_camera->GetProjectionView()[0][0]));

	//bind shadowmap values
	glm::mat4 textureSpaceOffset(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
		);

	lightMatrix = textureSpaceOffset * m_lightMatrix;

	FBXSkeleton* skeleton = fbx->getSkeletonByIndex(0);
	skeleton->updateBones();

	int bones_location = glGetUniformLocation(program, "bones");
	glUniformMatrix4fv(bones_location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

	//// set texture slots
	//glActiveTexture(GL_TEXTURE7);
	//GLuint h = fbx->getTextureByIndex(3)->handle;
	//glBindTexture(GL_TEXTURE_2D, h);
	//glActiveTexture(GL_TEXTURE8);
	//glBindTexture(GL_TEXTURE_2D, fbx->getTextureByIndex(1)->handle);
	//glActiveTexture(GL_TEXTURE9);
	//glBindTexture(GL_TEXTURE_2D, fbx->getTextureByIndex(2)->handle);

	////// tell the shader where it is
	loc = glGetUniformLocation(program, "diffuse");
	glUniform1i(loc, 7);
	loc = glGetUniformLocation(program, "normal");
	glUniform1i(loc, 8);
	loc = glGetUniformLocation(program, "specular");
	glUniform1i(loc, 9);

	//shadow and lighting for object1
	loc = glGetUniformLocation(program, "LightMatrix");
	glUniformMatrix4fv(loc, 1, GL_FALSE, &lightMatrix[0][0]);

	loc = glGetUniformLocation(program, "LightDir");
	glUniform3fv(loc, 1, &m_lightDirection[0]);

	loc = glGetUniformLocation(program, "shadowBias");
	glUniform1f(loc, 0.01f);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_shadowFBODepth);
	loc = glGetUniformLocation(program, "shadowMap");
	glUniform1i(loc, 4);

	// bind our vertex array object and draw the mesh
	for (unsigned int i = 0; i < fbx->getMeshCount(); ++i)
	{
		FBXMeshNode* mesh = fbx->getMeshByIndex(i);
		unsigned int* glData = (unsigned int*)mesh->m_userData;

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::DiffuseTexture]->handle);
		glActiveTexture(GL_TEXTURE8);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::NormalTexture]->handle);
		glActiveTexture(GL_TEXTURE9);
		glBindTexture(GL_TEXTURE_2D, mesh->m_material->textures[FBXMaterial::SpecularTexture]->handle);

		int m_global = glGetUniformLocation(program, "global");
		glUniformMatrix4fv(m_global, 1, GL_FALSE, &(mesh->m_globalTransform)[0][0]);

		glBindVertexArray(glData[0]);
		glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
	}
}
void Scene3D::GenerateAnimatedObjectShaders()
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, "./Res/Shaders/animatedObject.vert");
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, "./Res/Shaders/animatedObject.frag");

	int success = GL_FALSE;
	m_animated_program = glCreateProgram();

	glAttachShader(m_animated_program, vs);
	glAttachShader(m_animated_program, fs);
	glLinkProgram(m_animated_program);
	glGetProgramiv(m_animated_program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_animated_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_animated_program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Scene3D::GenerateShaders(unsigned int program, const char* fragment, const char* vertex)
{
	unsigned int vs = loadShader(GL_VERTEX_SHADER, vertex);
	unsigned int fs = loadShader(GL_FRAGMENT_SHADER, fragment);

	int success = GL_FALSE;
	program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	// remove unneeded handles
	glDeleteShader(vs);
	glDeleteShader(fs);
}