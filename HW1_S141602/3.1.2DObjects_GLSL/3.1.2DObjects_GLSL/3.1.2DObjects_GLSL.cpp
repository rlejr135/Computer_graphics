#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Shaders/LoadShaders.h"
GLuint h_ShaderProgram; // handle to shader program
GLint loc_ModelViewProjectionMatrix, loc_primitive_color; // indices of uniform variables

// include glm/*.hpp only if necessary
//#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, ortho, etc.
glm::mat4 ModelViewProjectionMatrix;
glm::mat4 ViewMatrix, ProjectionMatrix, ViewProjectionMatrix;

#define TO_RADIAN 0.01745329252f  
#define TO_DEGREE 57.295779513f
#define BUFFER_OFFSET(offset) ((GLvoid *) (offset))

#define LOC_VERTEX 0

int win_width = 0, win_height = 0; 
float centerx = 0.0f, centery = 0.0f, rotate_angle = 0.0f;

GLfloat axes[4][2];
GLfloat axes_color[3] = { 0.0f, 0.0f, 0.0f };
GLuint VBO_axes, VAO_axes;

void prepare_axes(void) { // Draw axes in their MC.
	axes[0][0] = -win_width / 2.5f; axes[0][1] = 0.0f;
	axes[1][0] = win_width / 2.5f; axes[1][1] = 0.0f;
	axes[2][0] = 0.0f; axes[2][1] = -win_height / 2.5f;
	axes[3][0] = 0.0f; axes[3][1] = win_height / 2.5f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_axes);
	glBindVertexArray(VAO_axes);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_axes(void) {
	axes[0][0] = -win_width / 2.25f; axes[1][0] = win_width / 2.25f; 
	axes[2][1] = -win_height / 2.25f;
	axes[3][1] = win_height / 2.25f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axes), axes, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_axes(void) {
	glUniform3fv(loc_primitive_color, 1, axes_color);
	glBindVertexArray(VAO_axes);
	glDrawArrays(GL_LINES, 0, 4);
	glBindVertexArray(0);
}

void timer(int value) {
	glutPostRedisplay();      //윈도우를 다시 그리도록 요청
	glutTimerFunc(30, timer, 0); //다음 타이머 이벤트는 30밀리세컨트 후  호출됨.
}



GLfloat line[2][2];
GLfloat line_color[3] = { 1.0f, 0.0f, 0.0f };
GLuint VBO_line, VAO_line;

void prepare_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_line);
	glBindVertexArray(VAO_line);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void update_line(void) { 	// y = x - win_height/4
	line[0][0] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height; 
	line[0][1] = (1.0f / 4.0f - 1.0f / 2.5f)*win_height - win_height / 4.0f;
	line[1][0] = win_width / 2.5f; 
	line[1][1] = win_width / 2.5f - win_height / 4.0f;

	glBindBuffer(GL_ARRAY_BUFFER, VBO_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(line), line, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void draw_line(void) { // Draw line in its MC.
	// y = x - win_height/4
	glUniform3fv(loc_primitive_color, 1, line_color);
	glBindVertexArray(VAO_line);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}

//shirt
#define SHIRT_LEFT_BODY 0
#define SHIRT_RIGHT_BODY 1
#define SHIRT_LEFT_COLLAR 2
#define SHIRT_RIGHT_COLLAR 3
#define SHIRT_FRONT_POCKET 4
#define SHIRT_BUTTON1 5
#define SHIRT_BUTTON2 6
#define SHIRT_BUTTON3 7
#define SHIRT_BUTTON4 8
GLfloat left_body[6][2] = { { 0.0, -9.0 }, { -8.0, -9.0 }, { -11.0, 8.0 }, { -6.0, 10.0 }, { -3.0, 7.0 }, { 0.0, 9.0 } };
GLfloat right_body[6][2] = { { 0.0, -9.0 }, { 0.0, 9.0 }, { 3.0, 7.0 }, { 6.0, 10.0 }, { 11.0, 8.0 }, { 8.0, -9.0 } };
GLfloat left_collar[4][2] = { { 0.0, 9.0 }, { -3.0, 7.0 }, { -6.0, 10.0 }, { -4.0, 11.0 } };
GLfloat right_collar[4][2] = { { 0.0, 9.0 }, { 4.0, 11.0 }, { 6.0, 10.0 }, { 3.0, 7.0 } };
GLfloat front_pocket[6][2] = { { 5.0, 0.0 }, { 4.0, 1.0 }, { 4.0, 3.0 }, { 7.0, 3.0 }, { 7.0, 1.0 }, { 6.0, 0.0 } };
GLfloat button1[3][2] = { { -1.0, 6.0 }, { 1.0, 6.0 }, { 0.0, 5.0 } };
GLfloat button2[3][2] = { { -1.0, 3.0 }, { 1.0, 3.0 }, { 0.0, 2.0 } };
GLfloat button3[3][2] = { { -1.0, 0.0 }, { 1.0, 0.0 }, { 0.0, -1.0 } };
GLfloat button4[3][2] = { { -1.0, -3.0 }, { 1.0, -3.0 }, { 0.0, -4.0 } };

GLfloat shirt_color[9][3] = {
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 255 / 255.0f, 255 / 255.0f, 255 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f },
	{ 206 / 255.0f, 173 / 255.0f, 184 / 255.0f }
};

GLuint VBO_shirt, VAO_shirt;
void prepare_shirt() {
	GLsizeiptr buffer_size = sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3)+sizeof(button4);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(left_body), left_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body), sizeof(right_body), right_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body), sizeof(left_collar), left_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar), sizeof(right_collar), right_collar);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar),
		sizeof(front_pocket), front_pocket);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket), sizeof(button1), button1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1), sizeof(button2), button2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2), sizeof(button3), button3);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(left_body)+sizeof(right_body)+sizeof(left_collar)+sizeof(right_collar)
		+sizeof(front_pocket)+sizeof(button1)+sizeof(button2)+sizeof(button3), sizeof(button4), button4);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_shirt);
	glBindVertexArray(VAO_shirt);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_shirt);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_shirt() {
	glBindVertexArray(VAO_shirt);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 6, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_LEFT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_RIGHT_COLLAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_FRONT_POCKET]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 6);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON1]);
	glDrawArrays(GL_TRIANGLE_FAN, 26, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON2]);
	glDrawArrays(GL_TRIANGLE_FAN, 29, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON3]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, shirt_color[SHIRT_BUTTON4]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);
	glBindVertexArray(0);
}

