// The object modelling tasks performed by this file are usually done 
// by reading a scene configuration file or through a help of graphics user interface!!!

#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

typedef struct _material {
	glm::vec4 emission, ambient, diffuse, specular;
	GLfloat exponent;
} Material;

#define N_MAX_GEOM_COPIES 5
typedef struct _Object {
	char filename[512];

	GLenum front_face_mode; // clockwise or counter-clockwise
	int n_triangles;

	int n_fields; // 3 floats for vertex, 3 floats for normal, and 2 floats for texcoord
	GLfloat *vertices; // pointer to vertex array data
	GLfloat xmin, xmax, ymin, ymax, zmin, zmax; // bounding box <- compute this yourself

	GLuint VBO, VAO; // Handles to vertex buffer object and vertex array object

	int n_geom_instances;
	glm::mat4 ModelMatrix[N_MAX_GEOM_COPIES];
	Material material[N_MAX_GEOM_COPIES];
} Object;

#define N_MAX_STATIC_OBJECTS		15
Object static_objects[N_MAX_STATIC_OBJECTS]; // allocage memory dynamically every time it is needed rather than using a static array
int n_static_objects = 0;

glm::mat4 tiger_view_matrix, tiger_to_camera;



#define OBJ_BUILDING		0
#define OBJ_TABLE			1
#define OBJ_LIGHT			2
#define OBJ_TEAPOT			3
#define OBJ_NEW_CHAIR		4
#define OBJ_FRAME			5
#define OBJ_NEW_PICTURE		6
#define OBJ_COW				7
#define OBJ_IRONMAN			8
#define OBJ_BIKE			9
#define OBJ_BUS				10
#define OBJ_TANK			11
#define OBJ_GODZILLA		12

int read_geometry(GLfloat **object, int bytes_per_primitive, char *filename) {
	int n_triangles;
	FILE *fp;

	// fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: cannot open the object file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(&n_triangles, sizeof(int), 1, fp);
	*object = (float *)malloc(n_triangles*bytes_per_primitive);
	if (*object == NULL) {
		fprintf(stderr, "Error: cannot allocate memory for the geometry file %s ...\n", filename);
		exit(EXIT_FAILURE);
	}
	fread(*object, bytes_per_primitive, n_triangles, fp); // assume the data file has no faults.
														  // fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);
	fclose(fp);

	return n_triangles;
}

void compute_AABB(Object *obj_ptr) {
	// Do it yourself.
}

