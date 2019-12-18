#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include "Shaders/LoadShaders.h"
#include "my_shading.h"
GLuint h_ShaderProgram_simple, h_ShaderProgram_PS, h_ShaderProgram_GS; // handles to shader programs

												   // for simple shaders
GLint loc_ModelViewProjectionMatrix_simple, loc_primitive_color;

// for Phong Shading shaders
#define NUMBER_OF_LIGHT_SUPPORTED 4 
GLint loc_global_ambient_color;
loc_light_Parameters loc_light[NUMBER_OF_LIGHT_SUPPORTED];
loc_Material_Parameters loc_material;
GLint loc_ModelViewProjectionMatrix_PS, loc_ModelViewMatrix_PS, loc_ModelViewMatrixInvTrans_PS;
GLint loc_ModelViewProjectionMatrix_GS, loc_ModelViewMatrix_GS, loc_ModelViewMatrixInvTrans_GS;
GLint loc_blind_effect, loc_screen_effect, loc_n_screen_effect, loc_n_blind_effect, loc_n_blind_width;
GLfloat loc_blind_width, loc_screen_width, loc_screen_num;


Light_Parameters light[NUMBER_OF_LIGHT_SUPPORTED];




// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, lookAt, perspective, etc.
#include <glm/gtc/matrix_inverse.hpp>
//glm::mat4 ModelViewProjectionMatrix;

typedef struct {
	glm::vec3 prp, vrp, vup; // in this example code, make vup always equal to the v direction.
	float fov_y, aspect_ratio, near_clip, far_clip, zoom_factor;
} CAMERA;

typedef struct {
	int x, y, w, h;
} VIEWPORT;

typedef enum {
	VIEW_WORLD, VIEW_SQUARE, VIEW_TIGER, VIEW_COW
} VIEW_MODE;

#define NUMBER_OF_CAMERAS 8 // main = 0, static = 1,2,3 , 정면 4, 측면 5, 상면 6, dynamic = 7

CAMERA camera[NUMBER_OF_CAMERAS];
VIEWPORT viewport[NUMBER_OF_CAMERAS];

// ViewProjectionMatrix = ProjectionMatrix * ViewMatrix
glm::mat4 ViewProjectionMatrix[NUMBER_OF_CAMERAS], ViewMatrix[NUMBER_OF_CAMERAS], ProjectionMatrix[NUMBER_OF_CAMERAS];
glm::mat4 ModelViewMatrix, tmpMatrix;
// ModelViewProjectionMatrix = ProjectionMatrix * ViewMatrix * ModelMatrix

glm::mat4 ModelMatrix_CAR_BODY, ModelMatrix_CAR_WHEEL, ModelMatrix_CAR_NUT, ModelMatrix_CAR_DRIVER;
glm::mat4 ModelMatrix_CAR_BODY_to_DRIVER; // computed only once in initialize_camera()
float rotation_angle_car = 0.0f;

glm::mat4 ModelViewProjectionMatrix; // This one is sent to vertex shader when it is ready.
glm::mat3 ModelViewMatrixInvTrans;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

int shading_method = 0;			// 0 == phong shading, 1 == gouraud shading
int flag_draw_screen, flag_screen_effect, flag_n_screen_effect, flag_blind_effect, flag_n_blind_effect, flag_cartoon_effect;
float screen_width, cartoon_levels, blind_width, screen_num, n_blind_width;

#include "Object_Definitions.h"


int animated_object = 0;

int car_rotate_flag = 0;
int draw_view_volume = 0;
int draw_path_flag = 0;

GLfloat frontwheel = 0.0f;

#define rad 1.7f
#define ww 1.0f



void draw_wheel_and_nut(int camera_index) {
	// angle is used in Hierarchical_Car_Correct later
	int i;
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glUniform3f(loc_primitive_color, 0.000f, 0.808f, 0.820f); // color name: DarkTurquoise
	draw_geom_obj(GEOM_OBJ_ID_CAR_WHEEL); // draw wheel

	for (i = 0; i < 5; i++) {
		ModelMatrix_CAR_NUT = glm::rotate(ModelMatrix_CAR_WHEEL, TO_RADIAN*72.0f*i, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix_CAR_NUT = glm::translate(ModelMatrix_CAR_NUT, glm::vec3(rad - 0.5f, 0.0f, ww));
		ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_NUT;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
		glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);

		glUniform3f(loc_primitive_color, 0.690f, 0.769f, 0.871f); // color name: LightSteelBlue
		draw_geom_obj(GEOM_OBJ_ID_CAR_NUT); // draw i-th nut
	}
}

