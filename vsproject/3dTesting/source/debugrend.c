
CREATEDYNAMICARRAY(vec2, vert_buffer)
CREATEDYNAMICARRAY(int,index_buffer)


const char* VERT_SRC = "#version 130 \n \
						//The vertex shader operates on each vertex \n \
						//input data from the VBO. Each vertex is 2 floats \n \
						in vec2 vertexPosition; \n \
						out vec2 fragmentPosition; \n \
						uniform mat4 P; \n \
						void main() { \n \
							//Set the x,y position on the screen \n \
							gl_Position.xy = (P * vec4(vertexPosition, 0.0, 1.0)).xy; \n \
							//the z position is zero since we are in 2D \n \
							gl_Position.z = 0.0; \n \
							//Indicate that the coordinates are normalized \n \
							gl_Position.w = 1.0; \n \
							fragmentPosition = vertexPosition;\n \
						}";
const char* FRAG_SRC = "#version 130 \n \
	//The fragment shader operates on each pixel in a given polygon \n \
	in vec2 fragmentPosition; \n \
	//This is the 3 component float vector that gets outputted to the screen \n \
	//for each pixel. \n \
	out vec4 color; \n \
	void main() { \n \
		color = vec4(1,1,1,1); \n \
	}";


typedef struct
{
	uint				vao;
	uint				vbo;
	uint				ibo;
	int					numIndicies;
	ShaderHandle		shader;
	struct vert_buffer	verts;
	struct index_buffer	indexes;
} DebugRend;


void init_debugrend(DebugRend* rend)
{
	uint vert = compile_shader(GL_VERTEX_SHADER,VERT_SRC);
	assert(vert);

	uint frag = compile_shader(GL_FRAGMENT_SHADER, FRAG_SRC);
	assert(frag);

	rend->shader.progId = glCreateProgram();

	add_attribute(&rend->shader, "vertexPosition");
	glAttachShader(rend->shader.progId, vert);
	glAttachShader(rend->shader.progId, frag);
	link_shader(&rend->shader,vert,frag);

	glGenVertexArrays(1, &rend->vao);
	glGenBuffers(1, &rend->vbo);
	glGenBuffers(1, &rend->ibo);

	assert(rend->vao > 0 || rend->vbo > 0 || rend->ibo > 0);

	glBindVertexArray(rend->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rend->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend->ibo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
	glBindVertexArray(0);

	INITARRAY(rend->verts);
	INITARRAY(rend->indexes);

	rend->numIndicies = 0;
}

void draw_line(DebugRend* rend, const vec2 pos1, const vec2 pos2)
{
	vec2* vertArray = NULL;
	int ind = rend->verts.num;
	GET_NEW_BLOCK(rend->verts, vertArray, 2);
	vertArray[0] = pos1;
	vertArray[1] = pos2;
	PUSH_NEW_OBJ(rend->indexes, ind);
	PUSH_NEW_OBJ(rend->indexes, ind + 1);
}

inline vec2 rotatePoint(const vec2 point, float angle)
{
	vec2 newV = { 0 };
	newV.x = point.x * cosf(angle) - point.y *sinf(angle);
	newV.y = point.x * sinf(angle) + point.y *cosf(angle);
	return newV;
}

void draw_box(DebugRend* rend,const vec2 pos,const vec2 dim,float angle)
{
	int ind = rend->verts.num;
	vec2* vertArray = NULL;
	GET_NEW_BLOCK(rend->verts, vertArray, 4);
	vec2 p1 = { .x = pos.x - dim.x ,.y = pos.y - dim.y };
	vec2 p2 = { .x = pos.x - dim.x ,.y = pos.y + dim.y };
	vec2 p3 = { .x = pos.x + dim.x ,.y = pos.y + dim.y };
	vec2 p4 = { .x = pos.x + dim.x ,.y = pos.y - dim.y };

	if(angle)
	{
		vec2 d1 = { .x = -dim.x ,.y = -dim.y };
		vec2 d2 = { .x = -dim.x ,.y = dim.y };
		vec2 d3 = { .x =  dim.x ,.y =  dim.y };
		vec2 d4 = { .x =  dim.x ,.y = -dim.y };

		p1 = rotatePoint(d1, angle);
		p2 = rotatePoint(d2, angle);
		p3 = rotatePoint(d3, angle);
		p4 = rotatePoint(d4, angle);
		
		add_vec2(&p1, &p1, &pos);
		add_vec2(&p2, &p2, &pos);
		add_vec2(&p3, &p3, &pos);
		add_vec2(&p4, &p4, &pos);
		
	}
	vertArray[0] = p1;
	vertArray[1] = p2;
	vertArray[2] = p3;
	vertArray[3] = p4;

	PUSH_NEW_OBJ(rend->indexes, ind++);
	PUSH_NEW_OBJ(rend->indexes, ind);
	PUSH_NEW_OBJ(rend->indexes, ind++);
	PUSH_NEW_OBJ(rend->indexes, ind);
	PUSH_NEW_OBJ(rend->indexes, ind++);
	PUSH_NEW_OBJ(rend->indexes, ind);
	PUSH_NEW_OBJ(rend->indexes, ind);
	PUSH_NEW_OBJ(rend->indexes, ind - 3);
}


void populate_debugRend_buffers(DebugRend* rend)
{
	glBindBuffer(GL_ARRAY_BUFFER, rend->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * rend->verts.num, NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * rend->verts.num, rend->verts.buff);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rend->ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, rend->indexes.num * sizeof(int), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, rend->indexes.num * sizeof(int), rend->indexes.buff); 
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	rend->numIndicies = rend->indexes.num;
	rend->indexes.num = 0;
	rend->verts.num = 0;
}
void dispose_debug_renderer(DebugRend* rend)
{
	if (rend->vao)
	{
		glDeleteVertexArrays(1, &(rend->vao));
	}
	if (rend->vbo)
	{
		glDeleteBuffers(1, &(rend->vao));
	}
	if (rend->ibo)
	{
		glDeleteBuffers(1, &(rend->ibo));
	}
	dispose_shader(&(rend->shader));
	DISPOSE_ARRAY(rend->indexes);
	DISPOSE_ARRAY(rend->verts);
}
void render_debug_lines(DebugRend* rend, mat4* mat)
{
	if (rend->numIndicies == 0) return;
	use_shader(&rend->shader);
	set_mat4(&rend->shader, "P", mat->mat);
	glLineWidth(0.5);
	glBindVertexArray(rend->vao);
	glDrawElements(GL_LINES, rend->numIndicies, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	unuse_shader(&rend->shader);
}