//house
#define HOUSE_ROOF 0
#define HOUSE_BODY 1
#define HOUSE_CHIMNEY 2
#define HOUSE_DOOR 3
#define HOUSE_WINDOW 4

GLfloat roof[3][2] = { { -12.0, 0.0 }, { 0.0, 12.0 }, { 12.0, 0.0 } };
GLfloat house_body[4][2] = { { -12.0, -14.0 }, { -12.0, 0.0 }, { 12.0, 0.0 }, { 12.0, -14.0 } };
GLfloat chimney[4][2] = { { 6.0, 6.0 }, { 6.0, 14.0 }, { 10.0, 14.0 }, { 10.0, 2.0 } };
GLfloat door[4][2] = { { -8.0, -14.0 }, { -8.0, -8.0 }, { -4.0, -8.0 }, { -4.0, -14.0 } };
GLfloat window[4][2] = { { 4.0, -6.0 }, { 4.0, -2.0 }, { 8.0, -2.0 }, { 8.0, -6.0 } };

GLfloat house_color[5][3] = {
	{ 200 / 255.0f, 39 / 255.0f, 42 / 255.0f },
	{ 235 / 255.0f, 225 / 255.0f, 196 / 255.0f },
	{ 255 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 233 / 255.0f, 113 / 255.0f, 23 / 255.0f },
	{ 44 / 255.0f, 180 / 255.0f, 49 / 255.0f }
};

GLuint VBO_house, VAO_house;
void prepare_house() {
	GLsizeiptr buffer_size = sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door)
		+sizeof(window);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(roof), roof);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof), sizeof(house_body), house_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body), sizeof(chimney), chimney);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney), sizeof(door), door);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(roof)+sizeof(house_body)+sizeof(chimney)+sizeof(door),
		sizeof(window), window);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_house);
	glBindVertexArray(VAO_house);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_house);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_house() {
	glBindVertexArray(VAO_house);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_ROOF]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 3);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 3, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_CHIMNEY]);
	glDrawArrays(GL_TRIANGLE_FAN, 7, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_DOOR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 4);

	glUniform3fv(loc_primitive_color, 1, house_color[HOUSE_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 15, 4);

	glBindVertexArray(0);
}

//car
#define CAR_BODY 0
#define CAR_FRAME 1
#define CAR_WINDOW 2
#define CAR_LEFT_LIGHT 3
#define CAR_RIGHT_LIGHT 4
#define CAR_LEFT_WHEEL 5
#define CAR_RIGHT_WHEEL 6

GLfloat car_body[4][2] = { { -16.0, -8.0 }, { -16.0, 0.0 }, { 16.0, 0.0 }, { 16.0, -8.0 } };
GLfloat car_frame[4][2] = { { -10.0, 0.0 }, { -10.0, 10.0 }, { 10.0, 10.0 }, { 10.0, 0.0 } };
GLfloat car_window[4][2] = { { -8.0, 0.0 }, { -8.0, 8.0 }, { 8.0, 8.0 }, { 8.0, 0.0 } };
GLfloat car_left_light[4][2] = { { -9.0, -6.0 }, { -10.0, -5.0 }, { -9.0, -4.0 }, { -8.0, -5.0 } };
GLfloat car_right_light[4][2] = { { 9.0, -6.0 }, { 8.0, -5.0 }, { 9.0, -4.0 }, { 10.0, -5.0 } };
GLfloat car_left_wheel[4][2] = { { -10.0, -12.0 }, { -10.0, -8.0 }, { -6.0, -8.0 }, { -6.0, -12.0 } };
GLfloat car_right_wheel[4][2] = { { 6.0, -12.0 }, { 6.0, -8.0 }, { 10.0, -8.0 }, { 10.0, -12.0 } };

GLfloat car_color[7][3] = {
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 0 / 255.0f, 149 / 255.0f, 159 / 255.0f },
	{ 216 / 255.0f, 208 / 255.0f, 174 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f },
	{ 21 / 255.0f, 30 / 255.0f, 26 / 255.0f }
};

