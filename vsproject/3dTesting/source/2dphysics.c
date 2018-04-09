
typedef struct
{
	vec2	pos;
	vec2	dim;
	struct{
		vec2	velocity;
		vec2	acceleration;
		vec2	forces;
		float	mass;
	};
	struct
	{
		float	rotation;
		float	rotVelocity;
		float	rotAcc;
		float	momentumOfInteria;
		float	torque;
	};
	uint Move;
} Object;

const Object DEFAULTOBJECT = { 0 };

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
	vec2			gravity;
} PhysicsContext;

void init_physicsContext(PhysicsContext* pc,const vec2 worldPos,const vec2 worldDims)
{
	init_body_allocator(&pc->bAllo);
	pc->treeAllocator = calloc((MAX_TREELEVEL *MAX_TREELEVEL * MAX_TREELEVEL *MAX_TREELEVEL )+ 1,sizeof(struct tree) );
	create_new_node(pc->treeAllocator, 0, &worldPos, &worldDims);
	pc->treeIndex = 1;
}
void dispose_physicsContext(PhysicsContext* pc)
{
	dispose_bodyAllocator(&pc->bAllo);
	
	int poolamount = 1 + 4 * 4 * 4;
	for (int i = 0; i < poolamount; i++)
	{
		if (pc->treeAllocator[i].objects.buffer)//->treeAllocator[i].objects.data)
		{
			dispose_buffer(&pc->treeAllocator[i].objects);
		}
	}
	free(pc->treeAllocator);
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
	*ret = DEFAULTOBJECT;
	return ret;
}

inline void dispose_body(PhysicsContext* pc,Object* obj)
{
	BodyAbstractor temp = { 0 };
	temp.type = single;
	temp.single = obj;
	PUSH_NEW_OBJ(pc->bAllo.freelist, temp);
}

uint AABB(vec2 point,vec2 pos,vec2 dim)
{
	float distX = abs(point.x - pos.x);
	float distY = abs(point.y - pos.y);

	return (distX <= dim.x && distY <= dim.y);

}