void prepare_geom_of_static_object(Object *obj_ptr) {
	int i, n_bytes_per_vertex, n_bytes_per_triangle;
//	char filename[512];

	n_bytes_per_vertex = obj_ptr->n_fields * sizeof(float); // 3 for vertex, 3 for normal, and 2 for texcoord
	n_bytes_per_triangle = 3 * n_bytes_per_vertex;

	obj_ptr->n_triangles = read_geometry(&(obj_ptr->vertices), n_bytes_per_triangle, obj_ptr->filename);

	// Initialize vertex buffer object.
	glGenBuffers(1, &(obj_ptr->VBO));

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glBufferData(GL_ARRAY_BUFFER, obj_ptr->n_triangles*n_bytes_per_triangle, obj_ptr->vertices, GL_STATIC_DRAW);

	compute_AABB(obj_ptr);
	free(obj_ptr->vertices);

	// Initialize vertex array object.
	glGenVertexArrays(1, &(obj_ptr->VAO));
	glBindVertexArray(obj_ptr->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, obj_ptr->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void define_static_objects(void) {
	// building
	strcpy(static_objects[OBJ_BUILDING].filename, "Data/Building1_vnt.geom");
	static_objects[OBJ_BUILDING].n_fields = 8;

	static_objects[OBJ_BUILDING].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUILDING]));

	static_objects[OBJ_BUILDING].n_geom_instances = 1;

	static_objects[OBJ_BUILDING].ModelMatrix[0] = glm::mat4(1.0f);

	static_objects[OBJ_BUILDING].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].ambient = glm::vec4(0.135f, 0.2225f, 0.1575f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].diffuse = glm::vec4(0.54f, 0.89f, 0.63f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].specular = glm::vec4(0.316228f, 0.316228f, 0.316228f, 1.0f);
	static_objects[OBJ_BUILDING].material[0].exponent = 128.0f*0.1f;

	// table
	strcpy(static_objects[OBJ_TABLE].filename, "Data/Table_vn.geom");
	static_objects[OBJ_TABLE].n_fields = 6;

	static_objects[OBJ_TABLE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TABLE]));

	static_objects[OBJ_TABLE].n_geom_instances = 2;

	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(157.0f, 76.5f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[0] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[0],
		glm::vec3(0.5f, 0.5f, 0.5f));

	static_objects[OBJ_TABLE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[0].ambient = glm::vec4(0.1f, 0.3f, 0.1f, 1.0f);
	static_objects[OBJ_TABLE].material[0].diffuse = glm::vec4(0.4f, 0.6f, 0.3f, 1.0f);
	static_objects[OBJ_TABLE].material[0].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[0].exponent = 15.0f;

	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(198.0f, 120.0f, 0.0f));
	static_objects[OBJ_TABLE].ModelMatrix[1] = glm::scale(static_objects[OBJ_TABLE].ModelMatrix[1],
		glm::vec3(0.8f, 0.6f, 0.6f));

	static_objects[OBJ_TABLE].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TABLE].material[1].ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
	static_objects[OBJ_TABLE].material[1].diffuse = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	static_objects[OBJ_TABLE].material[1].specular = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
	static_objects[OBJ_TABLE].material[1].exponent = 128.0f*0.078125f;

	// Light
	strcpy(static_objects[OBJ_LIGHT].filename, "Data/Light_vn.geom");
	static_objects[OBJ_LIGHT].n_fields = 6;

	static_objects[OBJ_LIGHT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(static_objects + OBJ_LIGHT);

	static_objects[OBJ_LIGHT].n_geom_instances = 5;

	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 100.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[0] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[0].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(80.0f, 47.5f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[1] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[1].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(40.0f, 130.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[2] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[2].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::translate(glm::mat4(1.0f), glm::vec3(190.0f, 60.0f, 49.0f));
	static_objects[OBJ_LIGHT].ModelMatrix[3] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[3],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[3].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[3].exponent = 128.0f*0.4f;

	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 112.5f, 49.0));
	static_objects[OBJ_LIGHT].ModelMatrix[4] = glm::rotate(static_objects[OBJ_LIGHT].ModelMatrix[4],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_LIGHT].material[4].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].ambient = glm::vec4(0.24725f, 0.1995f, 0.0745f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].diffuse = glm::vec4(0.75164f, 0.60648f, 0.22648f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].specular = glm::vec4(0.628281f, 0.555802f, 0.366065f, 1.0f);
	static_objects[OBJ_LIGHT].material[4].exponent = 128.0f*0.4f;

	// teapot
	strcpy(static_objects[OBJ_TEAPOT].filename, "Data/Teapotn_vn.geom");
	static_objects[OBJ_TEAPOT].n_fields = 6;

	static_objects[OBJ_TEAPOT].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TEAPOT]));

	static_objects[OBJ_TEAPOT].n_geom_instances = 1;

	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(193.0f, 120.0f, 11.0f));
	static_objects[OBJ_TEAPOT].ModelMatrix[0] = glm::scale(static_objects[OBJ_TEAPOT].ModelMatrix[0],
		glm::vec3(2.0f, 2.0f, 2.0f));

	static_objects[OBJ_TEAPOT].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].ambient = glm::vec4(0.1745f, 0.01175f, 0.01175f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].diffuse = glm::vec4(0.61424f, 0.04136f, 0.04136f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].specular = glm::vec4(0.727811f, 0.626959f, 0.626959f, 1.0f);
	static_objects[OBJ_TEAPOT].material[0].exponent = 128.0f*0.6;

	// new_chair
	strcpy(static_objects[OBJ_NEW_CHAIR].filename, "Data/new_chair_vnt.geom");
	static_objects[OBJ_NEW_CHAIR].n_fields = 8;

	static_objects[OBJ_NEW_CHAIR].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_CHAIR]));

	static_objects[OBJ_NEW_CHAIR].n_geom_instances = 1;

	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 104.0f, 0.0f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		glm::vec3(0.8f, 0.8f, 0.8f));
	static_objects[OBJ_NEW_CHAIR].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_CHAIR].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	static_objects[OBJ_NEW_CHAIR].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].ambient = glm::vec4(0.05f, 0.05f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].diffuse = glm::vec4(0.5f, 0.5f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].specular = glm::vec4(0.7f, 0.7f, 0.04f, 1.0f);
	static_objects[OBJ_NEW_CHAIR].material[0].exponent = 128.0f*0.078125f;

	// frame
	strcpy(static_objects[OBJ_FRAME].filename, "Data/Frame_vn.geom");
	static_objects[OBJ_FRAME].n_fields = 6;

	static_objects[OBJ_FRAME].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_FRAME]));

	static_objects[OBJ_FRAME].n_geom_instances = 3;

	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(188.0f, 116.0f, 30.0f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[0],
		glm::vec3(0.6f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[0] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[0].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[0].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[0].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[0].exponent = 128.0f*0.1f;


	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::translate(glm::mat4(1.0f), glm::vec3(63.0f, 50.0f, 36.5f));
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[1],
		glm::vec3(0.6f, 1.0f, 0.9f));
	static_objects[OBJ_FRAME].ModelMatrix[1] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[1],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_FRAME].material[1].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[1].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[1].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[1].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[1].exponent = 128.0f*0.1f;


	static_objects[OBJ_FRAME].ModelMatrix[2] = glm::translate(glm::mat4(1.0f), glm::vec3(122.0f, 130.0f, 31.0f));
	static_objects[OBJ_FRAME].ModelMatrix[2] = glm::scale(static_objects[OBJ_FRAME].ModelMatrix[2],
		glm::vec3(1.0f, 0.6f, 0.6f));
	static_objects[OBJ_FRAME].ModelMatrix[2] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_FRAME].ModelMatrix[2] = glm::rotate(static_objects[OBJ_FRAME].ModelMatrix[2],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));


	static_objects[OBJ_FRAME].material[2].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_FRAME].material[2].ambient = glm::vec4(0.19125f, 0.0735f, 0.0225f, 1.0f);
	static_objects[OBJ_FRAME].material[2].diffuse = glm::vec4(0.7038f, 0.27048f, 0.0828f, 1.0f);
	static_objects[OBJ_FRAME].material[2].specular = glm::vec4(0.256777f, 0.137622f, 0.086014f, 1.0f);
	static_objects[OBJ_FRAME].material[2].exponent = 128.0f*0.1f;


	// new_picture
	strcpy(static_objects[OBJ_NEW_PICTURE].filename, "Data/new_picture_vnt.geom");
	static_objects[OBJ_NEW_PICTURE].n_fields = 8;

	static_objects[OBJ_NEW_PICTURE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_NEW_PICTURE]));

	static_objects[OBJ_NEW_PICTURE].n_geom_instances = 1;

	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(189.5f, 116.0f, 30.0f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::scale(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		glm::vec3(13.5f*0.6f, 13.5f*0.6f, 13.5f*0.6f));
	static_objects[OBJ_NEW_PICTURE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_NEW_PICTURE].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));

	static_objects[OBJ_NEW_PICTURE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].ambient = glm::vec4(0.25f, 0.25f, 0.25f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].diffuse = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].specular = glm::vec4(0.774597f, 0.774597f, 0.774597f, 1.0f);
	static_objects[OBJ_NEW_PICTURE].material[0].exponent = 128.0f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_COW].filename, "Data/cow_vn.geom");
	static_objects[OBJ_COW].n_fields = 6;

	static_objects[OBJ_COW].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_COW]));

	static_objects[OBJ_COW].n_geom_instances = 1;

	static_objects[OBJ_COW].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(210.0f, 25.0f, 9.5f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::scale(static_objects[OBJ_COW].ModelMatrix[0],
		glm::vec3(30.0f, 30.0f, 30.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		180.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_COW].ModelMatrix[0] = glm::rotate(static_objects[OBJ_COW].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_COW].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_COW].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_COW].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
	static_objects[OBJ_COW].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_COW].material[0].exponent = 0.21794872f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_IRONMAN].filename, "Data/IronMan.geom");
	static_objects[OBJ_IRONMAN].n_fields = 8;

	static_objects[OBJ_IRONMAN].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_IRONMAN]));

	static_objects[OBJ_IRONMAN].n_geom_instances = 1;

	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(63.0f, 50.0f, 25.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::scale(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		glm::vec3(2.0f, 6.0f, 6.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_IRONMAN].ModelMatrix[0] = glm::rotate(static_objects[OBJ_IRONMAN].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_IRONMAN].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].diffuse = glm::vec4(0.78f, 0.0f, 0.2f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_IRONMAN].material[0].exponent = 0.21794872f*0.6f;


	// new_picture
	strcpy(static_objects[OBJ_BIKE].filename, "Data/Bike.geom");
	static_objects[OBJ_BIKE].n_fields = 8;

	static_objects[OBJ_BIKE].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BIKE]));

	static_objects[OBJ_BIKE].n_geom_instances = 1;

	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(120.0f, 130.0f, 25.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::scale(static_objects[OBJ_BIKE].ModelMatrix[0],
		glm::vec3(6.0f, 3.0f, 6.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BIKE].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_BIKE].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BIKE].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_BIKE].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BIKE].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_BIKE].material[0].diffuse = glm::vec4(0.6f, 0.3f, 0.0f, 1.0f);
	static_objects[OBJ_BIKE].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_BIKE].material[0].exponent = 0.21794872f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_BUS].filename, "Data/Bus.geom");
	static_objects[OBJ_BUS].n_fields = 8;

	static_objects[OBJ_BUS].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_BUS]));

	static_objects[OBJ_BUS].n_geom_instances = 1;

	static_objects[OBJ_BUS].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(207.0f, 150.0f, 0.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::scale(static_objects[OBJ_BUS].ModelMatrix[0],
		glm::vec3(1.0f, 1.0f, 1.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	static_objects[OBJ_BUS].ModelMatrix[0] = glm::rotate(static_objects[OBJ_BUS].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_BUS].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_BUS].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_BUS].material[0].diffuse = glm::vec4(0.2f, 0.3f, 0.8f, 1.0f);
	static_objects[OBJ_BUS].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_BUS].material[0].exponent = 0.21794872f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_TANK].filename, "Data/Tank.geom");
	static_objects[OBJ_TANK].n_fields = 8;

	static_objects[OBJ_TANK].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_TANK]));

	static_objects[OBJ_TANK].n_geom_instances = 1;

	static_objects[OBJ_TANK].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(43.0f, 5.0f, 0.0f));
	static_objects[OBJ_TANK].ModelMatrix[0] = glm::scale(static_objects[OBJ_TANK].ModelMatrix[0],
		glm::vec3(3.0f, 3.0f, 3.0f));

	static_objects[OBJ_TANK].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_TANK].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_TANK].material[0].diffuse = glm::vec4(0.1f, 0.74f, 0.2f, 1.0f);
	static_objects[OBJ_TANK].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_TANK].material[0].exponent = 0.21794872f*0.6f;

	// new_picture
	strcpy(static_objects[OBJ_GODZILLA].filename, "Data/Godzilla.geom");
	static_objects[OBJ_GODZILLA].n_fields = 8;

	static_objects[OBJ_GODZILLA].front_face_mode = GL_CCW;
	prepare_geom_of_static_object(&(static_objects[OBJ_GODZILLA]));

	static_objects[OBJ_GODZILLA].n_geom_instances = 1;

	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::translate(glm::mat4(1.0f), glm::vec3(203.0f, 120.0f, 11.0f));
	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::scale(static_objects[OBJ_GODZILLA].ModelMatrix[0],
		glm::vec3(0.03f, 0.03f, 0.03f));
	static_objects[OBJ_GODZILLA].ModelMatrix[0] = glm::rotate(static_objects[OBJ_GODZILLA].ModelMatrix[0],
		90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));

	static_objects[OBJ_GODZILLA].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].diffuse = glm::vec4(0.77f, 0.55f, 0.0f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
	static_objects[OBJ_GODZILLA].material[0].exponent = 0.21794872f*0.6f;


	tiger_to_camera = glm::translate(glm::mat4(1.0f), glm::vec3(-3.0f, 0.5f, 2.5f));
	tiger_to_camera = glm::rotate(tiger_to_camera,
		TO_RADIAN*90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

	n_static_objects = 13;
}

void draw_static_object(Object *obj_ptr, int instance_ID, int camera_index) {
	glFrontFace(obj_ptr->front_face_mode);

	ModelViewMatrix = ViewMatrix[camera_index] * obj_ptr->ModelMatrix[instance_ID];
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;


	ViewProjectionMatrix[camera_index] = ProjectionMatrix[camera_index] * ViewMatrix[camera_index];


	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, obj_ptr->material[instance_ID].diffuse.r,
		obj_ptr->material[instance_ID].diffuse.g, obj_ptr->material[instance_ID].diffuse.b);

	glBindVertexArray(obj_ptr->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * obj_ptr->n_triangles);
	glBindVertexArray(0);
}