GLuint VBO_car, VAO_car;
void prepare_car() {
	GLsizeiptr buffer_size = sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel)+sizeof(car_right_wheel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car_body), car_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body), sizeof(car_frame), car_frame);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame), sizeof(car_window), car_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window), sizeof(car_left_light), car_left_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light),
		sizeof(car_right_light), car_right_light);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light), sizeof(car_left_wheel), car_left_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car_body)+sizeof(car_frame)+sizeof(car_window)+sizeof(car_left_light)
		+sizeof(car_right_light)+sizeof(car_left_wheel), sizeof(car_right_wheel), car_right_wheel);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car);
	glBindVertexArray(VAO_car);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car() {
	glBindVertexArray(VAO_car);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_FRAME]);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_LIGHT]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_LEFT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glUniform3fv(loc_primitive_color, 1, car_color[CAR_RIGHT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);

	glBindVertexArray(0);
}


#define CAT_HEAD 0
#define CAT_LEFTEAR 1
#define CAT_RIGHTEAR 2
#define CAT_BODY 3
#define CAT_FRONT_LEFT_LEG 4
#define CAT_FRONT_RIGHT_LEG 5
#define CAT_BACK_LEFT_LEG 6
#define CAT_BACK_RIGHT_LEG 7
#define CAT_TAIL 8
#define CAT_LEYE 9
#define CAT_REYE 10
#define CAT_MOUSE 11
#define CAT_linear 12
#define CAT_rinear 13
#define CAT_nose 14
#define CAT_su 15

GLfloat cathead[8][2] = { { -10.0, -0.0 }, { -5.0, 0.0 }, { -3.0, 1.5 }, { -3.0, 6.0 },  { -4.0, 7.0 }, { -11.0, 7.0 }, { -12.0, 6.0 }, { -12.0, 1.5 } };
GLfloat leftear[3][2] = { { -11.0, 7.0 }, { -8.0, 7.0 }, { -9.5, 11.6 } };
GLfloat rightear[3][2] = { { -7.0, 7.0 }, { -4.0, 7.0 }, { -5.5, 11.6 } };
GLfloat catbody[4][2] = { { -8.0, 0 }, { 14.0, 0.0 }, { 14.0, -7.0 }, { -8.0, -7.0 } };
GLfloat front_left_leg[6][2] = { { -8.0, -11.0 }, { -7.0, -7.0 }, { -6.0, -7.0 }, { -7.0, -11.0 }, { -6.7, -13.9 }, { -7.7, -14.2 } };
GLfloat front_right_leg[4][2] = { { -5.0, -7.0 },{ -4.0, -7.0 },{ -4.0, -15.0 },{ -5.0, -15.0 } };
GLfloat back_left_leg[6][2] = { { 9.0, -11.0 }, { 10.0, -7.0 },{ 11.0, -7.0 }, { 10.0, -11.0 }, { 10.3, -13.9 },{ 9.3, -14.2 } };
GLfloat back_right_leg[4][2] = { { 12.0, -7.0 },{ 13.0, -7.0 },{ 13.0, -15.0 },{ 12.0, -15.0 } };
GLfloat cattail[7][2] = { { 14.0, 0.0},{ 16.0, 6.5 }, { 17.0, 3.0 },{ 21.0, 5.0 }, { 21.0, 1.0 },{ 25.0, 11.0 }, { 27.0, 8.0 }};
GLfloat catleye[1][2] = { { -9.3, 4.5 } };
GLfloat catreye[1][2] = { { -5.7, 4.5 } };
GLfloat catmouse[3][2] = { { -7.5, 0.5 }, { -8.5, 1.4 }, { -6.5, 1.4 } };
GLfloat leftinear[3][2] = { { -10.5, 7.0 },{ -8.5, 7.0 },{ -9.5, 9.8 } };
GLfloat rightinear[3][2] = { { -6.5, 7.0 },{ -4.5, 7.0 },{ -5.5, 9.8 } };
GLfloat catnose[1][2] = { { -7.5, 2.5 } };
GLfloat catbr[6][2] = { {-11.5, 2.5}, {-3.5, 2.5}, {-11.0, 3.5}, {-3.0, 1.5}, {-11.0, 1.5}, {-3.0, 3.5} };


GLfloat cat_color[2][16][3]{

	{	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0xA4 / 255.0f, 0x81 / 255.0f, 0x4F / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xFF / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xD7 / 255.0f, 0xAC / 255.0f, 0x87 / 255.0f },
	{ 0xD7 / 255.0f, 0xAC / 255.0f, 0x87 / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xC9 / 255.0f, 0xB1 / 255.0f, 0x70 / 255.0f }},

	{ { 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x87 / 255.0f, 0x26 / 255.0f, 0x00 / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xFF / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xD7 / 255.0f, 0xAC / 255.0f, 0x87 / 255.0f },
	{ 0xD7 / 255.0f, 0xAC / 255.0f, 0x87 / 255.0f },
	{ 0x00 / 255.0f, 0x00 / 255.0f, 0x00 / 255.0f },
	{ 0xC9 / 255.0f, 0xB1 / 255.0f, 0x70 / 255.0f }}
};