uint collides(Object* a,Object* b,vec2* MTV,DebugRend* drend,vec2* normal,vec2* collisionPointR)
{
	/*vec2 dist = { 0 };
	add_vec2(&dist, &a->pos, &b->pos);
	return dist.x < a->dim.x + b->dim.x || dist.y < a->dim.y + b->dim.y;*/
	float c1 = cosf(a->rotation);
	float c2 = cosf(b->rotation);

	float s1 = sinf(a->rotation);
	float s2 = sinf(b->rotation);

	vec2 norm = { 0 };
	//vec2 collisionPoint = { 0 };


	vec2 corners[8] = { 0 };
	int mult[2] = { 1,1 };
	mat2 rotate1 = {
		{	c1 ,s1 ,
			-s1, c1}
	};
	mat2 rotate2 = {
		{ c2 ,s2 ,
		-s2, c2 }
	};
	for (int i = 0; i < 4; i++)
	{
		vec2 co1 = { a->dim.x * mult[0]	, a->dim.y * mult[1] };
		vec2 co2 = { b->dim.x * mult[0]	, b->dim.y * mult[1] };
		//vec2 co1 = { a->dim.x * mult[0] * c1 + a->dim.y * mult[0] * -s1				, a->dim.x * mult[1] * -s1 + a->dim.y * mult[1] * c1 };
		//vec2 co2 = { b->dim.x * mult[0] * c1 + b->dim.y * mult[0] * -s1				, b->dim.x * mult[1] * -s1 + b->dim.y * mult[1] * c1 };
		corners[i] = mat2_add_vec2(&rotate1,&co1);
		corners[i].x += a->pos.x;
		corners[i].y += a->pos.y;
		corners[i + 4] = mat2_add_vec2(&rotate2, &co2);
		corners[i + 4].x += b->pos.x;
		corners[i + 4].y += b->pos.y;
		int ind = i & 1 ? 1 : 0;
		mult[ind] *= -1;
	}

	vec2 axis[4] = {
		{ corners[0].y - corners[1].y  ,-1 * (corners[0].x - corners[1].x) },
		{ corners[1].y - corners[2].y  ,-1 *( corners[1].x - corners[2].x )},
		{ corners[2 + 4].y - corners[3 + 4].y  ,-1 *( corners[2 + 4].x - corners[3 + 4].x)},
		{ corners[3 + 4].y - corners[0 + 4].y  ,-1 * (corners[3 + 4].x - corners[0 + 4].x) }
		};
	
	for (int i = 0; i < 4; i++)
		normalize_vec2(&axis[i]);

	vec2 mtv = { HUGE,HUGE };
	for(int i = 0; i < 4; i++)
	{
			float shortest1 = 454545;
			float longest1 = 454545;

			float shortest2 = 454545;
			float longest2 = 454545;

			vec2 collP = { 0 };

		for (int i2 = 0; i2 < 4; i2++) // find largest and shortest point
		{
			float o1 = vec2_point(&corners[i2], &axis[i]);
			float o2 = vec2_point(&corners[i2 + 4], &axis[i]);

			longest1 = o1 >= longest1 || (longest1 == 454545) ? o1 : longest1;
		


			shortest1 = (o1 <= shortest1) || (o1 == 454545) ? o1 : shortest1;



			longest2 = o2 >= longest2 || (longest2 == 454545) ? o2 : longest2;
			shortest2 = (o2 <= shortest2) || (shortest2 == 454545) ? o2 : shortest2;

		}
		if (shortest2 > longest1 || longest2 < shortest1) {
			return 0;
		}
		//calculate mtv
		float scalar = longest1 > longest2 ? -(longest2 - shortest1) : longest1 - shortest2;
		float currLen = vec2_lenght(&mtv);
		if(abs(scalar) < currLen)
		{
			mtv.x = axis[i].x * scalar;
			mtv.y = axis[i].y * scalar;

			norm = axis[i];
			//collisionPoint = collP;

		}
	}
	vec2 collisionPoint = { 0 };
	/*float lenght = HUGE;*/
	
	uint inserted = 0;
	for(int i = 0; i < 4; i++)
	{
		/*vec2 distance = { (corners[i].x - b->pos.x),(corners[i].y - b->pos.y) };
		float nLenght = distance.x * distance.x + distance.y * distance.y;
		
		collisionPoint = nLenght < lenght ? corners[i] : collisionPoint;
		lenght = nLenght < lenght ? nLenght : lenght;*/
		vec2 temp = rotate_point2D(b->rotation, corners[i], b->pos);
		vec2 nulllll = { 0 };
		if(AABB(temp,nulllll/*b->pos*/,b->dim))
		{
			if (b->dim.x == 400)
			{
				int abcd = 0;
			}
			collisionPoint = corners[i];
			inserted = 1;
			break;
		}

	}
	if(!inserted)
	{
		for (int i = 4; i < 8; i++)
		{
			/*vec2 distance = { corners[i + 4].x - a->pos.x,corners[i + 4].y - a->pos.y };
			float nLenght = distance.x * distance.x + distance.y * distance.y;

			collisionPoint = nLenght < lenght ? corners[i + 4] : collisionPoint;
			lenght = nLenght < lenght ? nLenght : lenght;*/
			vec2 temp = rotate_point2D(a->rotation, corners[i], a->pos);
			vec2 nulllll = { 0 };
			if (AABB(temp, nulllll/* a->pos*/, a->dim))
			{
				if(a->dim.x == 400)
				{
					int abcd = 0;
				}
				collisionPoint = corners[i];
				inserted = 1;
				break;
			}
		}
	}
	if(!inserted)
	{
		int abcd = 0;
		return 0;
	}
	assert(inserted);
	vec2 tdim = { 5 ,  5 };
	draw_box(drend, collisionPoint, tdim, 0.f);
	if(b->Move)
	{
		b->pos.x += mtv.x;
		b->pos.y += mtv.y;
	}
	else
	{
		a->pos.x -= mtv.x;
		a->pos.y -= mtv.y;
	}

	mtv.x = b->pos.x - a->pos.x;
	mtv.y = b->pos.y - a->pos.y;
	*collisionPointR = collisionPoint;
	*MTV = mtv;
	*normal = norm;
	return 1;
}
void force_to_body(Object* obj,float x, float y,vec2 force,DebugRend* rend)
{
	obj->forces.x += force.x;
	obj->forces.y += force.y;

	vec2 forceTEMP = { 0 };

	forceTEMP.x = cosf(obj->rotation) * force.x + (-sinf(obj->rotation) * force.y);
	forceTEMP.y = sinf(obj->rotation) * force.x + (cosf(obj->rotation) * force.y);



	float tempx = cosf(obj->rotation) * x + (-sinf(obj->rotation) * y);
	float tempy = sinf(obj->rotation) * x + (cosf(obj->rotation) * y);

	vec2 pos = { cosf(obj->rotation) * x + (-sinf(obj->rotation) * y) + obj->pos.x, sinf(obj->rotation) * x + (cosf(obj->rotation) * y) + obj->pos.y };
	vec2 dim = { 6,6 };

	vec2 pos2 = { (pos.x + force.x * 1),(pos.y + force.y * 1) };
	draw_line(rend, pos, pos2);
	draw_box(rend, pos, dim,obj->rotation);


	float perpDotX = -tempy;
	float perpDorY = tempx;

	obj->torque += perpDotX * force.x + perpDorY * force.y;
}

#define FOO(...)
#define MATH_0 vec2_sub(); vec2_();

