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

static int MEMTRACK = 0;

inline void* DEBUG_MALLOC(int size)
{
	MEMTRACK++;
	return malloc(size);
}

inline void* DEBUG_CALLOC(int COUNT,int SIZE)
{
	MEMTRACK++;
	return calloc(COUNT,SIZE);
}

#define MEM_DEBUG
#ifdef  MEM_DEBUG
#define free(PTR) do{ free(PTR); MEMTRACK--;}while(0)
#define malloc(SIZE) DEBUG_MALLOC(SIZE)
#define calloc(COUNT,SIZE) DEBUG_CALLOC(COUNT,SIZE)
#endif //  MEM_DEBUG


#include "source\smallGenericDynArray.h"
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
void push_to_renderer(Renderer* rend, vec2* pos, vec2* dimensions, vec4* uv, float rotation, int txtid);
void render(ShaderHandle* handle, Renderer* in, mat4* camMat, vec2* campos);


typedef struct
{
	mat4	ortho;
	mat4	camera;
	vec2	pos;
} Camera;

void init_camera(Camera* cam)
{
	orthomat_2d(&cam->ortho, 0, SCREENWIDHT, 0, SCREENHEIGHT);
}
void update_camera(Camera* cam)
{
	vec3 translate = { .x = (-cam->pos.x) + ((float)SCREENWIDHT) / 2.f,.y = (-cam->pos.y) + ((float)SCREENHEIGHT / 2.f),.z = 0.f };
	translate_mat4(&cam->camera, &cam->ortho, translate);
}


#include "source\debugrend.c"
#include "source\2dphysics.c"
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

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


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
	glVertexAttribDivisor(1, 0);

	glBindBuffer(GL_ARRAY_BUFFER, rend.singleObjectBuffer);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(vec4) + sizeof(float) + sizeof(float)) * DEFAULT_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);



	/*update these once per object*/
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, objectData));
	glVertexAttribDivisor(2, 1);//wdata


	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, rotation));
	glVertexAttribDivisor(3, 1);//rotation

						
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, textid));
	glVertexAttribDivisor(4, 1);//texid

	glBindVertexArray(0);
	glCheckError();

#undef DEFAULT_BUFFER_SIZE
	rend.vertdata = malloc(sizeof(VertexData) * 20);
	rend.uvdata = malloc(sizeof(vec2) * 20);
	rend.uvSize = 20;
	rend.vertDsize = 20;

	Camera camera = { 0 };
	init_camera(&camera);
	//mat4 camOr =
	//{
	//	1/SCREENWIDHT, 0.f, 0.f, 0.f,
	//	0.f, 1 / SCREENHEIGHT, 0.f, 0.f,
	//	0.f, 0.f, 0.f, 0.f,
	//	0.f, 0.f, 0.f, 1.f,
	//};
	
	//vec4 res = { 0 };
	//vec4 poir = { 400.f,300.f,0.f,1.f };
	//mat4_mult_vec4(&res, &camOr, &poir);


	const double dt = 1.0 / 60.0;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;
	vec2 campos = { 0 };
	float cameramovementrate = 5.f;

	DebugRend drend = { 0 };
	init_debugrend(&drend);

	PhysicsContext world = { 0 };
	vec2 pos1 = { 0.f,0.f };
	vec2 dimConst = { 100.f , 100.f };
	init_physicsContext(&world,pos1,dimConst);
	Object* objects[2] = { 0 };
	
	objects[0] = get_new_body(&world);
	objects[1] = get_new_body(&world);
	vec2 pos2 = { 150.f,150.f };
	objects[0]->pos = pos1;
	objects[0]->dim = dimConst;
	objects[1]->pos = pos2;
	objects[1]->dim = dimConst;

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		double newTime = glfwGetTime();

		double frameTime = newTime - currentTime;
		currentTime = newTime;
		accumulator += frameTime;
		if (key_down(GLFW_KEY_ESCAPE))
		{
			break;
		}
		vec2 pos = { 00.f ,00.f };
		vec2 dim = { 100.f ,100.f};
		vec4 uv = { 0.f, 0.f,1.f,1.f };
		static float ro = 0.f;
		ro += 0.01f;
		while (accumulator >= dt)//processloop
		{
			accumulator -= dt;
			if (key_down(GLFW_KEY_W))
			{
				camera.pos.y += cameramovementrate;
			}
			if (key_down(GLFW_KEY_A))
			{
				camera.pos.x -= cameramovementrate;
			}
			if (key_down(GLFW_KEY_D))
			{
				camera.pos.x += cameramovementrate;
			}
			if (key_down(GLFW_KEY_S))
			{
				camera.pos.y -= cameramovementrate;
			}
			update_bodies(&world, (float)dt, objects, 2, &drend);
	/*		draw_box(&drend, objects[0]->pos, objects[0]->dim, ro);
			draw_box(&drend, objects[1]->pos, objects[0]->dim, ro);*/
			vec2 dim2 = { 50,50 };
			//draw_box(&drend, dim, dim2, 0);
			update_keys();
			populate_debugRend_buffers(&drend);
		}


		push_to_renderer(&rend, &objects[0]->pos, &objects[0]->dim, &uv, 0, box.UserId);
		push_to_renderer(&rend, &objects[1]->pos, &objects[1]->dim, &uv, 0, box.UserId);
		create_render_buffers(&rend);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCheckError();

		update_camera(&camera);

		render(&shader, &rend,&camera.camera,&campos);


	/*	vec3 translate = { .x = (-campos.x) + ((float)SCREENWIDHT) / 2.f,.y = (-campos.y) + ((float)SCREENHEIGHT / 2.f),.z = 0.f };
		mat4 bind = { 0 };*/
		//translate_mat4(&bind, &camera.camera, translate);

		render_debug_lines(&drend, &camera.camera);
		glfwSwapBuffers(window);
	}
	free(rend.uvdata);
	free(rend.vertdata);
	dispose_debug_renderer(&drend);
	dipose_inputs();
	dispose_physicsContext(&world);
	glfwTerminate();

	if(MEMTRACK != 0)
	{
		printf("MEMORY NOT BALANCED %d\n",MEMTRACK);
		FATALERROR;
	}
	return 1;
}