int catcolornum = 0;
GLuint VBO_cat, VAO_cat;
void prepare_cat() {
	GLsizeiptr buffer_size = sizeof(cathead) + sizeof(leftear) + sizeof(rightear) +sizeof(catbody) + sizeof(front_left_leg) + sizeof(front_right_leg)
		+ sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye) + sizeof(catreye) + sizeof(catmouse) + sizeof(leftinear)
		+ sizeof(rightinear) + sizeof(catnose) + sizeof(catbr);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_cat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cat);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cathead), cathead);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead), sizeof(leftear), leftear);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear), sizeof(rightear), rightear);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear), sizeof(catbody), catbody);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody), sizeof(front_left_leg), front_left_leg);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg), 
		sizeof(front_right_leg), front_right_leg);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg), sizeof(back_left_leg), back_left_leg);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg), sizeof(back_right_leg), back_right_leg);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg), sizeof(cattail), cattail);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail), sizeof(catleye), catleye);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye), sizeof(catreye),catreye);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye)+ sizeof(catreye), sizeof(catmouse), catmouse);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye) + sizeof(catreye)
		+ sizeof(catmouse), sizeof(leftinear), leftinear);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye) + sizeof(catreye)
		+ sizeof(catmouse) + sizeof(leftinear), sizeof(rightinear), rightinear);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye) + sizeof(catreye)
		+ sizeof(catmouse) + sizeof(leftinear) + sizeof(rightinear), sizeof(catbr), catbr);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cathead) + sizeof(leftear) + sizeof(rightear) + sizeof(catbody) + sizeof(front_left_leg) +
		sizeof(front_right_leg) + sizeof(back_left_leg) + sizeof(back_right_leg) + sizeof(cattail) + sizeof(catleye) + sizeof(catreye)
		+ sizeof(catmouse) + sizeof(leftinear) + sizeof(rightinear) + sizeof(catbr), sizeof(catnose), catnose);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_cat);
	glBindVertexArray(VAO_cat);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_cat);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}
void draw_cat() {
	glBindVertexArray(VAO_cat);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_LEFTEAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 3);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_RIGHTEAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 11, 3);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 14, 4);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_FRONT_LEFT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 18, 6);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_FRONT_RIGHT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 4);
	
	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_BACK_LEFT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 28, 6);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_BACK_RIGHT_LEG]);
	glDrawArrays(GL_TRIANGLE_FAN, 34, 4);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_TAIL]);
	glDrawArrays(GL_TRIANGLE_STRIP, 38, 7);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_LEYE]);
	glPointSize(3.5);
	glDrawArrays(GL_POINTS, 45, 1);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_REYE]);
	glDrawArrays(GL_POINTS, 46, 1);
	glPointSize(1.0);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_MOUSE]);
	glDrawArrays(GL_TRIANGLE_STRIP, 47, 3);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_linear]);
	glDrawArrays(GL_TRIANGLE_STRIP, 50, 3);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_rinear]);
	glDrawArrays(GL_TRIANGLE_STRIP, 53, 3);


	glLineWidth(1.5);
	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_su]);
	glDrawArrays(GL_LINES, 56, 6);
	glLineWidth(1.0);

	glUniform3fv(loc_primitive_color, 1, cat_color[catcolornum][CAT_nose]);
	glPointSize(2.0);
	glDrawArrays(GL_POINTS, 62, 1);
	glPointSize(1.0);


	glBindVertexArray(0);
}


#define CATFOOD_BODY 0
#define CATFOOD_HEAD 1
#define CATFOOD_LABEL 2

GLfloat canbody[8][2] = { { 0.0 , 0.0 }, { -2.0, 0.0 }, { -3.5, 1.0 }, { -3.5, 3.0 }, { -2.0, 2.0 }, { 0.0, 2.0 }, { 1.5, 3.0 }, { 1.5, 1.0 } };
GLfloat canhead[6][2] = { { -3.5, 3.0 }, { -2.0, 2.0 }, { 0.0, 2.0 }, { 1.5, 3.0 }, { 0.0, 4.0 }, { -2.0, 4.0 } };
GLfloat canlabel[4][2] = { { 0.0, 0.8 }, { 0.0, 1.2 }, { -2.0, 1.2 }, { -2.0, 0.8 } };

GLfloat catfood_color[3][3]{
	{ 0x70 / 255.0f, 0xB7 / 255.0f, 0xc9 / 255.0f },
	{ 0xc3 / 255.0f, 0xc6 / 255.0f, 0xBA / 255.0f },
	{ 0xCE / 255.0f, 0xCE / 255.0f, 0x3D / 255.0f }
};