GLuint VBO_axes, VAO_axes;
GLfloat vertices_axes[6][3] = {
	{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },
	{ 0.0f, 0.0f, 0.0f },{ 0.0f, 0.0f, 1.0f }
};
GLfloat axes_color[3][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f },{ 0.0f, 0.0f, 1.0f } };

void define_axes(void) {
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_axes), &vertices_axes[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_AXIS_LENGTH		60.0f
void draw_axes(int camera_index) {

	glBindVertexArray(VAO_axes);
	glUniform3fv(loc_primitive_color, 1, axes_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[1]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, axes_color[2]);
	glDrawArrays(GL_LINES, 4, 2);
	glBindVertexArray(0);
}





#define xposs 1.5f
#define yposs 1.0f
#define nearc 10.0f
#define farc 150.0f
#define fovc 70.0f

GLuint VBO_vol, VAO_vol;
GLfloat vertices_vol[20][3] = 
{
	{ 0.0f, 0.0f, 0.0f },{ 2.026609, 0.094711, 0.365902 },
{ 0.0f, 0.0f, 0.0f },{ -0.801928, -1.319392, -1.366059 },
{ 0.0f, 0.0f, 0.0f },{ -0.094711, -2.026609, 0.365902 },
{ 0.0f, 0.0f, 0.0f },{ 1.319392, 0.801928, -1.366058 },
{ 2.026609, 0.094711, 0.365902 },
{ -0.094711, -2.026609, 0.365902 },
{ -0.801928, -1.319392, -1.366059 },
{ 1.319392, 0.801928, -1.366058 },
{ 2.026609, 0.094711, 0.365902 },
{ -0.094711, -2.026609, 0.365902 },
{ -0.801928, -1.319392, -1.366059 },
{ 1.319392, 0.801928, -1.366058 },
{ 2.026609, 0.094711, 0.365902 },
{ -0.094711, -2.026609, 0.365902 },
{ -0.801928, -1.319392, -1.366059 },
{ 1.319392, 0.801928, -1.366058 }
};


GLfloat vol_color[3][3] = { { 1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f, 1.0f } };

void define_vol(void) {
	glGenBuffers(1, &VBO_vol);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_vol);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_vol), &vertices_vol[0][0], GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_vol);
	glBindVertexArray(VAO_vol);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_vol);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

