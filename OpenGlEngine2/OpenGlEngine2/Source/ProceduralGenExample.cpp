#include "ProceduralGenExample.h"
#include <iostream>

bool ProceduralGeneration::StartUp()
{
	FlyCamera* pCamera = new FlyCamera();
	SetUpCamera(pCamera);

	m_gui = new GUIBar(m_window);

	RAND_MAX == 32;

	m_rows = 257;
	m_cols = 257;
	
	DisplayGrid(257);
	GenerateGrid(m_rows, m_cols);
	GenerateOpenGLBuffers();
	CreateShaders();
	GeneratePerlinValues();

	return true;
}

void ProceduralGeneration::ShutDown()
{
	
}

bool ProceduralGeneration::Update(double dt)
{
	m_camera->Update(dt);

	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{

		GeneratePerlinValues();
	}

	return true;
}

void ProceduralGeneration::Render()
{
	glUseProgram(m_program);

	unsigned int projectionViewUniform = glGetUniformLocation(m_program, "ProjectionView");
	glUniformMatrix4fv(projectionViewUniform, 1, false, &(m_camera->GetProjectionView()[0][0]));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_perlin_texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_grass_texture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_water_texture);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_rocks_texture);

	int loc = glGetUniformLocation(m_program, "perlin_texture");
	glUniform1i(loc, 0);

	loc = glGetUniformLocation(m_program, "m_grass_texture");
	glUniform1i(loc, 1);

	loc = glGetUniformLocation(m_program, "m_water_texture");
	glUniform1i(loc, 2);

	loc = glGetUniformLocation(m_program, "m_rocks_texture");
	glUniform1i(loc, 3);

	glBindVertexArray(m_vao);

	m_indexCount = (m_rows - 1) * (m_cols - 1) * 6;
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);

	m_gui->Draw();
}

float ProceduralGeneration::FindHeight(unsigned int row, unsigned int column)
{
	return aoVertices[row * 257 + column].texCoord.y;
}

void ProceduralGeneration::GenerateGrid(unsigned int rows, unsigned int cols) 
{

	aoVertices = new Vertex[rows * cols];
	for (unsigned int r = 0; r < rows; ++r) 
	{
		for (unsigned int c = 0; c < cols; ++c) 
		{
			aoVertices[r * cols + c].position = glm::vec4((float)c, 0, (float)r, 1);
			aoVertices[r * cols + c].texCoord = glm::vec2((float)c / cols, (float)r / rows);
			// create some arbitrary colour based off something
			// that might not be related to tiling a texture
			glm::vec3 colour = glm::vec3(sinf((c / (float)(cols - 1)) * (r / (float)(rows - 1))));
			aoVertices[r * cols + c].colour = glm::vec4(colour, 1);

		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[(rows - 1) * (cols - 1) * 6];
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
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);	glBindVertexArray(m_vao);	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, (rows * cols) * sizeof(Vertex), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)*2));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (rows - 1) * (cols - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	delete[] aoVertices;
	delete[] auiIndices;

}

void ProceduralGeneration::GenerateOpenGLBuffers()
{	data = stbi_load("./Res/textures/terrain/grass2.jpg", &imageWidth, &imageHeight, &imageFormat, STBI_default);	glGenTextures(1, &m_grass_texture);
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
}