void draw_car_dummy(int camera_index) {
	ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
	glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
	glUniform3f(loc_primitive_color, 0.498f, 1.000f, 0.831f); // color name: Aquamarine
	draw_geom_obj(GEOM_OBJ_ID_CAR_BODY); // draw body
	static float car_wheel_rotate = 90;


	car_wheel_rotate += 3;

	if (car_wheel_rotate > 360)
		car_wheel_rotate = 0;

	//front
	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), (car_wheel_rotate) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), (frontwheel)* TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, 4.5f)) *ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(camera_index);  // draw wheel 0


	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), car_wheel_rotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, 4.5f))* ModelMatrix_CAR_WHEEL;
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(camera_index);  // draw wheel 1


	//front
	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), (car_wheel_rotate) * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), (frontwheel)* TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(-3.9f, -3.5f, -4.5f))* ModelMatrix_CAR_WHEEL;
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(camera_index);  // draw wheel 2

	ModelMatrix_CAR_WHEEL = glm::rotate(glm::mat4(1.0f), car_wheel_rotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelMatrix_CAR_WHEEL = glm::translate(ModelMatrix_CAR_BODY, glm::vec3(3.9f, -3.5f, -4.5f))* ModelMatrix_CAR_WHEEL;
	ModelMatrix_CAR_WHEEL = glm::scale(ModelMatrix_CAR_WHEEL, glm::vec3(1.0f, 1.0f, -1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_WHEEL;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_wheel_and_nut(camera_index);  // draw wheel 3
}

void draw_animated_car(int camera_index) {
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
		ttt = acosf((t1.x * t2.x + t1.y * t2.y) /
			(sqrt(t1.x*t1.x + t1.y*t1.y) * sqrt(t2.x*t2.x + t2.y*t2.y)));

		ttt *= TO_DEGREE;
		ttt = (t1.x * t2.y - t1.y * t2.x > 0.0f) ? ttt : -ttt;
		int bu = 1;
		if (ttt != 0) {

			if (ttt < 0)
				bu = -1;
			car_rotate_flag++;
			if (car_rotate_flag < 19) {
				frontwheel += bu * 3;
			}
			else {
				frontwheel -= bu * 2;
			}
		}
		else {
			car_rotate_flag = 0;
			frontwheel = 0;
		}
		nowrotate += ttt;
		if (nowrotate > 360)
			nowrotate -= 360;
		if (nowrotate < 0)
			nowrotate += 360;

		nowvec = tmpvec;
		gwan = 1;
	}

	if (gwan == 0) {
		if ((nowrotate >= 357 && nowrotate <= 360) || (nowrotate >= 0 && nowrotate <= 3))
			nowrotate = 0;
		else if (nowrotate >= 80 && nowrotate <= 100)
			nowrotate = 90;
		else if (nowrotate >= 170 && nowrotate <= 190)
			nowrotate = 180;
		else if (nowrotate >= 260 && nowrotate <= 280)
			nowrotate = 270;
	}




	ModelMatrix_CAR_BODY = glm::translate(glm::mat4(1.0f), glm::vec3(object[n_vernum][0], object[n_vernum][1], object[n_vernum][2] + 10.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, nowrotate * TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));

	ModelMatrix_CAR_BODY = glm::scale(ModelMatrix_CAR_BODY, glm::vec3(1.5f, 1.5f, 1.5f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(1.0f, 0.0f, 0.0f));
	ModelMatrix_CAR_BODY = glm::rotate(ModelMatrix_CAR_BODY, 90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
	n_vernum += 1;

	if (n_vernum >= path_n_vertices) {
		n_vernum = 0;
	}

	ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index] * ModelMatrix_CAR_BODY;

	draw_car_dummy(camera_index);
}

void display_camera(int camera_index) {

	glViewport(viewport[camera_index].x, viewport[camera_index].y, viewport[camera_index].w, viewport[camera_index].h);

	glUseProgram(h_ShaderProgram_simple);

	ModelViewMatrix = glm::scale(ViewMatrix[camera_index], glm::vec3(WC_AXIS_LENGTH, WC_AXIS_LENGTH, WC_AXIS_LENGTH));
	ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	glLineWidth(2.0f);
	draw_axes(camera_index);
	glLineWidth(1.0f);


	if(draw_view_volume == 1) {
		glLineWidth(1.0f);
		draw_vol(camera_index);
		glLineWidth(1.0f);
	}

	if(draw_path_flag == 1) {
		ModelViewProjectionMatrix = ViewProjectionMatrix[camera_index];
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix_simple, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		draw_path();
	}

	if(shading_method == 0) {
		glUseProgram(h_ShaderProgram_PS);
	}
	else if(shading_method == 1){
		glUseProgram(h_ShaderProgram_GS);
	}


	glm::vec4 position_EC[4];
	glm::vec3 direction_EC[4];

//	 light 0
	//light[0].light_on = 0;
	//light[1].light_on = 1;
	//light[2].light_on = 0;
	glUniform1i(loc_light[0].light_on, light[0].light_on);
	position_EC[0] = ViewMatrix[camera_index] * glm::vec4(light[0].position[0], light[0].position[1],
		light[0].position[2], light[0].position[3]);
	glUniform4fv(loc_light[0].position, 1, &(position_EC[0][0]));
	glUniform4fv(loc_light[0].ambient_color, 1, light[0].ambient_color);
	glUniform4fv(loc_light[0].diffuse_color, 1, light[0].diffuse_color);
	glUniform4fv(loc_light[0].specular_color, 1, light[0].specular_color);

	direction_EC[0] = glm::mat3(ViewMatrix[camera_index]) * glm::vec3(light[0].spot_direction[0], light[0].spot_direction[1],
		light[0].spot_direction[2]);
	glUniform3fv(loc_light[0].spot_direction, 1, &(direction_EC[0][0]));
	glUniform1f(loc_light[0].spot_cutoff_angle, light[0].spot_cutoff_angle);
	glUniform1f(loc_light[0].spot_exponent, light[0].spot_exponent);



	// light 1
	glUniform1i(loc_light[1].light_on, light[1].light_on);
	position_EC[1] = ViewMatrix[camera_index] * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[1].position, 1, &(position_EC[1][0]));
	glUniform4fv(loc_light[1].ambient_color, 1, light[1].ambient_color);
	glUniform4fv(loc_light[1].diffuse_color, 1, light[1].diffuse_color);
	glUniform4fv(loc_light[1].specular_color, 1, light[1].specular_color);

	direction_EC[1] = glm::mat3(ViewMatrix[camera_index]) * glm::vec3(light[1].spot_direction[0], light[1].spot_direction[1],
		light[1].spot_direction[2]);
	glUniform3fv(loc_light[1].spot_direction, 1, &(direction_EC[1][0]));
	glUniform1f(loc_light[1].spot_cutoff_angle, light[1].spot_cutoff_angle);
	glUniform1f(loc_light[1].spot_exponent, light[1].spot_exponent);

	//light 2
	glUniform1i(loc_light[2].light_on, light[2].light_on);
	position_EC[2] = ViewMatrix[camera_index] * glm::vec4(light[1].position[0], light[1].position[1],
		light[1].position[2], light[1].position[3]);
	glUniform4fv(loc_light[2].position, 1, &(position_EC[2][0]));
	glUniform4fv(loc_light[2].ambient_color, 1, light[2].ambient_color);
	glUniform4fv(loc_light[2].diffuse_color, 1, light[2].diffuse_color);
	glUniform4fv(loc_light[2].specular_color, 1, light[2].specular_color);




	set_material_tiger();
	draw_animated_tiger(camera_index);


	if(flag_draw_screen == 1) {
		set_material_screen();
		ModelViewMatrix = glm::translate(ViewMatrix[camera_index], glm::vec3(110.0f, 81.0f, 0.0f));
		ModelViewMatrix = glm::scale(ModelViewMatrix, glm::vec3(30.0f, 30.0f, 45.0f));
		ModelViewMatrix = glm::rotate(ModelViewMatrix, -90.0f*TO_RADIAN, glm::vec3(0.0f, 1.0f, 0.0f));
		ModelViewProjectionMatrix = ProjectionMatrix[camera_index] * ModelViewMatrix;
		ModelViewMatrixInvTrans = glm::inverseTranspose(glm::mat3(ModelViewMatrix));

		if(shading_method == 0) {
			glUniform1i(loc_screen_effect, flag_screen_effect);
			glUniform1i(loc_n_screen_effect, flag_n_screen_effect);
			glUniformMatrix4fv(loc_ModelViewProjectionMatrix_PS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniformMatrix4fv(loc_ModelViewMatrix_PS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
			glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_PS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		}
		else if(shading_method == 1) {

			glUniformMatrix4fv(loc_ModelViewProjectionMatrix_GS, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
			glUniformMatrix4fv(loc_ModelViewMatrix_GS, 1, GL_FALSE, &ModelViewMatrix[0][0]);
			glUniformMatrix3fv(loc_ModelViewMatrixInvTrans_GS, 1, GL_FALSE, &ModelViewMatrixInvTrans[0][0]);
		}

		draw_screen();
		glUniform1i(loc_screen_effect, 0);
		glUniform1i(loc_n_screen_effect, 0);
	}

	only_for_geom_func = 0;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_BUILDING]), 0, camera_index);

	only_for_geom_func = 1;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_TABLE]), 0, camera_index);
	draw_static_object(&(static_objects[OBJ_TABLE]), 1, camera_index);


	only_for_geom_func = 2;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_LIGHT]), 0, camera_index);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 1, camera_index);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 2, camera_index);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 3, camera_index);
	draw_static_object(&(static_objects[OBJ_LIGHT]), 4, camera_index);


	only_for_geom_func = 3;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_TEAPOT]), 0, camera_index);

	only_for_geom_func = 4;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_NEW_CHAIR]), 0, camera_index);

	only_for_geom_func = 5;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_FRAME]), 0, camera_index);
	draw_static_object(&(static_objects[OBJ_FRAME]), 1, camera_index);
	draw_static_object(&(static_objects[OBJ_FRAME]), 2, camera_index);

	only_for_geom_func = 6;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_NEW_PICTURE]), 0, camera_index);

	only_for_geom_func = 7;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_COW]), 0, camera_index);

	only_for_geom_func = 8;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_IRONMAN]), 0, camera_index);

	only_for_geom_func = 9;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_BIKE]), 0, camera_index);

	only_for_geom_func = 10;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_BUS]), 0, camera_index);

	only_for_geom_func = 11;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_TANK]), 0, camera_index);

	only_for_geom_func = 12;
	set_material_static();
	draw_static_object(&(static_objects[OBJ_GODZILLA]), 0, camera_index);



	glUseProgram(0);
}