#define WC_VOL_LENGTH		150.0f
void draw_vol(int camera_index) {

	
	ModelViewMatrix = glm::translate(ViewMatrix[camera_index], camera[6].prp);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(WC_VOL_LENGTH, WC_VOL_LENGTH, WC_VOL_LENGTH));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glBindVertexArray(VAO_vol);
	glUniform3fv(loc_primitive_color, 1, vol_color[0]);
	glDrawArrays(GL_LINES, 0, 2);
	glUniform3fv(loc_primitive_color, 1, vol_color[0]);
	glDrawArrays(GL_LINES, 2, 2);
	glUniform3fv(loc_primitive_color, 1, vol_color[0]);
	glDrawArrays(GL_LINES, 4, 2);
	glUniform3fv(loc_primitive_color, 1, vol_color[0]);
	glDrawArrays(GL_LINES, 6, 2);

	ModelViewMatrix = glm::translate(ViewMatrix[camera_index], camera[6].prp);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(nearc, nearc, nearc));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, vol_color[1]);
	glDrawArrays(GL_LINE_LOOP, 8, 4);

	ModelViewMatrix = glm::translate(ViewMatrix[camera_index], camera[6].prp);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(farc, farc, farc));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, vol_color[1]);
	glDrawArrays(GL_LINE_LOOP, 12, 4);

	ModelViewMatrix = glm::translate(ViewMatrix[camera_index], camera[6].prp);
	ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(fovc, fovc, fovc));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniform3fv(loc_primitive_color, 1, vol_color[2]);
	glDrawArrays(GL_LINE_LOOP, 16, 4);


	glBindVertexArray(0);
}



