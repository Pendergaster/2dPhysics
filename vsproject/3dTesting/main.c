#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <glad\glad.h>
#include <glfw3.h>
#include <assert.h>
#define MATH_IMPLEMENTATION
#include<mathutil.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
typedef struct
{
	int a;
} myree;
#define SCREENWIDHT 800
#define SCREENHEIGHT 600

#define FATALERROR assert(0);
#define FATALERRORMESSAGE(STRING) printf(STRING); assert(0);
typedef struct 
{
	ubyte*	keys;
	ubyte*	lastkeys;
	vec2	mousepos;
	vec2	lastMousepos;

} InputManager;



void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

}

static InputManager in;
void set_key(int key,ubyte state)
{
	in.keys[key] = state;
}
void set_mouse( float x, float y)
{
	in.mousepos.x = x;
	in.mousepos.y = y;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS || action == GLFW_RELEASE)
	set_key( key, action == GLFW_PRESS ? 1 : 0);
}
static ubyte mouse_init = 0;
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	set_mouse((float)xpos, (float)ypos);
	if(!mouse_init)
	{
		mouse_init = 1;
		in.lastMousepos = in.mousepos;
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
		}
		else if (action == GLFW_RELEASE)
		{
		}
	}
}
int key_pressed(int key)
{
	return in.keys[key] == 1 && in.lastkeys[key] == 0;
}
int key_down(int key)
{
	return in.keys[key] == 1;
}
int key_released(int key)
{
	return in.keys[key] == 0 && in.lastkeys[key] == 1;
}




void init_keys()
{
	int i = GLFW_KEY_LAST;
	in.keys = calloc(GLFW_KEY_LAST,sizeof(ubyte));
	in.lastkeys = calloc(GLFW_KEY_LAST, sizeof(ubyte));
	in.mousepos.x = (float)SCREENWIDHT / 2.f;
	in.mousepos.y = (float)SCREENHEIGHT / 2.f;
	in.lastMousepos.x = in.mousepos.x;
	in.lastMousepos.y = in.mousepos.y;
}
void dipose_inputs()
{
	free(in.keys);
	free(in.lastkeys);

}
void update_keys()
{
	memcpy(in.lastkeys, in.keys, sizeof(ubyte)*GLFW_KEY_LAST);
	if (!mouse_init) return;
	in.lastMousepos = in.mousepos;
}

#include "source/shaderutils.c"

#define SHADER_FILES(FILE)\
		FILE(vert) \
		FILE(frag) \

#define TXT_FILES(FILE) \
		FILE(nonefile)  \
		

#define PNG_FILES(FILE) \
		FILE(linux_pingu)\


#define JPG_FILES(FILE) \
		FILE(laatikko)	\


#define GENERATE_ENUM(ENUM) ENUM,

#define GENERATE_STRING(STRING) #STRING".txt",

#define GENERATE_STRINGPNG(STRING) #STRING".png",

#define GENERATE_STRINGJPG(STRING) #STRING".jpg",

#define GENERATE_SHADER_STRING(STRING) "shaders/"#STRING".txt",



static const char* txt_file_names[] = {
	TXT_FILES(GENERATE_STRING)
	SHADER_FILES(GENERATE_SHADER_STRING)
};

static const char* pic_file_names[] = {
	PNG_FILES(GENERATE_STRINGPNG)
	JPG_FILES(GENERATE_STRINGJPG)
};


enum txt_files
{
	TXT_FILES(GENERATE_ENUM)
	SHADER_FILES(GENERATE_ENUM)
	maxtxtfiles
};


enum picture_files {
	PNG_FILES(GENERATE_ENUM)
	JPG_FILES(GENERATE_ENUM)
	maxpicfiles
};



typedef struct
{
	uint	ID;
	uint	UserId;
	int		widht;
	int		height;
	int		channels;
} Texture;
Texture textureCache[maxpicfiles] = {0};