void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	display_camera(0);
	display_camera(1);
	display_camera(2);
	display_camera(3);
	display_camera(4);
	display_camera(5);
	display_camera(6);
	display_camera(7);

	glutSwapBuffers();
}

GLfloat eye[3] ={0.0f, 0.0f, 0.0f}, cen[3]={ 0.0f, 0.0f, 0.0f }, up[3]={ 0.0f, 0.0f, 0.0f };
GLfloat maincam_eye[3], maincam_cen[3], maincam_up[3];

unsigned int leftbutton_pressed = 0, rotation_mode_cow = 1, timestamp_cow = 0;
unsigned int scroll_pressed = 0;
int prevx, prevy;



int mode = 0;
void rotate_camera(GLfloat dx, GLfloat dy, GLfloat dz, int mode, int wh) {

	if (mode == 0) {
		int axs = wh % 3; // 어떤 축
		glm::mat4 rot_tmp, tttmp, ittmp;
		glm::vec3 uvn[3];
		glm::vec4 tvec;
		glm::vec3 uu, vv, nn;

		uu.x = ViewMatrix[0][0][0];
		uu.y = ViewMatrix[0][1][0];
		uu.z = ViewMatrix[0][2][0];

		vv.x = ViewMatrix[0][0][1];
		vv.y = ViewMatrix[0][1][1];
		vv.z = ViewMatrix[0][2][1];

		nn.x = ViewMatrix[0][0][2];
		nn.y = ViewMatrix[0][1][2];
		nn.z = ViewMatrix[0][2][2];

		uvn[0] = uu;
		uvn[1] = vv;
		uvn[2] = nn;

		if (wh >= 3) {
			tttmp = glm::rotate(glm::mat4(1.0f), 1.0f * TO_RADIAN, glm::vec3(uvn[axs]));
		}
		else {
			tttmp = glm::rotate(glm::mat4(1.0f), -1.0f * TO_RADIAN, glm::vec3(uvn[axs]));
		}

		ittmp = glm::inverse(tttmp);
		ittmp = glm::transpose(ittmp);

		uvn[0] = glm::mat3(ittmp) * uvn[0];
		uvn[1] = glm::mat3(ittmp) * uvn[1];
		uvn[2] = glm::mat3(ittmp) * uvn[2];

		rot_tmp[0][0] = uvn[0].x;
		rot_tmp[1][0] = uvn[0].y;
		rot_tmp[2][0] = uvn[0].z;

		rot_tmp[0][1] = uvn[1].x;
		rot_tmp[1][1] = uvn[1].y;
		rot_tmp[2][1] = uvn[1].z;

		rot_tmp[0][2] = uvn[2].x;
		rot_tmp[1][2] = uvn[2].y;
		rot_tmp[2][2] = uvn[2].z;

		rot_tmp[0][3] = 0;
		rot_tmp[1][3] = 0;
		rot_tmp[2][3] = 0;
		rot_tmp[3][0] = 0;
		rot_tmp[3][1] = 0;
		rot_tmp[3][2] = 0;
		rot_tmp[3][3] = 1;

		rot_tmp = rot_tmp * glm::translate(glm::mat4(1.0f), glm::vec3(-camera[0].prp.x, -camera[0].prp.y, -camera[0].prp.z));
		ViewMatrix[0] = rot_tmp;
		
		glutPostRedisplay();
	}
	else {
		int axs = wh % 3; // 어떤 축
		glm::mat4 rot_tmp, tttmp, ittmp;
		glm::vec3 uvn[3];
		glm::vec4 tvec;
		glm::vec3 uu, vv, nn;

		uu.x = ViewMatrix[0][0][0];
		uu.y = ViewMatrix[0][1][0];
		uu.z = ViewMatrix[0][2][0];

		vv.x = ViewMatrix[0][0][1];
		vv.y = ViewMatrix[0][1][1];
		vv.z = ViewMatrix[0][2][1];

		nn.x = ViewMatrix[0][0][2];
		nn.y = ViewMatrix[0][1][2];
		nn.z = ViewMatrix[0][2][2];

		uvn[0] = uu;
		uvn[1] = vv;
		uvn[2] = nn;

		if (wh >= 3) {

			tttmp = glm::rotate(glm::mat4(1.0f), 1.0f * TO_RADIAN, glm::vec3(uvn[axs]));
			tttmp = glm::translate(tttmp, glm::vec3(uvn[axs]));
		}
		else {
			tttmp = glm::translate(glm::mat4(1.0f), glm::vec3(uvn[axs]));
			tttmp = glm::rotate(tttmp, -1.0f * TO_RADIAN, glm::vec3(uvn[axs]));
		}

		ittmp = glm::inverse(tttmp);
		ittmp = glm::transpose(ittmp);

		uvn[0] = glm::mat3(tttmp) * uvn[0];
		uvn[1] = glm::mat3(tttmp) * uvn[1];
		uvn[2] = glm::mat3(tttmp) * uvn[2];

		uvn[0] = glm::mat3(glm::translate(glm::mat4(1.0f), glm::vec3(uvn[0])))*uvn[0];
		uvn[1] = glm::mat3(glm::translate(glm::mat4(1.0f), glm::vec3(uvn[1])))*uvn[1];
		uvn[2] = glm::mat3(glm::translate(glm::mat4(1.0f), glm::vec3(uvn[2])))*uvn[2];

		ViewMatrix[0][0][0] = uvn[0].x;
		ViewMatrix[0][1][0] = uvn[0].y;
		ViewMatrix[0][2][0] = uvn[0].z;

		ViewMatrix[0][0][1] = uvn[1].x;
		ViewMatrix[0][1][1] = uvn[1].y;
		ViewMatrix[0][2][1] = uvn[1].z;

		ViewMatrix[0][0][2] = uvn[2].x;
		ViewMatrix[0][1][2] = uvn[2].y;
		ViewMatrix[0][2][2] = uvn[2].z;

		glutPostRedisplay();
	}
}