void update_bodies(PhysicsContext* pc,float dt,Object** objects,int size, DebugRend* drend)
{
	FOO((vec2:a + vec2:d * float:c) / float:2)



	for (int i = 0; i < size; i++)
	{
		objects[i]->rotAcc = objects[i]->torque / objects[i]->momentumOfInteria;

		objects[i]->rotVelocity += objects[i]->rotAcc * dt;
		objects[i]->rotation += objects[i]->rotVelocity * dt;

		objects[i]->acceleration.x = objects[i]->forces.x / objects[i]->mass;
		objects[i]->acceleration.y = objects[i]->forces.y / objects[i]->mass;

		objects[i]->velocity.x += (objects[i]->acceleration.x + pc->gravity.x * objects[i]->Move) * dt;
		objects[i]->velocity.y += (objects[i]->acceleration.y + pc->gravity.y * objects[i]->Move) * dt;

		objects[i]->pos.x += objects[i]->velocity.x * dt;
		objects[i]->pos.y += objects[i]->velocity.y * dt;

		objects[i]->forces.x = 0;
		objects[i]->forces.y = 0;
		objects[i]->torque = 0;
	}

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
			for(int j = 0; j < colldata.num;j++)
			{
				if(colldata.buff[j].b == buffer.buffer[k])
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
		buffer.numObjs = 0;
	}
	for(int i = 0; i < colldata.num;i++)
	{
		vec2 mtv = { 0 };
		vec2 N = { 0 };
		vec2 CollisionPoint = { 0 };
		if(collides(colldata.buff[i].a, colldata.buff[i].b,&mtv,drend,&N, &CollisionPoint))
		{
			//vec2 N = { 0.f, 1.f };
			vec2 AP  = { -( CollisionPoint.y - colldata.buff[i].a->pos.y), CollisionPoint.x - colldata.buff[i].a->pos.x };
			vec2 BP = { -(CollisionPoint.y - colldata.buff[i].b->pos.y ),CollisionPoint.x  - colldata.buff[i].b->pos.x  };


			vec2 APV = { colldata.buff[i].a->velocity.x + colldata.buff[i].a->rotVelocity * AP.x ,colldata.buff[i].a->velocity.y + colldata.buff[i].a->rotVelocity * AP.y };
			vec2 BPV = { colldata.buff[i].b->velocity.x + colldata.buff[i].b->rotVelocity * BP.x ,colldata.buff[i].b->velocity.y + colldata.buff[i].b->rotVelocity * BP.y };
			vec2 ABP = { APV.x - BPV.x,  APV.y - BPV.y };
			//Object* temp = colldata.buff[i].a;
			//colldata.buff[i].a = colldata.buff[i].b;
			//colldata.buff[i].b = temp;

			float E = 0.5f;
			float div = (1 / colldata.buff[i].a->mass) + (1 / colldata.buff[i].b->mass);
			div = N.x * N.x * div + N.y * N.y * div;

			div += ((AP.x * N.x + AP.y * N.y) * (AP.x * N.x + AP.y * N.y)) / colldata.buff[i].a->momentumOfInteria;
			div += ((BP.x * N.x + BP.y * N.y) * (BP.x * N.x + BP.y * N.y)) / colldata.buff[i].b->momentumOfInteria;


			float EmultiPlier = -1 * (1 + E);
			//vec2 Dist = { colldata.buff[i].b->pos.x - colldata.buff[i].a->pos.x , colldata.buff[i].b->pos.y - colldata.buff[i].a->pos.y };
			float J = (EmultiPlier* ABP.x * N.x + EmultiPlier* ABP.y * N.y) / div;

			vec2 Avel = { (J / colldata.buff[i].a->mass) * N.x + colldata.buff[i].a->velocity.x , (J / colldata.buff[i].a->mass) * N.y + colldata.buff[i].a->velocity.y };
				//inserted = 1;
			colldata.buff[i].a->velocity = Avel;

			vec2 Bvel = { -1*(J / colldata.buff[i].b->mass) * N.x + colldata.buff[i].b->velocity.x , -1*(J / colldata.buff[i].b->mass) * N.y + colldata.buff[i].b->velocity.y };

			colldata.buff[i].b->velocity = Bvel;



			float angularAddOn = ((AP.x * N.x * J + AP.y * N.y * J) / colldata.buff[i].a->momentumOfInteria);
			colldata.buff[i].a->rotVelocity += angularAddOn;

			angularAddOn = ((BP.x * N.x * -J + BP.y * N.y * -J) / colldata.buff[i].b->momentumOfInteria);
			colldata.buff[i].b->rotVelocity += angularAddOn;

			//add_vec2(&colldata.buff[i].b->pos, &colldata.buff[i].b->pos, &mtv);
			draw_box(drend, colldata.buff[i].a->pos, colldata.buff[i].a->dim, colldata.buff[i].a->rotation);
			draw_box(drend, colldata.buff[i].b->pos, colldata.buff[i].b->dim, colldata.buff[i].b->rotation);
		}
	}
	dispose_buffer(&buffer);
	DISPOSE_ARRAY(colldata)
}
//https://en.wikipedia.org/wiki/List_of_moments_of_inertia
#undef DEFAULT_POOL_SIZE
#undef	MAX_OBJECTAMOUNT 
#undef	MAX_TREELEVEL 
#undef	DEFOBJBUFFS 