GLuint path_VBO, path_VAO;
GLfloat *path_vertices;
GLfloat **object;
int path_n_vertices;

int read_path_file(char filename[]) {
	int i, n_vertices;
	FILE *fp;
	float *flt_ptr;

	fprintf(stdout, "Reading path from the path file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the path file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d\n", &n_vertices);
	object = (GLfloat **)malloc((n_vertices+1) * sizeof(GLfloat*));
	path_vertices = (float *)malloc(n_vertices * 3 * sizeof(float));

	for (i = 0; i <= n_vertices; i++)
		object[i] = (GLfloat *)malloc(sizeof(GLfloat) * 3);

	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the path file %s ...", filename);
		return -1;
	}
	flt_ptr = path_vertices;
	for (i = 0; i < n_vertices; i++) {
		fscanf(fp, "%f %f %f\n", &object[i][0], &object[i][1], &object[i][2]);
		*flt_ptr = object[i][0];
		*(flt_ptr + 1) = object[i][1];
		*(flt_ptr + 2) = object[i][2];
		flt_ptr += 3;
	}
	fclose(fp);

	fprintf(stdout, "Read %d vertices successfully.\n\n", n_vertices);

	return n_vertices;
}

void prepare_path(void) { // Draw path.
						  //	return;

	// Initialize vertex array object.
	glGenVertexArrays(1, &path_VAO);
	glBindVertexArray(path_VAO);
	glGenBuffers(1, &path_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, path_VBO);
	glBufferData(GL_ARRAY_BUFFER, path_n_vertices * 3 * sizeof(float), path_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_path(void) {
	glBindVertexArray(path_VAO);
	glUniform3f(loc_primitive_color, 1.000f, 1.000f, 1.000f); 
	glDrawArrays(GL_LINE_STRIP, 0, path_n_vertices);
}





#define N_TIGER_FRAMES 12
Object tiger[N_TIGER_FRAMES];
struct {
	int cur_frame = 0;
	float rotation_angle = 0.0f;
} tiger_data;

void define_animated_tiger(void) {
	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		sprintf(tiger[i].filename, "Data/Tiger_%d%d_triangles_vnt.geom", i / 10, i % 10);

		tiger[i].n_fields = 8;
		tiger[i].front_face_mode = GL_CW;
		prepare_geom_of_static_object(&(tiger[i]));

		tiger[i].n_geom_instances = 1;

		tiger[i].ModelMatrix[0] = glm::scale(glm::mat4(1.0f), glm::vec3(0.2f, 0.2f, 0.2f));

		tiger[i].material[0].emission = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		tiger[i].material[0].ambient = glm::vec4(0.329412f, 0.223529f, 0.027451f, 1.0f);
		tiger[i].material[0].diffuse = glm::vec4(0.780392f, 0.568627f, 0.113725f, 1.0f);
		tiger[i].material[0].specular = glm::vec4(0.992157f, 0.941176f, 0.807843f, 1.0f);
		tiger[i].material[0].exponent = 128.0f*0.21794872f;
	}
}

int n_vernum = 0;

void draw_animated_tiger(int camera_index) {

	static GLfloat nowrotate = 270;
	static glm::vec3 nowvec = { -1, 0, 0 };
	static int gwan = 0, tmpn;
	static glm::vec3 tmpvec;
	static glm::vec2 t1, t2;
	float ttt;

	tmpn = n_vernum + 1;
	if (tmpn >= path_n_vertices)
		tmpn = 0;

	tmpvec.x = object[tmpn][0] - object[n_vernum][0];
	tmpvec.y = object[tmpn][1] - object[n_vernum][1];
	tmpvec.z = object[tmpn][2] - object[n_vernum][2];
	tmpvec = glm::normalize(tmpvec);

	if (!(object[tmpn][0] - object[n_vernum][0] == 0 && object[tmpn][1] - object[n_vernum][1] == 0)) {

		t2.x = tmpvec.x;
		t2.y = tmpvec.y;
		t1.x = nowvec.x;
		t1.y = nowvec.y;
		ttt = acosf((t1.x * t2.x + t1.y * t2.y)/
			(sqrt(t1.x*t1.x + t1.y*t1.y) * sqrt(t2.x*t2.x + t2.y*t2.y)));

		ttt *= TO_DEGREE;
		ttt = (t1.x * t2.y - t1.y * t2.x > 0.0f) ? ttt : -ttt;

		nowrotate += ttt;
		if (nowrotate > 360)
			nowrotate -= 360;
		if (nowrotate < 0)
			nowrotate += 360;

		nowvec = tmpvec;
		gwan = 1;
	}

	if (gwan == 0) {
		if ((nowrotate >= 357 &&  nowrotate <= 360) || (nowrotate >= 0 &&nowrotate <= 3))
			nowrotate = 0;
		else if (nowrotate >= 80 && nowrotate <= 100)
			nowrotate = 90;
		else if (nowrotate >= 170 && nowrotate <= 190)
			nowrotate = 180;
		else if (nowrotate >= 260 && nowrotate <= 280)
			nowrotate = 270;
	}

	ModelViewMatrix = glm::translate(ViewMatrix[camera_index], glm::vec3(object[n_vernum][0], object[n_vernum][1], object[n_vernum][2]));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, nowrotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	n_vernum += 1;
	if ( n_vernum >= path_n_vertices) {
		n_vernum = 0;
	}

	ModelViewMatrix *= tiger[tiger_data.cur_frame].ModelMatrix[0];
	tiger_view_matrix = ModelViewMatrix;							// for change view
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

	glUniform3f(loc_primitive_color, tiger[tiger_data.cur_frame].material[0].diffuse.r,
		tiger[tiger_data.cur_frame].material[0].diffuse.g, tiger[tiger_data.cur_frame].material[0].diffuse.b);

	glBindVertexArray(tiger[tiger_data.cur_frame].VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3 * tiger[tiger_data.cur_frame].n_triangles);
	glBindVertexArray(0);

	ModelViewProjectionMatrix = glm::scale(ModelViewProjectionMatrix, glm::vec3(20.0f, 20.0f, 20.0f));
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_axes(camera_index);
}

void cleanup_OpenGL_stuffs(void) {
	for (int i = 0; i < n_static_objects; i++) {
		glDeleteVertexArrays(1, &(static_objects[i].VAO));
		glDeleteBuffers(1, &(static_objects[i].VBO));
	}

	for (int i = 0; i < N_TIGER_FRAMES; i++) {
		glDeleteVertexArrays(1, &(tiger[i].VAO));
		glDeleteBuffers(1, &(tiger[i].VBO));
	}

	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);
}