GLuint VBO_catfood, VAO_catfood;
void prepare_catfood() {
	GLsizeiptr buffer_size = sizeof(canbody) + sizeof(canhead) + sizeof(canlabel);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_catfood);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_catfood);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(canbody), canbody);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(canbody), sizeof(canhead), canhead);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(canbody) + sizeof(canhead), sizeof(canlabel), canlabel);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_catfood);
	glBindVertexArray(VAO_catfood);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_catfood);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_catfood() {
	glBindVertexArray(VAO_catfood);

	glUniform3fv(loc_primitive_color, 1, catfood_color[CATFOOD_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, catfood_color[CATFOOD_HEAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 6);

	glUniform3fv(loc_primitive_color, 1, catfood_color[CATFOOD_LABEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 14, 4);

	glBindVertexArray(0);
}



#define TREE_LEAFS 0
#define TREE_POLE 1

GLfloat treeleafs[32][2] = { {14.0,-8.0 }, {12.0,-3.0 }, {14.0,-3.0 }, {15.0,-2.0}, {16.0 , -1.0 }, { 17.0 , -1.0 }, {18,-2}, {19,-2}, {20,-3},
{21, -4}, {21, -5}, {22, -5}, {22, -10}, {21, -11}, {20, -12}, {19, -13}, {18, -13}, {17, -14}, {11, -14}, {10, -13}, {9, -13}, {8, -12}, {7, -11},
{7, -9}, {6, -8}, {6, -5}, {7, -4}, {8, -3}, {9, -3}, {10, -2}, {11, -2},{ 12.0,-3.0 } };
GLfloat treepole[14][2] = { {14, -23}, {17, -14}, {12, -14}, {12, -21}, {11, -22}, {10, -23}, {10, -24}, {11, -25}, {18, -25}, {19, -24},
{19, -23}, {18, -22}, {17, -21}, { 17, -14 } };

GLfloat tree_color[2][3]{
	{ 0x4C / 255.0f, 0xAF / 255.0f, 0x50 / 255.0f },
	{ 0x79 / 255.0f, 0x55 / 255.0f, 0x48 / 255.0f }
};

GLuint VBO_tree, VAO_tree;
void prepare_tree() {
	GLsizeiptr buffer_size = sizeof(treeleafs) + sizeof(treepole);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_tree);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_tree);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(treeleafs), treeleafs);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(treeleafs), sizeof(treepole), treepole);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_tree);
	glBindVertexArray(VAO_tree);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_tree);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_tree() {
	glBindVertexArray(VAO_tree);

	glUniform3fv(loc_primitive_color, 1, tree_color[TREE_LEAFS]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 32);

	glUniform3fv(loc_primitive_color, 1, tree_color[TREE_POLE]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 14);

	glBindVertexArray(0);
}




#define ROAD 0

GLfloat road[4][2] = { { 600.0 , -80.0 },{ -600.0, -80.0 },{ -600.0, -320.0 },{ 600.0, -320.0 } };

GLfloat road_color[3][3]{
	{ 0x2F / 255.0f, 0x2C / 255.0f, 0x2A / 255.0f }
};


GLuint VBO_road, VAO_road;
void prepare_road() {
	GLsizeiptr buffer_size = sizeof(road);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(road), road);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_road);
	glBindVertexArray(VAO_road);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_road);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_road() {
	glBindVertexArray(VAO_road);

	glUniform3fv(loc_primitive_color, 1, road_color[ROAD]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
}


#define RECTANGULAR 0

GLfloat rectan[4][2] = { { 600.0 , -190.0 },{ -600.0, -190.0 },{ -600.0, -210.0 },{ 600.0, -210.0 } };

GLfloat rectan_color[3][3]{
	{ 0xF3 / 255.0f, 0xF7 / 255.0f, 0x18 / 255.0f }
};


GLuint VBO_rectan, VAO_rectan;
void prepare_rectan() {
	GLsizeiptr buffer_size = sizeof(rectan);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_rectan);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_rectan);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rectan), rectan);


	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_rectan);
	glBindVertexArray(VAO_rectan);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_rectan);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_rectan() {
	glBindVertexArray(VAO_rectan);

	glUniform3fv(loc_primitive_color, 1, rectan_color[RECTANGULAR]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glBindVertexArray(0);
}

//draw car2
#define CAR2_BODY 0
#define CAR2_FRONT_WINDOW 1
#define CAR2_BACK_WINDOW 2
#define CAR2_FRONT_WHEEL 3
#define CAR2_BACK_WHEEL 4
#define CAR2_LIGHT1 5
#define CAR2_LIGHT2 6

GLfloat car2_body[8][2] = { { -18.0, -7.0 }, { -18.0, 0.0 }, { -13.0, 0.0 }, { -10.0, 8.0 }, { 10.0, 8.0 }, { 13.0, 0.0 }, { 18.0, 0.0 }, { 18.0, -7.0 } };
GLfloat car2_front_window[4][2] = { { -10.0, 0.0 }, { -8.0, 6.0 }, { -2.0, 6.0 }, { -2.0, 0.0 } };
GLfloat car2_back_window[4][2] = { { 0.0, 0.0 }, { 0.0, 6.0 }, { 8.0, 6.0 }, { 10.0, 0.0 } };
GLfloat car2_front_wheel[8][2] = { { -11.0, -11.0 }, { -13.0, -8.0 }, { -13.0, -7.0 }, { -11.0, -4.0 }, { -7.0, -4.0 }, { -5.0, -7.0 }, { -5.0, -8.0 }, { -7.0, -11.0 } };
GLfloat car2_back_wheel[8][2] = { { 7.0, -11.0 }, { 5.0, -8.0 }, { 5.0, -7.0 }, { 7.0, -4.0 }, { 11.0, -4.0 }, { 13.0, -7.0 }, { 13.0, -8.0 }, { 11.0, -11.0 } };
GLfloat car2_light1[3][2] = { { -18.0, -1.0 }, { -17.0, -2.0 }, { -18.0, -3.0 } };
GLfloat car2_light2[3][2] = { { -18.0, -4.0 }, { -17.0, -5.0 }, { -18.0, -6.0 } };

GLfloat car2_color[7][3] = {
	{ 100 / 255.0f, 141 / 255.0f, 159 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 235 / 255.0f, 219 / 255.0f, 208 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 0 / 255.0f, 0 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f },
	{ 249 / 255.0f, 244 / 255.0f, 0 / 255.0f }
};

GLuint VBO_car2, VAO_car2;
void prepare_car2() {
	GLsizeiptr buffer_size = sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1)+sizeof(car2_light2);

	// Initialize vertex buffer object.
	glGenBuffers(1, &VBO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW); // allocate buffer object memory

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(car2_body), car2_body);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body), sizeof(car2_front_window), car2_front_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window), sizeof(car2_back_window), car2_back_window);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window), sizeof(car2_front_wheel), car2_front_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel),
		sizeof(car2_back_wheel), car2_back_wheel);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel), sizeof(car2_light1), car2_light1);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(car2_body)+sizeof(car2_front_window)+sizeof(car2_back_window)+sizeof(car2_front_wheel)
		+sizeof(car2_back_wheel)+sizeof(car2_light1), sizeof(car2_light2), car2_light2);

	// Initialize vertex array object.
	glGenVertexArrays(1, &VAO_car2);
	glBindVertexArray(VAO_car2);

	glBindBuffer(GL_ARRAY_BUFFER, VBO_car2);
	glVertexAttribPointer(LOC_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void draw_car2() {
	glBindVertexArray(VAO_car2);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BODY]);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WINDOW]);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_FRONT_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_BACK_WHEEL]);
	glDrawArrays(GL_TRIANGLE_FAN, 24, 8);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT1]);
	glDrawArrays(GL_TRIANGLE_FAN, 32, 3);

	glUniform3fv(loc_primitive_color, 1, car2_color[CAR2_LIGHT2]);
	glDrawArrays(GL_TRIANGLE_FAN, 35, 3);

	glBindVertexArray(0);
}