void swap(int *a, int *b) {
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}
void keyboard(unsigned char key, int x, int y) {
	static int flag_cull_face = 0, polygon_fill_on = 0, depth_test_on = 0;
	static int cur_camera = 0, cur_camera_move = 0;


	if ((key >= '7') && (key <= '7' + 2)) {
		int light_ID = (int)(key - '7');

		if (shading_method == 0)
			glUseProgram(h_ShaderProgram_PS);
		else if(shading_method == 1)
			glUseProgram(h_ShaderProgram_GS);
		light[light_ID].light_on = 1 - light[light_ID].light_on;
		glUniform1i(loc_light[light_ID].light_on, light[light_ID].light_on);
		glUseProgram(0);

		glutPostRedisplay();
		return;
	}

	switch (key) {
	case 27: // ESC key

		for (int i = 0; i <= path_n_vertices; i++)
			free(object[i]);
		free(object);
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	case 'c':
		flag_cull_face = (flag_cull_face + 1) % 3;
		switch (flag_cull_face) {
		case 0:
			glDisable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ No faces are culled.\n");
			break;
		case 1: // cull back faces;
			glCullFace(GL_BACK);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Back faces are culled.\n");
			break;
		case 2: // cull front faces;
			glCullFace(GL_FRONT);
			glEnable(GL_CULL_FACE);
			glutPostRedisplay();
			fprintf(stdout, "^^^ Front faces are culled.\n");
			break;
		}
		break;
	case 'f':
		polygon_fill_on = 1 - polygon_fill_on;
		if (polygon_fill_on) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			fprintf(stdout, "^^^ Polygon filling enabled.\n");
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			fprintf(stdout, "^^^ Line drawing enabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'd':
		depth_test_on = 1 - depth_test_on;
		if (depth_test_on) {
			glEnable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test enabled.\n");
		}
		else {
			glDisable(GL_DEPTH_TEST);
			fprintf(stdout, "^^^ Depth test disabled.\n");
		}
		glutPostRedisplay();
		break;
	case 'k': // move camera x

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(1.0f, 0.0f, 0.0f));

		glutPostRedisplay();
		break;
	case 'l': // move camera x

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(-1.0f, 0.0f, 0.0f));

		glutPostRedisplay();
		break;
	case 'i': // move camera y

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 1.0f, 0.0f));
		glutPostRedisplay();
		break;
	case 'o': // move camera y

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, -1.0f, 0.0f));
		glutPostRedisplay();
		break;
	case '.': //move camera z

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 0.0f, 1.0f));
		glutPostRedisplay();
		break;
	case ',': // move camera z

		ViewMatrix[0] = glm::translate(ViewMatrix[0], glm::vec3(0.0f, 0.0f, -1.0f));
		glutPostRedisplay();
		break;
	case 'z':
		mode = 1 - mode;
		break;
	case '1': // x rotate
		rotate_camera(1.0f, 0.0f, 0.0f, mode, 0);

		break;
	case '2': // x rotate
		rotate_camera(-1.0f, 0.0f, 0.0f, mode, 3);

		break;

	case '3': // y rotate
		rotate_camera(0.0f, 1.0f, 0.0f, mode, 1);

		break;
	case '4': // y rotate
		rotate_camera(0.0f, -1.0f, 0.0f, mode, 4);

		break;
	case '5': // z rotate
		rotate_camera(0.0f, 0.0f, 1.0f, mode, 2);

		break;
	case '6': // z rotate
		rotate_camera(0.0f, 0.0f, -1.0f, mode, 5);

		break;
	case '[': // 축소
		camera[0].fov_y++;
		ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case ']': // 확대
		if (camera[0].fov_y > 1)
			camera[0].fov_y--;
		ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];
		break;
	case 't': // 동적 cctv, main camera frame 교환
		cur_camera = 1 - cur_camera;		// 0이면 main camera가 큰 프레임, 1이면 동적 cctv가 작은 프레임.
		swap(&(viewport[0].x), &(viewport[7].x));
		swap(&(viewport[0].y), &(viewport[7].y));
		swap(&(viewport[0].w), &(viewport[7].w));
		swap(&(viewport[0].h), &(viewport[7].h));

		ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
		ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

		ProjectionMatrix[7] = glm::perspective(camera[7].fov_y*TO_RADIAN, camera[7].aspect_ratio, camera[7].near_clip, camera[7].far_clip);
		ViewProjectionMatrix[7] = ProjectionMatrix[7] * ViewMatrix[7];
		break;

	case 'v':
		draw_view_volume = 1 - draw_view_volume;
		break;
	case 'p':
		draw_path_flag = 1 - draw_path_flag;
		break;
	case 's':
		flag_draw_screen = 1 - flag_draw_screen;
		glutPostRedisplay();
		break;
	case 'e':
		if(flag_draw_screen && flag_n_screen_effect == 0) {
			flag_screen_effect  = 1 - flag_screen_effect;
			glutPostRedisplay();
		}
		break;
	case 'r':
		if(flag_draw_screen && flag_screen_effect == 0) {
			flag_n_screen_effect  = 1 - flag_n_screen_effect;
			glutPostRedisplay();
		}
		break;
	case 'a':
		if(flag_n_blind_effect == 0) {
			flag_blind_effect = 1 - flag_blind_effect;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1i(loc_blind_effect, flag_blind_effect);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
	case 'y':
		if(flag_blind_effect == 0) {
			flag_n_blind_effect = 1 - flag_n_blind_effect;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1i(loc_n_blind_effect, flag_n_blind_effect);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
	case 'b':
		if(flag_blind_effect) {
			if(blind_width >= 10.0f)
				blind_width -=10.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_blind_width, blind_width);
			glUseProgram(0);
			glutPostRedisplay();
		}
		if(flag_n_blind_effect) {
			if(n_blind_width >= 3.0f)
				n_blind_width -=1.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_n_blind_width, n_blind_width);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;

	case 'x':
		if(flag_blind_effect) {
			if(blind_width < 370.f)
				blind_width +=10.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_blind_width, blind_width);
			glUseProgram(0);
			glutPostRedisplay();
		}
		if(flag_n_blind_effect) {
			if(n_blind_width < 10.0f)
				n_blind_width += 1.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_n_blind_width, n_blind_width);
			glUseProgram(0);
			glutPostRedisplay();
		}
		break;
	case 'n':
		if(mode == 0) {
			if(screen_width <= 0.5f)
				screen_width +=0.025f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_width, screen_width);
			glUseProgram(0);
		}
		else {
			if(screen_num <= 5.0f)
				screen_num +=1.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_num, screen_num);
			glUseProgram(0);
		}
		break;
	case 'm':
		if(mode == 0) {
			if(screen_width > 0.05f)
				screen_width -=0.025f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_width, screen_width);
			glUseProgram(0);
		}
		else {
			if(screen_num > 0.0f)
				screen_num -= 1.0f;
			glUseProgram(h_ShaderProgram_PS);
			glUniform1f(loc_screen_num, screen_num);
			glUseProgram(0);
		}
		break;
	}
	
}

