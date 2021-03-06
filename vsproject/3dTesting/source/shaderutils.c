typedef struct
{

	uint	progId;
	int		numAttribs;
} ShaderHandle;

uint compile_shader(uint glenum, const char* source)
{
	uint compilecheck = 0;
	uint shader = glCreateShader(glenum);
	if (shader == 0) FATALERROR;

	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &compilecheck);

	if (!compilecheck)
	{
		uint infolen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infolen);
		if (infolen > 1)
		{
			char* infoLog = malloc(sizeof(char) * infolen);
			glGetShaderInfoLog(shader, infolen, NULL, infoLog);
			printf("Error compiling shader :\n%s\n", infoLog);
			free(infoLog);
		}
		glDeleteShader(shader);
		FATALERROR;
	}
	return shader;
}
void dispose_shader(ShaderHandle* sha)
{
	if (sha->progId)glDeleteProgram(sha->progId);
}
void unuse_shader(const ShaderHandle* sha)
{
	glUseProgram(0);
	for (int i = 0; i < sha->numAttribs; i++) {
		glDisableVertexAttribArray(i);
	}
}
void add_attribute(ShaderHandle* shader, const char* name)
{
	printf("Adding attribute %s to program %d \n", name, shader->progId);
	glBindAttribLocation(shader->progId, shader->numAttribs++, name);
}
void set_uniform_int(const ShaderHandle* shader, const char* name, int value)
{
	glUniform1i(glGetUniformLocation(shader->progId, name), value);
}
void set_uniform_float(const ShaderHandle* shader, const char* name, float value)
{
	glUniform1f(glGetUniformLocation(shader->progId, name), value);
}
void set_mat4(const ShaderHandle* sha, const char* name, float mat[4][4])
{
	int vertexLocation = glGetUniformLocation(sha->progId, name);
	if (vertexLocation == GL_INVALID_INDEX)
	{
		FATALERROR;
	}
	glUniformMatrix4fv(vertexLocation, 1, GL_FALSE, (GLfloat*)mat);
}
void set_vec3(const ShaderHandle* sha, const char* name, const vec3* vec)
{
	int vertexLocation = glGetUniformLocation(sha->progId, name);
	if (vertexLocation == GL_INVALID_INDEX)
	{
		FATALERROR;
	}
	glUniform3f(vertexLocation, vec->x, vec->y, vec->z);
}


uint get_uniform_location(ShaderHandle* shader, const char* name)
{
	GLint location = glGetUniformLocation(shader->progId, name);
	if (location == GL_INVALID_INDEX)
	{
		printf("failed to get uniform");
		FATALERROR;
	}
	return location;
}



void use_shader(const ShaderHandle* shader)
{
	glUseProgram(shader->progId);
	for (int i = 0; i < shader->numAttribs; i++) {
		glEnableVertexAttribArray(i);
	}
}

void link_shader(ShaderHandle* shader, uint vert, uint frag)
{
	printf("Linking program %d\n", shader->progId);
	//shader->progId = glCreateProgram();
	glLinkProgram(shader->progId);
	uint linked = 0;
	glGetProgramiv(shader->progId, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		uint infolen = 0;
		glGetProgramiv(shader->progId, GL_INFO_LOG_LENGTH, &infolen);
		if (infolen > 1)
		{
			char* infolog = malloc(sizeof(char)*infolen);
			glGetProgramInfoLog(shader->progId, infolen, NULL, infolog);
			printf("Error linking program \n %s", infolog);
			free(infolog);
		}
		glDeleteProgram(shader->progId);
		FATALERROR;
	}

	glDeleteShader(vert);
	glDeleteShader(frag);
}