GLfloat movex = 0.0f, howx;
GLfloat movey = 0.0f, howy;
GLint dx, dy;
GLfloat randomnum;
GLfloat gx, gy;
GLfloat car1x = -550.0f, car2x = 550.0f;
GLfloat housescale = 1;
GLint dcar1 = 1, dcar2 = -1;
GLfloat cattoX=0, cattoY=0;
GLfloat catdesX = 0, catdesY = 0;
GLfloat catscale = 0, eatcatfood = 0, recurflag = 0;
GLfloat rotatenum = 0;
GLfloat t = 0;
int meet_flag = 0;
int catchangeflag = 0;
int leftbuttonpressed = 0, maintaindraw = 0;
int car1stopflag = 0, car2stopflag = 0;
int car1ready = 0, car2ready = 0;
int i, houseflag = 0;
float x, r, s, delx, delr, dels;

void display(void) {

	glm::mat4 ModelMatrix;

	glClear(GL_COLOR_BUFFER_BIT);
	

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 20.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_road();

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 20.0f, 0.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_rectan();
	for (int i = 0; i < 4; i++) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f + i * 100, 360.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(10.5f, 13.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_tree();
	}

	if (houseflag == 0) {
		housescale+= 0.25;
	}
	else if (houseflag == 1) {
		housescale-= 0.25;
	}

	if (housescale >= 5) {
		houseflag = 1;
	}
	else if (housescale < 2) {
		houseflag = 0;
	}
	
	rotatenum += 1;
	for (i = 0; i < 7; i++){
		GLfloat tmp = housescale;
		ModelMatrix = glm::rotate(ModelMatrix, (GLfloat)72*i*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(150 * sin((360/7*i + rotatenum)* 3.1415 / 180) - 300.0f, 150 * cos((360/7 * i +rotatenum)*3.1415 / 180) + 150.0f, 0.0f));
	//	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(10*16 * sin(t*TO_RADIAN)*sin(t*TO_RADIAN)*sin(t*TO_RADIAN) - 300.0f,
	//		10*(13*cos(t*TO_RADIAN) - 5*cos(2*t*TO_RADIAN) - 2 * cos(3*t*TO_RADIAN) - 
	//		cos(4*t*TO_RADIAN)) + 150.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(tmp, tmp, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_house();
	}

	if (!meet_flag) {
		if (movex < -285 && movex > -315 && movey > 120 && movey < 185)
			meet_flag = 1;
	}

	if (car1x + 120 > movex && car1x - 120 < movex && -30 > movey && -210 < movey) {
		car1stopflag = 1;
		car1ready = 10;
	}
	else if (car1ready != 0) {
		car1ready--;
		car1stopflag = 0;
	}
	else
		car1ready = 0;

	if (car2x + 120 > movex && car2x - 120 < movex && -150 > movey && -320 < movey) {
		car2stopflag = 1;
		car2ready = 10;
	}
	else if (car2ready != 0) {
		car2ready--;
		car2stopflag = 0;
	}
	else
		car2ready = 0;


	if ((movex > 470.0f || movex < -570.0f || car1stopflag == 1 || car2stopflag == 1) && catchangeflag == 0) {
		if (movex > 0)
			dx = -1;
		else
			dx = 1;
		randomnum = (rand() % 100) / (GLfloat)100;
		howx = randomnum * 10;
		howy = 10.0 - randomnum * 10;
		catchangeflag = 1;
		if (car1stopflag) {
			if (car1x + 110 < movex)
				dx = 1;
			else if (car1x - 110 > movex)
				dx = -1;
		}
		else if (car2stopflag) {
			if (car2x + 110 < movex)
				dx = 1;
			else if (car2x - 110 > movex)
				dx = -1;
		}
	}
	if ((movey < -335.0f || movey > 372.8f || car1stopflag == 1 || car2stopflag == 1) && catchangeflag == 0) {
		if (movey < 0)
			dy = 1;
		else
			dy = -1;
		randomnum = (rand() % 100) / (GLfloat)100;
		howx = randomnum * 10;
		howy = 10.0 - randomnum * 10;
		catchangeflag = 1;
		if (car1stopflag) {
			if (-40 > movey)
				dy = 1;
			else if (-200 < movey)
				dy = -1;
		}
		else if (car2stopflag) {
			if (-160 > movey)
				dy = 1;
			else if (-310 < movey)
				dy = -1;
		}
	}


	if (car1x < -550.0f ||car1x > 550.0f) {
		dcar1 *= -1;
	}

	if (!car1ready)
		car1x += 10 * dcar1;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car1x, -120.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(6.0f, 6.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	draw_car();

	if (car2x < -550.0f || car2x > 550.0f) {
		dcar2 *= -1;
	}

	if (!car2ready)
		car2x += 10 * dcar2;

	ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(car2x, -240.0f, 0.0f));
	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(6.0f, 6.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]); 
	draw_car2();

	if (recurflag) {
		if (eatcatfood != 0)
			eatcatfood -= 0.25;
		else
			recurflag = 0;
	}
	else if (cattoX == -100) {
		//if (delr < 360)
		//	catscale += 0.15;
		//else
		//	catscale -= 0.15;
		delr += 10;
		t += 5;

		if (delr >= 720) {
			cattoX = 0;
			cattoY = 0;
			delr = 0;
			t = 0;
			eatcatfood += 0.5;
			if (eatcatfood >= 2.5)
				catcolornum = 1;
			else
				catcolornum = 0;
			if (eatcatfood > 3) {
				catcolornum = 0;
				recurflag = 1;
			}
		}
	}
	else if (cattoX != 0) {
		movex += cattoX;
		movey += cattoY;

		if (movex  < catdesX + 10 && movex > catdesX - 10) {
			cattoX = -100;
			cattoY = -100;
			maintaindraw = 0;
		}
	}
	else {
		movex += (dx*howx);
		movey += (dy*howy);
		catscale = 0;
	}

	if ((car1stopflag || car2stopflag) && cattoX != 0) {
		movex += (cattoX / 5);
		movey += (cattoY / 5);
		delr += 46;
		if (delr > 360)
			delr -= 360;
	}
	else if (car1stopflag || car2stopflag) {
		delr += 46;
		if (delr > 360)
			delr -= 360;
	}
	else if (delr != 0 && cattoX == 0) {
		delr += 46;
		if (delr > 360)
			delr -= 360;
		if (delr < 30)
			delr = 0;
	}


	if (t != 0)
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(15*16 * sin(t*TO_RADIAN)*sin(t*TO_RADIAN)*sin(t*TO_RADIAN) +movex,
			15*(13*cos(t*TO_RADIAN) - 5*cos(2*t*TO_RADIAN) - 2 * cos(3*t*TO_RADIAN) - 
			cos(4*t*TO_RADIAN)) + movey, 0.0f));
	else
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(movex, movey, 0.0f));

	ModelMatrix = glm::scale(ModelMatrix, glm::vec3(4.5f + catscale + eatcatfood, 4.5f + catscale + eatcatfood, 1.0f));
	ModelMatrix = glm::rotate(ModelMatrix, delr*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
	ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
	glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
	draw_cat();

	if (!meet_flag) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-300.0f, 150.0f, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(2.0f, 5.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_shirt();
	}
	else {
		if (t != 0)
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(15 * 16 * sin(t*TO_RADIAN)*sin(t*TO_RADIAN)*sin(t*TO_RADIAN) + 22.0f + movex,
				15 * (13 * cos(t*TO_RADIAN) - 5 * cos(2 * t*TO_RADIAN) - 2 * cos(3 * t*TO_RADIAN) -
					cos(4 * t*TO_RADIAN)) + movey - 14.0f , 0.0f));
		else
			ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(22.0f + movex, movey - 14.0f , 0.0f));

		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(5.21f + catscale + eatcatfood , 1.7f + catscale + eatcatfood, 1.0f));
		ModelMatrix = glm::rotate(ModelMatrix, (delr + 90)*TO_RADIAN, glm::vec3(0.0f, 0.0f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);
		draw_shirt();
	}

	if (catchangeflag > 0) {
		catchangeflag++;
		if (catchangeflag > 9)
			catchangeflag = 0;
	}


	if (leftbuttonpressed == 1 || maintaindraw == 1) {
		ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(gx, gy, 0.0f));
		ModelMatrix = glm::scale(ModelMatrix, glm::vec3(8.5f, 8.5f, 1.0f));
		ModelViewProjectionMatrix = ViewProjectionMatrix * ModelMatrix;
		glUniformMatrix4fv(loc_ModelViewProjectionMatrix, 1, GL_FALSE, &ModelViewProjectionMatrix[0][0]);

		draw_catfood();
		maintaindraw = 1;

		if (leftbuttonpressed == 1) {
			catdesX = gx;
			catdesY = gy;
			cattoX = (movex - gx) / (abs(movex - gx) + abs(movey - gy)) * -10;
			cattoY = (movey - gy) / (abs(movex - gx) + abs(movey - gy)) * -10;
			if (delr != 0)
				delr = 0;
		}
	}

	glFlush();	
}   