void mousepress(int button, int state, int x, int y) {
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)) {
		prevx = x, prevy = y;
		leftbutton_pressed = 1;
		glutPostRedisplay();
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)) {
		leftbutton_pressed = 0;
		glutPostRedisplay();
	}

	else if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)) {
		if(glutGetModifiers() & GLUT_ACTIVE_SHIFT) {
			shading_method = 1;
			glutPostRedisplay();
		}
		else {
			shading_method = 0;
			glutPostRedisplay();
		}
	}
	else if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)) {
			shading_method = 0;
			glutPostRedisplay();
	}
	
	else if (button == 3 || (button == 4)) {
		scroll_pressed = button;
	}
	else {
		scroll_pressed = 0;
	}
}

#define CAM_ROT_SENSITIVITY 0.15f
#define ROT_SENSITI 0.15f

GLfloat CCTV_r = 50;
GLfloat now_CCTV_rot = 180;

void motion_1(int x, int y) {
	glm::mat4 mat4_tmp;
	glm::vec3 vec3_tmp;
	float delx, dely, tmp;

	if (leftbutton_pressed) {
		delx = (float)(x - prevx), dely = -(float)(y - prevy);
		prevx = x, prevy = y;

		now_CCTV_rot += delx * ROT_SENSITI;

		if (now_CCTV_rot > 360)
			now_CCTV_rot = 0;
		else if (now_CCTV_rot < 0)
			now_CCTV_rot = 360;

		if ((CCTV_r + dely * CAM_ROT_SENSITIVITY > 0) && (CCTV_r + dely * CAM_ROT_SENSITIVITY <= 55))
			CCTV_r += dely * CAM_ROT_SENSITIVITY;

		camera[7].vrp.x = camera[7].prp.x +  CCTV_r* cos(now_CCTV_rot*TO_RADIAN);
		camera[7].vrp.y = camera[7].prp.y +  CCTV_r* sin(now_CCTV_rot * TO_RADIAN);

		tmp = camera[7].vup.z;

		ViewMatrix[7] = glm::lookAt(camera[7].prp, camera[7].vrp, camera[7].vup);
		camera[7].vup = glm::vec3(ViewMatrix[7][0].y, ViewMatrix[7][1].y, ViewMatrix[7][2].y);

		camera[7].vup.z = tmp;

		glutPostRedisplay();
	}
}

void mouseWheel(int button, int dir, int x, int y)
{
	if (dir > 0)
	{
		if (camera[7].fov_y > 25)
			camera[7].fov_y--;
	}
	else
	{
		if (camera[7].fov_y < 100)
			camera[7].fov_y++;
	}
	ProjectionMatrix[7] = glm::perspective(camera[7].fov_y*TO_RADIAN, camera[7].aspect_ratio, camera[7].near_clip, camera[7].far_clip);
	ViewProjectionMatrix[7] = ProjectionMatrix[7] * ViewMatrix[7];

	glutPostRedisplay();
	return;
}

void reshape(int width, int height) {

	camera[0].aspect_ratio = (float)width / height;	// main camera
	viewport[0].x = viewport[0].y = 0;
	viewport[0].w = (int)(0.7475f*width); viewport[0].h = (int)(0.7475f*height);
	ProjectionMatrix[0] = glm::perspective(camera[0].fov_y*TO_RADIAN, camera[0].aspect_ratio, camera[0].near_clip, camera[0].far_clip);
	ViewProjectionMatrix[0] = ProjectionMatrix[0] * ViewMatrix[0];

	camera[1].aspect_ratio = camera[0].aspect_ratio; // 상면
	viewport[1].x = (int)(0.0f*width); viewport[1].y = (int)(0.755f*height);
	viewport[1].w = (int)(0.2475f*width); viewport[1].h = (int)(0.2475*height);
	ProjectionMatrix[1] = glm::ortho(-150.0f, 150.0f, -100.0f, 100.0f, -200.0f, 200.0f);
	ViewProjectionMatrix[1] = ProjectionMatrix[1];

	camera[2].aspect_ratio = camera[1].aspect_ratio; // 정면
	viewport[2].x = (int)(0.2525f*width); viewport[2].y = (int)(0.755f*height);
	viewport[2].w = (int)(0.2475f*width); viewport[2].h = (int)(0.2475*height);
	ProjectionMatrix[2] = glm::ortho(-100.0f, 100.0f, -50.0f, 50.0f, -200.0f, 200.0f);
	ViewProjectionMatrix[2] = ProjectionMatrix[2];

	camera[3].aspect_ratio = camera[2].aspect_ratio; // 측면
	viewport[3].x = (int)(0.5025f*width); viewport[3].y = (int)(0.755f*height);
	viewport[3].w = (int)(0.2475f*width); viewport[3].h = (int)(0.2475*height);
	ProjectionMatrix[3] = glm::ortho(-120.0f, 120.0f, -60.0f, 60.0f, -200.0f, 200.0f);
	ViewProjectionMatrix[3] = ProjectionMatrix[3];

	camera[4].aspect_ratio = camera[3].aspect_ratio; // static 1
	viewport[4].x = (int)(0.7525f*width); viewport[4].y = (int)(0.5025f*height);
	viewport[4].w = (int)(0.2475f*width); viewport[4].h = (int)(0.2475*height);
	ProjectionMatrix[4] = glm::perspective(camera[4].fov_y*TO_RADIAN, camera[4].aspect_ratio, camera[4].near_clip, camera[4].far_clip);
	ViewProjectionMatrix[4] = ProjectionMatrix[4] * ViewMatrix[4];

	camera[5].aspect_ratio = camera[4].aspect_ratio; // static 2
	viewport[5].x = (int)(0.7525f*width); viewport[5].y = (int)(0.2525f*height);
	viewport[5].w = (int)(0.2475f*width); viewport[5].h = (int)(0.2475*height);
	ProjectionMatrix[5] = glm::perspective(camera[5].fov_y*TO_RADIAN, camera[5].aspect_ratio, camera[5].near_clip, camera[5].far_clip);
	ViewProjectionMatrix[5] = ProjectionMatrix[5] * ViewMatrix[5];

	camera[6].aspect_ratio = camera[5].aspect_ratio; // static 3
	viewport[6].x = (int)(0.7525f*width); viewport[6].y = (int)(0.0f*height);
	viewport[6].w = (int)(0.2475f*width); viewport[6].h = (int)(0.2475*height);
	ProjectionMatrix[6] = glm::perspective(camera[6].fov_y*TO_RADIAN, camera[6].aspect_ratio, camera[6].near_clip, camera[6].far_clip);
	ViewProjectionMatrix[6] = ProjectionMatrix[6] * ViewMatrix[6];

	camera[7].aspect_ratio = camera[6].aspect_ratio; // dynamic cctv
	viewport[7].x = (int)(0.7525f*width); viewport[7].y = (int)(0.755f*height);
	viewport[7].w = (int)(0.2475f*width); viewport[7].h = (int)(0.2475*height);
	ProjectionMatrix[7] = glm::perspective(camera[7].fov_y*TO_RADIAN, camera[7].aspect_ratio, camera[7].near_clip, camera[7].far_clip);
	ViewProjectionMatrix[7] = ProjectionMatrix[7] * ViewMatrix[7];
	
	glutPostRedisplay();
}