Texture loadTexture(const int file)
{
	if(textureCache[file].ID != 0)
	{
		return textureCache[file];
	}
	Texture* tex = &textureCache[file];
	//int* k = malloc(1000);
	//k[3] = 0;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &tex->ID);
	glBindTexture(GL_TEXTURE_2D, tex->ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	unsigned char* data = stbi_load(pic_file_names[file], &tex->widht, &tex->height, &tex->channels,0);
	if(!data)
	{
		FATALERROR;
		//FATALERRORMESSAGE("FAILED TO LOAD PICTURE %s\n", pic_file_names[file]);
	}
	if (tex->channels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->widht, tex->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else if (tex->channels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex->widht, tex->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		FATALERROR;
	}
	stbi_image_free(data);
	tex->UserId = (uint)(&textureCache[file] - textureCache);
	return *tex;
}
char* load_file_from_source(const char* file, int* size);
char* load_file(int file,int* size)
{
	char *source = NULL;
	source = load_file_from_source(txt_file_names[file], size);

	return source;
}
char* load_file_from_source(const char* file, int* size)
{
	char *source = NULL;
	FILE *fp = fopen(file, "r");
	if (fp != NULL) {
		/* Go to the end of the file. */
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			long bufsize = ftell(fp);
			if (bufsize == -1) { /* Error */ }

			/* Allocate our buffer to that size. */
			source = malloc(sizeof(char) * (bufsize + 1));

			/* Go back to the start of the file. */
			if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

			/* Read the entire file into memory. */
			size_t newLen = fread(source, sizeof(char), bufsize, fp);
			if (ferror(fp) != 0) {
				fputs("Error reading file", stderr);
				FATALERROR;
			}
			else {
				source[newLen++] = '\0'; /* Just to be safe. */
				if(size)
				{
					*size = (int)newLen;
				}
			}
		}
		fclose(fp);
	}
	else
	{
		printf("FILE NOT FOUND");
		FATALERROR;
	}
	return source;
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		unsigned char* error = NULL;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
			//case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
			//case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		//FATALERRORMESSAGE("GL ERROR %s \n", error);	
		printf("GL ERROR %s \n", error);
		FATALERROR;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

static const float vertex_buffer_data[] = {
	-0.5f,  0.5f,
	0.5f, -0.5f,
	-0.5f, -0.5f,

	-0.5f,  0.5f,
	0.5f, -0.5f,
	0.5f,  0.5f,
};
typedef struct
{
	vec4	objectData;
	float	rotation;
	int		textid;
} VertexData;

typedef struct
{
	uint		vao;
	uint		uvBuffer;
	uint		vertBuffer;
	uint		singleObjectBuffer;
	VertexData* vertdata;
	int			vertDsize;
	int			numVerts;
	vec2*		uvdata;
	int			uvSize;
	int			numUvs;
	ubyte		texturesToBind[maxpicfiles];
} Renderer;

void create_render_buffers(Renderer* rend);

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCREENWIDHT, SCREENHEIGHT, "Tabula Rasa", NULL, NULL);

	if (window == NULL)
	{
		printf("Failed to create window\n");
		_getch();
		return 0;
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to initialize GLAD\n");
		_getch();
		return 0;
	}

	glViewport(0, 0, SCREENWIDHT, SCREENHEIGHT);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	init_keys();



	Texture box = loadTexture(laatikko);


	ShaderHandle shader = { 0 };
	{
		char* vertS = load_file(vert, NULL);	
		uint vID = compile_shader(GL_VERTEX_SHADER, vertS);
		free(vertS);
		char* fragS = load_file(frag, NULL);
		uint fID = compile_shader(GL_FRAGMENT_SHADER, fragS);
		free(fragS);
		shader.progId = glCreateProgram();
		glAttachShader(shader.progId, vID);
		glAttachShader(shader.progId, fID);

		add_attribute(&shader,"uv");
		add_attribute(&shader,"vert");
		add_attribute(&shader,"wdata");
		add_attribute(&shader,"textid");
		add_attribute(&shader, "rotation");

		link_shader(&shader, vID, fID);
		use_shader(&shader);
		unuse_shader(&shader);
	}

	glCheckError();

	

	Renderer rend = { 0 };
	glGenVertexArrays(1, &rend.vao);
	glBindVertexArray(rend.vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);


	glGenBuffers(4, &rend.uvBuffer);
#define DEFAULT_BUFFER_SIZE 1000
	glBindBuffer(GL_ARRAY_BUFFER, rend.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * DEFAULT_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glVertexAttribDivisor(0, 0);//update every vertex

	glBindBuffer(GL_ARRAY_BUFFER, rend.vertBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex_buffer_data), vertex_buffer_data);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glVertexAttribDivisor(1, 0);//update every vertex

	glBindBuffer(GL_ARRAY_BUFFER, rend.singleObjectBuffer);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) + sizeof(float) + sizeof(int)) * DEFAULT_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);



	/*update these once per object*/
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, objectData));
	glVertexAttribDivisor(2, 1);//wdata


	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, rotation));
	glVertexAttribDivisor(3, 1);//rotaion

						
	glVertexAttribPointer(4, 1, GL_INT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, textid));
	glVertexAttribDivisor(4, 1);//texid

	glBindVertexArray(0);
	glCheckError();