/***************************************/

#define N_GEOMETRY_OBJECTS 3
#define GEOM_OBJ_ID_CAR_BODY 0
#define GEOM_OBJ_ID_CAR_WHEEL 1
#define GEOM_OBJ_ID_CAR_NUT 2

#define LOC_VERTEX 0
#define LOC_NORMAL 1
#define LOC_TEXCOORD 2

GLuint geom_obj_VBO[N_GEOMETRY_OBJECTS];
GLuint geom_obj_VAO[N_GEOMETRY_OBJECTS];

int geom_obj_n_triangles[N_GEOMETRY_OBJECTS];
GLfloat *geom_obj_vertices[N_GEOMETRY_OBJECTS];

// codes for the 'general' triangular-mesh object
typedef enum _GEOM_OBJ_TYPE { GEOM_OBJ_TYPE_V = 0, GEOM_OBJ_TYPE_VN, GEOM_OBJ_TYPE_VNT } GEOM_OBJ_TYPE;
// GEOM_OBJ_TYPE_V: (x, y, z)
// GEOM_OBJ_TYPE_VN: (x, y, z, nx, ny, nz)
// GEOM_OBJ_TYPE_VNT: (x, y, z, nx, ny, nz, s, t)
int GEOM_OBJ_ELEMENTS_PER_VERTEX[3] = { 3, 6, 8 };