void timer_scene(int timestamp_scene) {
	tiger_data.cur_frame = timestamp_scene % N_TIGER_FRAMES;
	tiger_data.rotation_angle = (timestamp_scene % 360)*TO_RADIAN;
	glutPostRedisplay();
	glutTimerFunc(100, timer_scene, (timestamp_scene + 1) % INT_MAX);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mousepress);
	glutMouseWheelFunc(mouseWheel);
	glutMotionFunc(motion_1);
	glutReshapeFunc(reshape);
	glutTimerFunc(100, timer_scene, 0);
	glutCloseFunc(cleanup_OpenGL_stuffs);
}

void prepare_shader_program(void) {
	int i;
	char string[256];
	ShaderInfo shader_info_simple[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};
	ShaderInfo shader_info_PS[3] = {
		{ GL_VERTEX_SHADER, "Shaders/Phong.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Phong.frag" },
		{ GL_NONE, NULL }
	};

	ShaderInfo shader_info_GS[3] ={
		{ GL_VERTEX_SHADER, "Shaders/Gouraud.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/Gouraud.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram_simple = LoadShaders(shader_info_simple);
	loc_ModelViewProjectionMatrix_simple = glGetUniformLocation(h_ShaderProgram_simple, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram_simple, "u_primitive_color");



	h_ShaderProgram_PS = LoadShaders(shader_info_PS);
	loc_ModelViewProjectionMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_PS = glGetUniformLocation(h_ShaderProgram_PS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_global_ambient_color");
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_PS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_PS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_PS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_PS, "u_material.specular_exponent");


	loc_screen_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_screen_effect");
	loc_n_screen_effect = glGetUniformLocation(h_ShaderProgram_PS, "u_nscreen_effect");
	loc_screen_width = glGetUniformLocation(h_ShaderProgram_PS, "u_screen_width");
	loc_screen_num = glGetUniformLocation(h_ShaderProgram_PS, "u_screen_num");

	loc_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "uu_blind_effect");
	loc_n_blind_effect = glGetUniformLocation(h_ShaderProgram_PS, "uu_nblind_effect");
	loc_n_blind_width = glGetUniformLocation(h_ShaderProgram_PS, "uu_nblind_width");
	loc_blind_width = glGetUniformLocation(h_ShaderProgram_PS, "uuu_blind_width");





	h_ShaderProgram_GS = LoadShaders(shader_info_GS);
	loc_ModelViewProjectionMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewProjectionMatrix");
	loc_ModelViewMatrix_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrix");
	loc_ModelViewMatrixInvTrans_GS = glGetUniformLocation(h_ShaderProgram_GS, "u_ModelViewMatrixInvTrans");

	loc_global_ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_global_ambient_color");
	for(i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		sprintf(string, "u_light[%d].light_on", i);
		loc_light[i].light_on = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].position", i);
		loc_light[i].position = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].ambient_color", i);
		loc_light[i].ambient_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].diffuse_color", i);
		loc_light[i].diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].specular_color", i);
		loc_light[i].specular_color = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_direction", i);
		loc_light[i].spot_direction = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_exponent", i);
		loc_light[i].spot_exponent = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].spot_cutoff_angle", i);
		loc_light[i].spot_cutoff_angle = glGetUniformLocation(h_ShaderProgram_GS, string);
		sprintf(string, "u_light[%d].light_attenuation_factors", i);
		loc_light[i].light_attenuation_factors = glGetUniformLocation(h_ShaderProgram_GS, string);
	}

	loc_material.ambient_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.ambient_color");
	loc_material.diffuse_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.diffuse_color");
	loc_material.specular_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_color");
	loc_material.emissive_color = glGetUniformLocation(h_ShaderProgram_GS, "u_material.emissive_color");
	loc_material.specular_exponent = glGetUniformLocation(h_ShaderProgram_GS, "u_material.specular_exponent");

}

