



typedef struct
{
	vec2	pos;
	vec2	dim;
	vec2	velocity;
} Object;

typedef struct
{
	int	sizeOfBuffer;
	int	numObjs;
	Object** buffer;
} Objectbuffer;

#define DEFOBJBUFFS 30

inline void init_buffer(Objectbuffer* b)
{
	b->buffer = malloc(sizeof(Objectbuffer*) * DEFOBJBUFFS);
	b->sizeOfBuffer = DEFOBJBUFFS;
	b->numObjs = 0;
}

inline void clear_buffer(Objectbuffer* b)
{
	b->numObjs = 0;
}

inline void dispose_buffer(Objectbuffer* b)
{
	free(b->buffer);
	b->numObjs = 0;
	b->sizeOfBuffer = 0;
}
struct ree
{
	int a;
	struct ree* k;
};

struct tree
{
	unsigned int				level;
	Objectbuffer				objects;
	struct tree*				treebuffer;
	vec2						pos;
	vec2						dim;
};



void create_new_node(struct tree* node, uint level, const vec2* pos, const vec2* dim)
{
	node->level = level;
	node->pos = *pos;
	node->level = level;
	node->dim = *dim;
	if (node->objects.buffer == NULL)
	{
		init_buffer(&node->objects);
	}
}
void clear_tree(struct tree* node)
{
	if (node->treebuffer)
	{
		for (uint32_t i = 0; i < 4; i++)
		{
			clear_tree(&node->treebuffer[i]);
		}
	}
	clear_buffer(&node->objects);
	node->treebuffer = NULL;
}

inline Object** get_new_item_from_buff(Objectbuffer* b)
{
	if(b->numObjs + 1 >= b->sizeOfBuffer)
	{
		b->sizeOfBuffer *= 2;
		Object** temp = b->buffer;
		b->buffer = realloc(b->buffer,sizeof(Object*) * b->sizeOfBuffer);
		if(!b->buffer)
		{
			b->buffer = temp;
			b->sizeOfBuffer /= 2;
			return &b->buffer[b->numObjs];
		}
	}
	return &b->buffer[b->numObjs++];
}

inline void fast_rem_from_buf(Objectbuffer* b,const uint index)
{
	assert(b->numObjs > 0);
	Object* temp = b->buffer[b->numObjs - 1];
	b->buffer[index] = temp;
	b->numObjs--;
}

void split_tree(struct tree* node,struct tree* allocator, uint32_t* allocatorstart)
{
	float subWidht = node->dim.x / 2;
	float subHeight = node->dim.y / 2;

	node->treebuffer = &allocator[*allocatorstart];
	*allocatorstart += 4;
	vec2 pos = { node->pos.x + subWidht,node->pos.y + subHeight };
	vec2 dim = { subWidht,subHeight };
	create_new_node(&node->treebuffer[0], node->level + 1,&pos,&dim);
	pos.x = node->pos.x - subWidht;
	pos.y = node->pos.y + subHeight;
	create_new_node(&node->treebuffer[1], node->level + 1,&pos,&dim);
	pos.x = node->pos.x - subWidht;
	pos.y = node->pos.y - subHeight;
	create_new_node(&node->treebuffer[2], node->level + 1,&pos,&dim);
	pos.x = node->pos.x + subWidht;
	pos.y = node->pos.y - subHeight;
	create_new_node(&node->treebuffer[3], node->level + 1, &pos, &dim);
}

inline int get_index(struct tree* node, vec2* pos, vec2* dim)
{
	int index = -1;

	//if true it can fit to bottom place
	uint bot = pos->y - dim->y  > node->pos.y - node->dim.y
		&& pos->y + dim->y <  node->pos.y;
	//if true it can fit to top place
	uint top = pos->y - dim->y  > node->pos.y
		&& pos->y + dim->y <  node->pos.y + node->dim.y;


	if (pos->x - dim->x  > node->pos.x - node->dim.x
		&& pos->x + dim->x <  node->pos.x)
	{
		if (top)
		{
			index = 1;
		}
		else if (bot)
		{
			index = 2;
		}
	}
	else if (pos->x - dim->x  > node->pos.x
		&& pos->x + dim->x <  node->pos.x + node->dim.x)
	{
		if (top)
		{
			index = 0;
		}
		else if (bot)
		{
			index = 3;
		}
	}
	return index;
}
#define MAX_OBJECTAMOUNT 20
#define MAX_TREELEVEL	 4

void insert_to_tree(struct tree* node, Object* obj,struct tree* allocator, uint32_t* allocatorsize)
{
	if (node->treebuffer)
	{
		int index = get_index(node, &obj->pos, &obj->dim);
		if (index != -1)
		{
			insert_to_tree(&node->treebuffer[index], obj, allocator, allocatorsize);
			return;
		}
	}
	Object** newObj = get_new_item_from_buff(&node->objects);//node->objects.get_new_item();
	(*newObj) = obj;

	if (node->objects.numObjs >= MAX_OBJECTAMOUNT && node->level < MAX_TREELEVEL)
	{
		if (node->treebuffer == NULL)
		{
			int s = node - allocator;
			split_tree(node, allocator, allocatorsize);
		}
		uint i = 0;
		while (i < node->objects.numObjs)
		{
			int index = get_index(node, &node->objects.buffer[i]->pos, &node->objects.buffer[i]->dim);
			if (index != -1)
			{
				insert_to_tree(&node->treebuffer[index], node->objects.buffer[i], allocator, allocatorsize);
				node->objects.buffer[i] = NULL;
				fast_rem_from_buf(&node->objects, i);
			}
			else
			{
				i++;
			}
		}
	}

}
void get_collisions(struct tree* node, Objectbuffer* buffer, Object* obj)
{
	int index = get_index(node, &obj->pos, &obj->dim);
	if (index != -1 && node->treebuffer != NULL)
	{
		get_collisions(&node->treebuffer[index], buffer, obj);
	}
	for (uint32_t i = 0; i < node->objects.numObjs; i++)
	{
		Object** newobj = get_new_item_from_buff(buffer);// buffer->get_new_item();
		*newobj = node->objects.buffer[i];
	}
}

#undef	MAX_OBJECTAMOUNT 
#undef	MAX_TREELEVEL 
#undef	DEFOBJBUFFS 