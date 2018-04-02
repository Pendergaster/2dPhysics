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


#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

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
#define SCREENWIDHT 1200
#define SCREENHEIGHT 800

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
inline vec2 point_to_world_pos(vec2 orgPos,vec2 camPos)
{
	orgPos.y = SCREENHEIGHT - orgPos.y;
	vec2 temp = { SCREENWIDHT / 2.f , SCREENHEIGHT / 2.f };
	neg_vec2(&orgPos, &orgPos, &temp);
	add_vec2(&orgPos, &orgPos, &camPos);
	return orgPos;
}

#include "source\debugrend.c"
#include "source\2dphysics.c"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024
static void error_callback(int e, const char *d)
{
	printf("Error %d: %s\n", e, d);
}


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
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);

	init_keys();


	struct nk_context *ctx;
	struct nk_colorf bg;

	ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);

	{struct nk_font_atlas *atlas;
	nk_glfw3_font_stash_begin(&atlas);
	nk_glfw3_font_stash_end(); }


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



	const double dt = 1.0 / 60.0;

	double currentTime = glfwGetTime();
	double accumulator = 0.0;
	vec2 campos = { 0 };
	float cameramovementrate = 5.f;

	DebugRend drend = { 0 };
	init_debugrend(&drend);

	PhysicsContext world = { 0 };
	world.gravity.y = 0.f;
	vec2 pos1 = { 0.f,0.f };
	vec2 dimConst = { 50.f , 50.f };
	init_physicsContext(&world,pos1,dimConst);
	Object* objects[2] = { 0 };
	
	objects[0] = get_new_body(&world);
	objects[1] = get_new_body(&world);
	vec2 pos2 = { 0.f,300.f };
	objects[0]->pos = pos1;
	objects[0]->dim = dimConst;
	objects[0]->rotation = deg_to_rad(10.f);
	objects[1]->pos = pos2;
	objects[1]->dim = dimConst;
	objects[1]->mass = 10.f;
	objects[0]->mass = 10.f;


	objects[1]->rotation = deg_to_rad(0.f);
	objects[1]->velocity.x = -0;
	objects[1]->velocity.y = 0;
	objects[1]->rotVelocity = deg_to_rad(0); 
	objects[0]->rotVelocity = deg_to_rad(0);
	objects[0]->velocity.x = 0;
	objects[0]->velocity.y = 0;
	objects[1]->velocity.y = -70;

	objects[1]->momentumOfInteria = (1.f / 12.f) * objects[1]->mass *((dimConst.x * 2) * (dimConst.x * 2) + (dimConst.y * 2) * (dimConst.y * 2));
	objects[0]->momentumOfInteria = objects[1]->momentumOfInteria;


	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		nk_glfw3_new_frame();


		if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
			NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
			NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE))
		{
			enum { EASY, HARD };
			static int op = EASY;
			static int property = 20;
			nk_layout_row_static(ctx, 30, 80, 1);
			if (nk_button_label(ctx, "button"))
				fprintf(stdout, "button pressed\n");

			nk_layout_row_dynamic(ctx, 30, 2);
			if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
			if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

			nk_layout_row_dynamic(ctx, 25, 1);
			nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

			nk_layout_row_dynamic(ctx, 20, 1);
			nk_label(ctx, "background:", NK_TEXT_LEFT);
			nk_layout_row_dynamic(ctx, 25, 1);
			if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx), 400))) {
				nk_layout_row_dynamic(ctx, 120, 1);
				bg = nk_color_picker(ctx, bg, NK_RGBA);
				nk_layout_row_dynamic(ctx, 25, 1);
				bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
				bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
				bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
				bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
				nk_combo_end(ctx);
			}
		}
		nk_end(ctx);

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
		
		while (accumulator >= dt)//processloop
		{
			ro += 10.f * dt;
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
			//objects[1]->pos = point_to_world_pos(in.mousepos, camera.pos);
			

			//printf("%f --- %f \n", objects[1]->pos.x, objects[1]->pos.y);
			vec2 forceTEMP = { -0.f, 10.f };
			
			vec2 finalforce = forceTEMP;

			/*finalforce.x = cosf(objects[0]->rotation) * forceTEMP.x + (-sinf(objects[0]->rotation) * forceTEMP.y);
			finalforce.y = sinf(objects[0]->rotation) * forceTEMP.x + (cosf(objects[0]->rotation) * forceTEMP.y);*/

			//force_to_body(objects[0], -dimConst.x, dimConst.y, finalforce,&drend);
			update_bodies(&world, (float)dt, objects, 2, &drend);
	/*		draw_box(&drend, objects[0]->pos, objects[0]->dim, ro);
			draw_box(&drend, objects[1]->pos, objects[0]->dim, ro);*/
			vec2 dim2 = { 50,50 };
			//draw_box(&drend, dim, dim2, 0);
			update_keys();
			populate_debugRend_buffers(&drend);
		}

		push_to_renderer(&rend, &objects[0]->pos, &objects[0]->dim, &uv, objects[0]->rotation, box.UserId);
		push_to_renderer(&rend, &objects[1]->pos, &objects[1]->dim, &uv, objects[1]->rotation , box.UserId);
		create_render_buffers(&rend);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glCheckError();

		update_camera(&camera);
		render(&shader, &rend, &camera.camera, &campos);

		GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
		glActiveTexture(GL_TEXTURE0);
		GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
		GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
		GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
		GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
		GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
		GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
		GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
		GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
		GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
		GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
		GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
		GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
		GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
		GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

		nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

		glUseProgram(last_program);
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindSampler(0, last_sampler);
		glActiveTexture(last_active_texture);
		glBindVertexArray(last_vertex_array);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
		glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
		glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
		if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
		if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
		if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
		if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
		glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);


	/*	vec3 translate = { .x = (-campos.x) + ((float)SCREENWIDHT) / 2.f,.y = (-campos.y) + ((float)SCREENHEIGHT / 2.f),.z = 0.f };
		mat4 bind = { 0 };*/
		//translate_mat4(&bind, &camera.camera, translate);

		render_debug_lines(&drend, &camera.camera);
		glfwSwapBuffers(window);
	}
	nk_glfw3_shutdown();

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