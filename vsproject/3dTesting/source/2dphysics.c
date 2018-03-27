
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
			int s = (int)(node - allocator);
			split_tree(node, allocator, allocatorsize);
		}
		int i = 0;
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
	for (int i = 0; i < node->objects.numObjs; i++)
	{
		Object** newobj = get_new_item_from_buff(buffer);// buffer->get_new_item();
		*newobj = node->objects.buffer[i];
	}
}


typedef struct
{
	int		ind;
	Object*	ptrToBlock;
} BodyBlock;


enum type
{
	error = 0,
	single = 1,
	block = 2
};
typedef struct
{
	int type;
	union
	{
		BodyBlock	block;
		Object*		single;
	};

} BodyAbstractor;

CREATEDYNAMICARRAY(Object*,bodypools)
CREATEDYNAMICARRAY(BodyAbstractor,freelist)

typedef struct
{
	struct bodypools	bodies;
	struct freelist		freelist;
	int					currentindex;
} BodyAllocator;

#define DEFAULT_POOL_SIZE 100

inline void init_body_allocator(BodyAllocator* all)
{
	INITARRAY(all->bodies);
	Object* curArr = malloc(sizeof(Object) * DEFAULT_POOL_SIZE);
	PUSH_NEW_OBJ(all->bodies, curArr);
	INITARRAY(all->freelist);

	all->currentindex = 0;
}

inline void dispose_bodyAllocator(BodyAllocator* all)
{
	for(int i = 0; i < all->bodies.num; i++)
	{
		free(all->bodies.buff[i]);
	}
	DISPOSE_ARRAY(all->bodies);
	DISPOSE_ARRAY(all->freelist);
}

typedef struct
{
	BodyAllocator	bAllo;
	struct tree*	treeAllocator;
	int				treeIndex;
} PhysicsContext;

void init_physicsContext(PhysicsContext* pc,const vec2 worldPos,const vec2 worldDims)
{
	init_body_allocator(&pc->bAllo);
	pc->treeAllocator = malloc(sizeof(struct tree) * MAX_TREELEVEL);
	create_new_node(pc->treeAllocator, 0, &worldPos, &worldDims);
	pc->treeIndex = 1;
}
void dispose_physicsContext(PhysicsContext* pc)
{
	dispose_bodyAllocator(&pc->bAllo);
}
inline Object* get_new_body(PhysicsContext* pc)
{
	Object* ret = NULL;
	if (pc->bAllo.freelist.num)
	{
		if (pc->bAllo.freelist.buff[pc->bAllo.freelist.num - 1].type == single)
		{
			BodyAbstractor* temp = NULL;
			POP_ARRAY(pc->bAllo.freelist, temp);
			ret = temp->single;
		}
		else if(pc->bAllo.freelist.buff[pc->bAllo.freelist.num - 1].type == block)
		{
			
			ret = &pc->bAllo.freelist.buff[pc->bAllo.freelist.num - 1].block.ptrToBlock[pc->bAllo.freelist.buff[pc->bAllo.freelist.num - 1].block.ind++];
			if(pc->bAllo.freelist.buff[pc->bAllo.freelist.num - 1].block.ind == DEFAULT_POOL_SIZE)
			{
				pc->bAllo.freelist.num--;
			}
		}
		else
		{
			FATALERROR;
		}
	}
	else
	{
		Object* curArr = pc->bAllo.bodies.buff[pc->bAllo.bodies.num - 1];
		
		ret = &curArr[pc->bAllo.currentindex++];

		if(pc->bAllo.currentindex >= DEFAULT_POOL_SIZE)
		{ 
			Object* temp = malloc(sizeof(Object) * DEFAULT_POOL_SIZE);
			PUSH_NEW_OBJ(pc->bAllo.bodies, temp);
			pc->bAllo.currentindex = 0;
			printf("MORE MEMORY ALLOCATED FOR PHYSICS\n");
		}
	}
	return ret;
}

inline void dispose_body(PhysicsContext* pc,Object* obj)
{
	BodyAbstractor temp = { 0 };
	temp.type = single;
	temp.single = obj;
	PUSH_NEW_OBJ(pc->bAllo.freelist, temp);
}

void update_bodies(PhysicsContext* pc,float dt,Object** objects,int size,DebugRend* drend)
{
	clear_tree(pc->treeAllocator);
	pc->treeIndex = 1;
	for(int i = 0; i < size; i++)
	{
		insert_to_tree(pc->treeAllocator, objects[i], pc->treeAllocator, &pc->treeIndex);
	}
	Objectbuffer buffer = { 0 };
	init_buffer(&buffer);
	typedef struct
	{
		Object* a;
		Object* b;
	} CollisionTable;
	CREATEDYNAMICARRAY(CollisionTable, collisiondata);

	struct collisiondata colldata = { 0 };
	INITARRAY(colldata);

	for(int i = 0; i < size; i++)
	{
		get_collisions(pc->treeAllocator, &buffer, objects[i]);

		for(int  k= 0; k < buffer.numObjs; k++)
		{
			if (buffer.buffer[k] == objects[i]) continue;

			//inserted before?
			uint insert = 1;
			for(int j = 0; j < colldata.num;i++)
			{
				if(colldata.buff[j].b = buffer.buffer[k])
				{
					insert = 0;
					break;
				}
			}
			if(insert)
			{
				CollisionTable temp = { .a = buffer.buffer[k],.b = objects[i] };
				PUSH_NEW_OBJ(colldata, temp);
			}
		}
	}
	for(int i = 0; i < colldata.num;i++)
	{
		if(1)
		{
			draw_box(drend, colldata.buff[i].a->pos, colldata.buff[i].a->dim, 0);
			draw_box(drend, colldata.buff[i].b->pos, colldata.buff[i].b->dim, 0);
		}
	}
}

#undef DEFAULT_POOL_SIZE
#undef	MAX_OBJECTAMOUNT 
#undef	MAX_TREELEVEL 
#undef	DEFOBJBUFFS 