void push_to_renderer(Renderer* rend,vec2* pos,vec2* dimensions,vec4* uv,float rotation,int txtid)
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
	if (rend->numUvs + 6 >= rend->uvSize)
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
	vec2* uvd = &rend->uvdata[rend->numUvs];
	rend->numUvs += 6;

	vec4 objdata = { (int)pos->x,(int)pos->y,(int)dimensions->x,(int)dimensions->y };
	data->objectData = objdata;
	data->rotation = rotation;
	data->textid = txtid;

	uvd->x = uv->x;
	uvd->y = uv->w;
	uvd++;

	uvd->x = uv->z;
	uvd->y = uv->y;
	uvd++;	

	uvd->x = uv->x;
	uvd->y = uv->y;
	uvd++;

	uvd->x = uv->x;
	uvd->y = uv->w;
	uvd++;

	uvd->x = uv->z;
	uvd->y = uv->y;
	uvd++;

	uvd->x = uv->z;
	uvd->y = uv->w;	

	rend->texturesToBind[txtid] = 1;
}
void create_render_buffers(Renderer* rend)
{
	glBindBuffer(GL_ARRAY_BUFFER, rend->singleObjectBuffer);
	glBufferData(GL_ARRAY_BUFFER, rend->numVerts * sizeof(VertexData), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, rend->numVerts * sizeof(VertexData), rend->vertdata);

	glBindBuffer(GL_ARRAY_BUFFER, rend->uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, rend->numUvs * sizeof(vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, rend->numUvs * sizeof(vec2), rend->uvdata);

	rend->numUvs = 0;
	glCheckError();

}
void render(ShaderHandle* handle, Renderer* in,mat4* camMat,vec2* campos)
{
	//bind textures
	for (int i = 0; i < maxpicfiles; i++)
	{
		if (in->texturesToBind[i] == 1)
		{
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textureCache[i].ID);
			in->texturesToBind[i] = 0;
		}
	}
	use_shader(handle);
	glBindVertexArray(in->vao);

	static const uint sample_Array[30] = { 0 ,1 ,2, 3, 4, 5, 6, 7, 8, 9 ,10 ,11 ,12 ,13, 14 , 15 ,16 ,17,18,19,20 ,21,22,23,24,25,26,27,28,29  };

	GLint textureUniform = get_uniform_location(handle, "mySamples");
	glUniform1iv(textureUniform, 30, sample_Array);

	set_mat4(handle, "P", camMat->mat);
	glCheckError();

	glBindVertexArray(in->vao);

	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, in->numVerts);
	in->numVerts = 0;
	glCheckError();

	glBindVertexArray(0);
	unuse_shader(handle);
}
//todo set matrix