#undef DEFAULT_BUFFER_SIZE
	rend.vertdata = malloc(sizeof(VertexData) * 20);
	rend.uvdata = malloc(sizeof(vec2) * 20);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		if (key_pressed(GLFW_KEY_ESCAPE))
		{
			break;
		}
		if (key_down(GLFW_KEY_W))
		{
			
		}
		if (key_down(GLFW_KEY_A))
		{
		
		}
		if (key_down(GLFW_KEY_D))
		{
	
		}
		if (key_down(GLFW_KEY_S))
		{
		
		}

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCheckError();

	
		update_keys();
		glfwSwapBuffers(window);
	}
	dipose_inputs();
	glfwTerminate();
	return 1;
}

void push_to_renderer(Renderer* rend,vec2* pos,vec2* dimensions,vec2* uv,float rotation,int txtid)
{
	if(rend->numVerts + 1 >= rend->vertDsize)
	{
		rend->vertDsize *= 2;
		VertexData* temp = rend->vertdata;
		rend->vertdata = realloc(rend->vertdata, sizeof(VertexData) * rend->vertDsize);
		if(!rend->vertdata)
		{
			rend->vertdata = temp;
			return;
		}
	}
	if (rend->numUvs + 1 >= rend->uvSize)
	{
		rend->uvSize *= 2;
		vec2* temp= rend->uvdata;
		rend->uvdata = realloc(rend->uvdata, sizeof(vec2) * rend->uvSize);
		if (!rend->uvdata)
		{
			rend->uvdata = temp;
			return;
		}
	}
	VertexData* data = &rend->vertdata[rend->numVerts++];
	vec2* uvd = &rend->uvdata[rend->numUvs++];

	vec4 objdata = { pos->x,pos->y,dimensions->x,dimensions->y };
	data->objectData = objdata;

	rend->texturesToBind[txtid] = 1;
}
void create_render_buffers(Renderer* rend)
{
	glBindBuffer(GL_ARRAY_BUFFER, rend->singleObjectBuffer);
	glBufferData(GL_ARRAY_BUFFER, rend->vertDsize* sizeof(VertexData), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, rend->vertDsize * sizeof(VertexData), rend->vertdata);

	glBindBuffer(GL_ARRAY_BUFFER, rend->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, rend->uvSize * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, rend->uvSize * sizeof(vec2), rend->uvdata);

	rend->numUvs = 0;
}
void render(ShaderHandle* handle,Renderer* in,uint tex)
{
	//bind textures


	for(int i = 0; i < maxpicfiles; i++)
	{
		if(in->texturesToBind[i] == 1)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureCache[i].ID);

			in->texturesToBind[i] = 0;
		}
	}

	use_shader(handle);

	glBindVertexArray(in->vao);

	//set_matrix(&in->instaShader, "P", camMatrix);
	GLint textureUniform = get_uniform_location(handle, "mySampler");
	glUniform1i(textureUniform, 0);
	glBindTexture(GL_TEXTURE_2D, tex);
	glBindVertexArray(in->vao);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, in->numVerts);
	in->numVerts = 0;
	glBindVertexArray(0);
	unuse_shader(handle);
}

//todo set matrix