void ProceduralGeneration::CreateShaders()
{
	// create shaders
	/*----------------------------------DIAMOND TEXTURE SHADER--------------------------------------------------------
	const char* vsSource = "#version 410\n \
   						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 texCoord;\
						   layout(location=2) in vec4 Colour; \
						   \
						   out vec2 frag_texcoord;\
						   out vec4 vColour; \
						   \
						   uniform mat4 ProjectionView; \
						   uniform sampler2D diamond_texture;\
						   \
						   void main()\
						   {\
								vec4 pos = Position;\
								pos.y += texture(diamond_texture, texCoord).r *5;\
								frag_texcoord = texCoord;\
								gl_Position = ProjectionView * pos;\
						    }";

	const char* fsSource = "#version 410\n \
						   	in vec2 frag_texcoord;\
							in vec4 vColour; \
							out vec4 out_color;\
							uniform sampler2D diamond_texture;\
							void main()\
							{\
								out_color = texture(diamond_texture, frag_texcoord).rrrr;\
								out_color.a = 1;\
							}";
	------------------------------------------------------------------------------------------------*/


	//-----------------------------------PERLIN TEXTURE SHADER-------------------------------------
		const char* vsSource = "#version 410\n \
   						   layout(location=0) in vec4 Position; \
						   layout(location=1) in vec2 texcoord;\
						   layout(location=2) in vec4 Colour; \
						   \
						   out vec2 frag_texcoord;\
						   out vec4 vColour; \
						   \
						   uniform mat4 ProjectionView; \
						   uniform sampler2D perlin_texture;\
						   uniform sampler2D m_grass_texture;\
						   uniform sampler2D m_water_texture;\
						   uniform sampler2D m_rocks_texture;\
						   \
						   void main()\
						   {\
								vec4 pos = Position;\
								pos.y += texture(perlin_texture, texcoord).r * 50;\
								frag_texcoord = texcoord;\
								gl_Position = ProjectionView * pos;\
						    }";

	const char* fsSource = "#version 410\n \
						   	in vec2 frag_texcoord;\
							in vec4 vColour; \
							out vec4 out_color;\
							uniform sampler2D perlin_texture;\
							uniform sampler2D m_grass_texture;\
							uniform sampler2D m_water_texture;\
							uniform sampler2D m_rocks_texture;\
							void main()\
							{\
								float height = texture(perlin_texture, frag_texcoord).r;\
								out_color = texture(perlin_texture, frag_texcoord).rrrr;\
								out_color.a = 1;\
								if(height <= 0.45)\
								{\
									out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_water_texture, frag_texcoord*2);\
								}\
								else if(height >= 0.45 && height <= 0.455)\
								{\
									out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_rocks_texture, frag_texcoord*2);\
								}\
								else\
								{\
									out_color = texture(perlin_texture, frag_texcoord).rrrr*texture(m_grass_texture, frag_texcoord*2);\
								}\
							}";

	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);
	
	m_program = glCreateProgram();
	glAttachShader(m_program, vertexShader);
	glAttachShader(m_program, fragmentShader);
	glLinkProgram(m_program);
	
	glGetProgramiv(m_program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) 
	{
		int infoLogLength = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(m_program, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program!\n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}
	
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void ProceduralGeneration::GeneratePerlinValues()
{
	int dims = 257;
	float *perlin_data = new float[dims * dims];
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

void ProceduralGeneration::DiamondSquare(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, int range)
{
	//if (level < 1)
	//	return;

	//// diamonds
	//for (unsigned i = x1 + level; i < x2; i += level)
	//{
	//	for (unsigned j = y1 + level; j < y2; j += level)
	//	{
	//		float a = gridArray[i - level][j - level];
	//		float b = gridArray[i][j - level];
	//		float c = gridArray[i - level][j];
	//		float d = gridArray[i][j];
	//		float e = gridArray[i - level / 2][j - level / 2] = (a + b + c + d) / 4 + rand() * range;
	//	}
	//}

	//// squares
	//for (unsigned i = x1 + 2 * level; i < x2; i += level)
	//{
	//	for (unsigned j = y1 + 2 * level; j < y2; j += level)
	//	{
	//		float a = gridArray[i - level][j - level];
	//		float b = gridArray[i][j - level];
	//		float c = gridArray[i - level][j];
	//		float d = gridArray[i][j];
	//		float e = gridArray[i - level / 2][j - level / 2];

	//		float f = gridArray[i - level][j - level / 2] = (a + c + e + gridArray[i - 3 * level / 2][j - level / 2]) / 4 + rand() * range;
	//		float g = gridArray[i - level / 2][j - level] = (a + b + e + gridArray[i - level / 2][j - 3 * level / 2]) / 4 + rand() * range;
	//	}
	//}

	//DiamondSquare(x1, y1, x2, y2, range / 2, level / 2);
}

void ProceduralGeneration::GenerateDiamondSquareNoise(float rows, float cols)
{
	DiamondSquare(0, 0, rows, cols, 32);

	glGenTextures(1, &m_diamond_texture);
	glBindTexture(GL_TEXTURE_2D, m_diamond_texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, rows, cols, 0, GL_RED, GL_FLOAT, gridArray);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}