void initialize_lights_and_material(void) { // follow OpenGL conventions for initialization
	int i;


	glUseProgram(h_ShaderProgram_PS);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for (i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if (i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]

	glUniform1i(loc_screen_effect, 0);
	glUniform1i(loc_n_screen_effect, 0);
	glUniform1f(loc_screen_width, 0.125f);
	glUniform1f(loc_screen_num, 0.0f);

	glUniform1i(loc_blind_effect, 0);
	glUniform1i(loc_n_blind_effect, 0);
	glUniform1f(loc_n_blind_width, 2.0f);
	glUniform1f(loc_blind_width, 90.0f);




	glUseProgram(h_ShaderProgram_GS);

	glUniform4f(loc_global_ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	for(i = 0; i < NUMBER_OF_LIGHT_SUPPORTED; i++) {
		glUniform1i(loc_light[i].light_on, 0); // turn off all lights initially
		glUniform4f(loc_light[i].position, 0.0f, 0.0f, 1.0f, 0.0f);
		glUniform4f(loc_light[i].ambient_color, 0.0f, 0.0f, 0.0f, 1.0f);
		if(i == 0) {
			glUniform4f(loc_light[i].diffuse_color, 1.0f, 1.0f, 1.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 1.0f, 1.0f, 1.0f, 1.0f);
		}
		else {
			glUniform4f(loc_light[i].diffuse_color, 0.0f, 0.0f, 0.0f, 1.0f);
			glUniform4f(loc_light[i].specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		glUniform3f(loc_light[i].spot_direction, 0.0f, 0.0f, -1.0f);
		glUniform1f(loc_light[i].spot_exponent, 0.0f); // [0.0, 128.0]
		glUniform1f(loc_light[i].spot_cutoff_angle, 180.0f); // [0.0, 90.0] or 180.0 (180.0 for no spot light effect)
		glUniform4f(loc_light[i].light_attenuation_factors, 1.0f, 0.0f, 0.0f, 0.0f); // .w != 0.0f for no ligth attenuation
	}

	glUniform4f(loc_material.ambient_color, 0.2f, 0.2f, 0.2f, 1.0f);
	glUniform4f(loc_material.diffuse_color, 0.8f, 0.8f, 0.8f, 1.0f);
	glUniform4f(loc_material.specular_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform4f(loc_material.emissive_color, 0.0f, 0.0f, 0.0f, 1.0f);
	glUniform1f(loc_material.specular_exponent, 0.0f); // [0.0, 128.0]



	glUseProgram(0);
}

void initialize_OpenGL(void) {
	glEnable(GL_DEPTH_TEST); // Default state
	 
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glClearColor(0.12f, 0.18f, 0.12f, 1.0f);


	// initialize the 0th camera.
	camera[0].prp = glm::vec3(600.0f, 600.0f, 500.0f);
	camera[0].vrp = glm::vec3(125.0f, 80.0f, 25.0f);
	camera[0].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[0] = glm::lookAt(camera[0].prp, camera[0].vrp, camera[0].vup);
	camera[0].vup = glm::vec3(ViewMatrix[0][0].y, ViewMatrix[0][1].y, ViewMatrix[0][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 0", ViewMatrix[0]);
#endif

	camera[0].fov_y = 15.0f;
	camera[0].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[0].near_clip = 0.1f;
	camera[0].far_clip = 2000.0f;
	camera[0].zoom_factor = 1.0f; // will be used for zoomming in and out.


	// initialize the 1th camera.
	camera[1].prp = glm::vec3(120.0f, 90.0f, 100.0f);
	camera[1].vrp = glm::vec3(120.0f, 90.0f, 0.0f);
	camera[1].vup = glm::vec3(0.0f, -10.0f, 0.0f);
	ViewMatrix[1] = glm::lookAt(camera[1].prp, camera[1].vrp, camera[1].vup);
	camera[1].vup = glm::vec3(ViewMatrix[1][0].y, ViewMatrix[1][1].y, ViewMatrix[1][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 1", ViewMatrix[1]);
#endif

	camera[1].fov_y = 10.0f;
	camera[1].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[1].near_clip = 1.0f;
	camera[1].far_clip = 1000.0f;
	camera[1].zoom_factor = 1.0f; // will be used for zoomming in and out.



	// initialize the 2th camera.
	camera[2].prp = glm::vec3(200.0f, 90.0f, 25.0f);
	camera[2].vrp = glm::vec3(0.0f, 90.0f, 25.0f);
	camera[2].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[2] = glm::lookAt(camera[2].prp, camera[2].vrp, camera[2].vup);
	camera[2].vup = glm::vec3(ViewMatrix[2][0].y, ViewMatrix[2][1].y, ViewMatrix[2][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 2", ViewMatrix[2]);
#endif

	camera[2].fov_y = 10.0f;
	camera[2].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[2].near_clip = 1.0f;
	camera[2].far_clip = 1500.0f;
	camera[2].zoom_factor = 1.0f; // will be used for zoomming in and out.



	// initialize the 3th camera.
	camera[3].prp = glm::vec3(120.0f, 100.0f, 25.0f);
	camera[3].vrp = glm::vec3(120.0f, 0.0f, 25.0f);
	camera[3].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[3] = glm::lookAt(camera[3].prp, camera[3].vrp, camera[3].vup);
	camera[3].vup = glm::vec3(ViewMatrix[3][0].y, ViewMatrix[3][1].y, ViewMatrix[3][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 3", ViewMatrix[3]);
#endif

	camera[3].fov_y = 18.0f;
	camera[3].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[3].near_clip = 1.0f;
	camera[3].far_clip = 1000.0f;
	camera[3].zoom_factor = 1.0f; // will be used for zoomming in and out.

		// initialize the 4th camera.
	camera[4].prp = glm::vec3(210.0f, 32.5f, 49.0f);
	camera[4].vrp = glm::vec3(150.0f, 120.0f, 0.0f);
	camera[4].vup = glm::vec3(-1.0f, 1.8f, 2.5f);
	ViewMatrix[4] = glm::lookAt(camera[4].prp, camera[4].vrp, camera[4].vup);
	camera[4].vup = glm::vec3(ViewMatrix[4][0].y, ViewMatrix[4][1].y, ViewMatrix[4][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 4", ViewMatrix[4]);
#endif

	camera[4].fov_y = 70.0f;
	camera[4].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[4].near_clip = 1.0f;
	camera[4].far_clip = 1000.0f;
	camera[4].zoom_factor = 1.0f; // will be used for zoomming in and out.



	// initialize the 5th camera.
	camera[5].prp = glm::vec3(80.0f, 20.0f, 49.0f);
	camera[5].vrp = glm::vec3(70.0f, 70.0f, 0.0f);
	camera[5].vup = glm::vec3(-0.2f, 1.0f, 1.0f);
	ViewMatrix[5] = glm::lookAt(camera[5].prp, camera[5].vrp, camera[5].vup);
	camera[5].vup = glm::vec3(ViewMatrix[5][0].y, ViewMatrix[5][1].y, ViewMatrix[5][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 5", ViewMatrix[5]);
#endif

	camera[5].fov_y = 80.0f;
	camera[5].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[5].near_clip = 1.0f;
	camera[5].far_clip = 1000.0f;
	camera[5].zoom_factor = 1.0f; // will be used for zoomming in and out.


	// initialize the 6th camera.
	camera[6].prp = glm::vec3(20.0f, 90.0f, 49.0f);
	camera[6].vrp = glm::vec3(80.0f, 30.0f, 0.0f);
	camera[6].vup = glm::vec3(-1.0f, 1.0f, 1.0f);
	ViewMatrix[6] = glm::lookAt(camera[6].prp, camera[6].vrp, camera[6].vup);
	camera[6].vup = glm::vec3(ViewMatrix[6][0].y, ViewMatrix[6][1].y, ViewMatrix[6][2].y); // in this example code, make vup always equal to the v direction.

#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 6", ViewMatrix[6]);
#endif

	camera[6].fov_y = 70.0f;
	camera[6].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[6].near_clip = 10.0f;
	camera[6].far_clip = 150.0f;
	camera[6].zoom_factor = 1.0f; // will be used for zoomming in and out.



	// initialize the 7th camera.
	camera[7].prp = glm::vec3(100.0f, 90.0f, 50.0f);
	camera[7].vrp = glm::vec3(0.0f, 90.0f, 0.0f);
	camera[7].vup = glm::vec3(0.0f, 0.0f, 1.0f);
	ViewMatrix[7] = glm::lookAt(camera[7].prp, camera[7].vrp, camera[7].vup);
	camera[7].vup = glm::vec3(ViewMatrix[7][0].y, ViewMatrix[7][1].y, ViewMatrix[7][2].y); // in this example code, make vup always equal to the v direction.


#ifdef PRINT_DEBUG_INFO 
	print_mat4("Cam 7", ViewMatrix[7]);
#endif

	camera[7].fov_y = 100.0f;
	camera[7].aspect_ratio = 1.0f; // will be set when the viewing window popped up.
	camera[7].near_clip = 1.0f;
	camera[7].far_clip = 200.0f;
	camera[7].zoom_factor = 1.0f; // will be used for zoomming in and out.

	initialize_lights_and_material();
}

void set_up_scene_lights(void) {
	// light 0
	light[0].light_on = 1;
	light[0].position[0] = 200.0f; light[0].position[1] = 100.0f; 	// spot light position in WC
	light[0].position[2] = 100.0f; light[0].position[3] = 1.0f;

	light[0].ambient_color[0] = 0.3f; light[0].ambient_color[1] = 0.3f;
	light[0].ambient_color[2] = 0.3f; light[0].ambient_color[3] = 1.0f;

	light[0].diffuse_color[0] = 0.8f; light[0].diffuse_color[1] = 0.35f;
	light[0].diffuse_color[2] = 0.35f; light[0].diffuse_color[3] = 1.0f;

	light[0].specular_color[0] = 0.9f; light[0].specular_color[1] = 0.9f;
	light[0].specular_color[2] = 0.9f; light[0].specular_color[3] = 1.0f;

	light[0].spot_direction[0] = 0.0f; light[0].spot_direction[1] = 0.0f; // spot light direction in WC
	light[0].spot_direction[2] = -1.0f;
	light[0].spot_cutoff_angle = 60.0f;
	light[0].spot_exponent = 27.0f;

	// use light 1
	light[1].light_on = 1;
	light[1].position[0] = 80.0f; light[1].position[1] = 47.5f; // spot light position in WC
	light[1].position[2] = 50.0f; light[1].position[3] = 1.0f;

	light[1].ambient_color[0] = 0.2f; light[1].ambient_color[1] = 0.2f;
	light[1].ambient_color[2] = 0.2f; light[1].ambient_color[3] = 1.0f;

	light[1].diffuse_color[0] = 0.82f; light[1].diffuse_color[1] = 0.82f;
	light[1].diffuse_color[2] = 0.82f; light[1].diffuse_color[3] = 1.0f;

	light[1].specular_color[0] = 0.82f; light[1].specular_color[1] = 0.82f;
	light[1].specular_color[2] = 0.82f; light[1].specular_color[3] = 1.0f;

	light[1].spot_direction[0] = 0.0f; light[1].spot_direction[1] = 0.0f; // spot light direction in WC
	light[1].spot_direction[2] = -1.0f;
	light[1].spot_cutoff_angle = 60.0f;
	light[1].spot_exponent = 27.0f;


	// use light 2
	light[2].light_on = 1;
	light[2].position[0] = 100.0f; light[2].position[1] = 90.0f; // point light position in EC
	light[2].position[2] = 50.0f; light[2].position[3] = 1.0f;

	light[2].ambient_color[0] = 0.2f; light[2].ambient_color[1] = 0.2f;
	light[2].ambient_color[2] = 0.2f; light[2].ambient_color[3] = 1.0f;

	light[2].diffuse_color[0] = 0.82f; light[2].diffuse_color[1] = 0.82f;
	light[2].diffuse_color[2] = 0.82f; light[2].diffuse_color[3] = 1.0f;

	light[2].specular_color[0] = 0.82f; light[2].specular_color[1] = 0.82f;
	light[2].specular_color[2] = 0.82f; light[2].specular_color[3] = 1.0f;

}

void initailize_effect(void) {
	flag_draw_screen = flag_screen_effect = flag_n_screen_effect = 0;
	screen_width = 0.125f;
	blind_width = 90.0f;
	screen_num = 0.0f;
	flag_blind_effect = 0;
	flag_n_blind_effect = 0;
	n_blind_width = 2.0f;
	flag_cartoon_effect = 0;
	cartoon_levels = 3.0f;
}

void prepare_scene(void) {
	char aaa[3][45] = { "Data/car_body_triangles_v.txt" , "Data/car_wheel_triangles_v.txt" ,"Data/car_nut_triangles_v.txt" };
	initailize_effect();
	define_axes();
	define_vol();
	define_static_objects();
	prepare_screen();
	define_animated_tiger();

	prepare_geom_obj(GEOM_OBJ_ID_CAR_BODY, aaa[0], GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_WHEEL, aaa[1], GEOM_OBJ_TYPE_V);
	prepare_geom_obj(GEOM_OBJ_ID_CAR_NUT, aaa[2], GEOM_OBJ_TYPE_V);
	set_up_scene_lights();
}

void initialize_renderer(void) {
	register_callbacks();
	prepare_shader_program();
	initialize_OpenGL();
	prepare_scene();
}

void initialize_glew(void) {
	GLenum error;

	glewExperimental = GL_TRUE;

	error = glewInit();
	if (error != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(error));
		exit(-1);
	}
	fprintf(stdout, "*********************************************************\n");
	fprintf(stdout, " - GLEW version supported: %s\n", glewGetString(GLEW_VERSION));
	fprintf(stdout, " - OpenGL renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(stdout, " - OpenGL version supported: %s\n", glGetString(GL_VERSION));
	fprintf(stdout, "*********************************************************\n\n");
}

void print_message(const char * m) {
	fprintf(stdout, "%s\n\n", m);
}

void greetings(char *program_name, char messages[][256], int n_message_lines) {
	fprintf(stdout, "**************************************************************\n\n");
	fprintf(stdout, "  PROGRAM NAME: %s\n\n", program_name);
	fprintf(stdout, "    This program was coded for CSE4170 students\n");
	fprintf(stdout, "      of Dept. of Comp. Sci. & Eng., Sogang University.\n\n");

	for (int i = 0; i < n_message_lines; i++)
		fprintf(stdout, "%s\n", messages[i]);
	fprintf(stdout, "\n**************************************************************\n\n");

	initialize_glew();
}

#define N_MESSAGE_LINES 1
void main(int argc, char *argv[]) {
	char program_name[256] = "Sogang CSE4170 Our_House_GLSL_V_0.5";
	char messages[N_MESSAGE_LINES][256] = { "    - Keys used: detail in readme file = (HW4_S141602.hwp)" };
	char fname[30] = "Data/path.txt";
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutInitWindowSize(1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);


	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();


	path_n_vertices = read_path_file(fname);
	prepare_path();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutMainLoop();
}