int read_geometry_file(GLfloat **object, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int i, n_triangles;
	float *flt_ptr;
	FILE *fp;

	fprintf(stdout, "Reading geometry from the geometry file %s...\n", filename);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		fprintf(stderr, "Cannot open the geometry file %s ...", filename);
		return -1;
	}

	fscanf(fp, "%d", &n_triangles);
	*object = (float *)malloc(3 * n_triangles*GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float));
	if (*object == NULL) {
		fprintf(stderr, "Cannot allocate memory for the geometry file %s ...", filename);
		return -1;
	}

	flt_ptr = *object;
	for (i = 0; i < 3 * n_triangles * GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type]; i++)
		fscanf(fp, "%f", flt_ptr++);
	fclose(fp);

	fprintf(stdout, "Read %d primitives successfully.\n\n", n_triangles);

	return n_triangles;
}

void prepare_geom_obj(int geom_obj_ID, char *filename, GEOM_OBJ_TYPE geom_obj_type) {
	int n_bytes_per_vertex;

	n_bytes_per_vertex = GEOM_OBJ_ELEMENTS_PER_VERTEX[geom_obj_type] * sizeof(float);
	geom_obj_n_triangles[geom_obj_ID] = read_geometry_file(&geom_obj_vertices[geom_obj_ID], filename, geom_obj_type);

	// Initialize vertex array object.
	glGenVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glGenBuffers(1, &geom_obj_VBO[geom_obj_ID]);
	glBindBuffer(GL_ARRAY_BUFFER, geom_obj_VBO[geom_obj_ID]);
	glBufferData(GL_ARRAY_BUFFER, 3 * geom_obj_n_triangles[geom_obj_ID] * n_bytes_per_vertex,
		geom_obj_vertices[geom_obj_ID], GL_STATIC_DRAW);
	glVertexAttribPointer(LOC_VERTEX, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0);
	if (geom_obj_type >= GEOM_OBJ_TYPE_VN) {
		glVertexAttribPointer(LOC_NORMAL, 3, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (geom_obj_type >= GEOM_OBJ_TYPE_VNT) {
		glVertexAttribPointer(LOC_TEXCOORD, 2, GL_FLOAT, GL_FALSE, n_bytes_per_vertex, BUFFER_OFFSET(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	free(geom_obj_vertices[geom_obj_ID]);
}

void draw_geom_obj(int geom_obj_ID) {
	glBindVertexArray(geom_obj_VAO[geom_obj_ID]);
	glDrawArrays(GL_TRIANGLES, 0, 3 * geom_obj_n_triangles[geom_obj_ID]);
	glBindVertexArray(0);
}

void free_geom_obj(int geom_obj_ID) {
	glDeleteVertexArrays(1, &geom_obj_VAO[geom_obj_ID]);
	glDeleteBuffers(1, &geom_obj_VBO[geom_obj_ID]);
}