void mouse(int button, int state, int x, int y) {

	gx = (GLfloat)x - (GLfloat)600;
	gy = (GLfloat)400 - (GLfloat)y;
	if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
		leftbuttonpressed = 1;
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP))
		leftbuttonpressed = 0;
}

void motion(int x, int y) {
	static int delay = 0;
	static float tmpx = 0.0, tmpy = 0.0;
	float dx, dy;
	if (leftbuttonpressed) {
		centerx = x - win_width / 2.0f, centery = (win_height - y) - win_height / 2.0f;
		if (delay == 8) {
			dx = centerx - tmpx;
			dy = centery - tmpy;

			if (dx > 0.0) {
				rotate_angle = atan(dy / dx) + 90.0f*TO_RADIAN;
			}
			else if (dx < 0.0) {
				rotate_angle = atan(dy / dx) - 90.0f*TO_RADIAN;
			}
			else if (dx == 0.0) {
				if (dy > 0.0) rotate_angle = 180.0f*TO_RADIAN;
				else  rotate_angle = 0.0f;
			}
			tmpx = centerx, tmpy = centery;
			delay = 0;
		}
		glutPostRedisplay();
		delay++;
	}
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27: // ESC key
		glutLeaveMainLoop(); // Incur destuction callback for cleanups.
		break;
	}
}

void reshape(int width, int height) {
	win_width = width, win_height = height;
	
  	glViewport(0, 0, win_width, win_height);
	ProjectionMatrix = glm::ortho(-win_width / 2.0, win_width / 2.0, 
		-win_height / 2.0, win_height / 2.0, -1000.0, 1000.0);
	ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;

	update_axes();
	update_line();

	glutPostRedisplay();
}

void cleanup(void) {
	glDeleteVertexArrays(1, &VAO_axes);
	glDeleteBuffers(1, &VBO_axes);

	glDeleteVertexArrays(1, &VAO_line);
	glDeleteBuffers(1, &VBO_line);

	glDeleteVertexArrays(1, &VAO_shirt);
	glDeleteBuffers(1, &VBO_shirt);

	glDeleteVertexArrays(1, &VAO_house);
	glDeleteBuffers(1, &VBO_house);

	glDeleteVertexArrays(1, &VAO_cat);
	glDeleteBuffers(1, &VBO_cat);

	glDeleteVertexArrays(1, &VAO_catfood);
	glDeleteBuffers(1, &VBO_catfood);

	glDeleteVertexArrays(1, &VAO_tree);
	glDeleteBuffers(1, &VBO_tree);

	glDeleteVertexArrays(1, &VAO_car);
	glDeleteBuffers(1, &VBO_car);

	glDeleteVertexArrays(1, &VAO_car2);
	glDeleteBuffers(1, &VBO_car2);

	glDeleteVertexArrays(1, &VAO_road);
	glDeleteBuffers(1, &VBO_road);

	glDeleteVertexArrays(1, &VAO_rectan);
	glDeleteBuffers(1, &VBO_rectan);
}

void register_callbacks(void) {
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutReshapeFunc(reshape);
	glutCloseFunc(cleanup);
}

void prepare_shader_program(void) {
	ShaderInfo shader_info[3] = {
		{ GL_VERTEX_SHADER, "Shaders/simple.vert" },
		{ GL_FRAGMENT_SHADER, "Shaders/simple.frag" },
		{ GL_NONE, NULL }
	};

	h_ShaderProgram = LoadShaders(shader_info);
	glUseProgram(h_ShaderProgram);

	loc_ModelViewProjectionMatrix = glGetUniformLocation(h_ShaderProgram, "u_ModelViewProjectionMatrix");
	loc_primitive_color = glGetUniformLocation(h_ShaderProgram, "u_primitive_color");
}

void initialize_OpenGL(void) {
	glEnable(GL_MULTISAMPLE); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glClearColor(0x7C / 255.0f, 0xFC / 255.0f, 0x00 / 255.0f, 1.0f);
	ViewMatrix = glm::mat4(1.0f);
}

void prepare_scene(void) {
	prepare_axes();
	prepare_line();
	prepare_shirt();
	prepare_house();
	prepare_car();
	prepare_car2();
	prepare_cat();
	prepare_catfood();
	prepare_road();
	prepare_rectan();
	prepare_tree();
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
	char program_name[64] = "Sogang CSE4170 2DObjects_GLSL_3.1";
	char messages[N_MESSAGE_LINES][256] = {
		"    - Keys used: 'ESC' "
	};

	glutInit (&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_MULTISAMPLE);
	glutInitWindowSize (1200, 800);
	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(program_name);

	greetings(program_name, messages, N_MESSAGE_LINES);
	initialize_renderer();

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);


	dx = dy = 1.0f;
	srand(time(NULL));
	randomnum = (rand() % 100) / (GLfloat)100;
	howx = randomnum * 10;
	howy = 10.0f - randomnum * 10;
	movex = 0.0f;
	movey = 0.0f;
	glutTimerFunc(0, timer, 0);


	glutMainLoop ();
}


