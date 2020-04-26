#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

volatile int pixel_buffer_start; // global variable
volatile int * key_address;
int x_ship[57] = {160, 160, 160, 160, 160, 161, 161, 161, 161, 161, 159, 159, 159, 159, 159, 162, 162, 162, 162, 162, 158, 158, 158, 158, 158, 163, 163, 163, 163, 163, 157, 157, 157, 157, 157, 164, 164, 164, 164, 164, 156, 156, 156, 156, 156, 165, 165, 165, 155, 155, 155, 166, 166, 154, 154, 167, 153};
int y_ship[57] = {200, 199, 198, 197, 196, 201, 200, 199, 198, 197, 201, 200, 199, 198, 197, 202, 201, 200, 199, 198, 202, 201, 200, 199, 198, 203, 202, 201, 200, 199, 203, 202, 201, 200, 199, 204, 203, 202, 201, 200, 204, 203, 202, 201, 200, 202, 203, 204, 202, 203, 204, 203, 204, 203, 204, 204, 204};
void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void draw_box(int x, int y, short int color);
void draw_ship(short int color);
void rotate(int x_pivot, int y_pivot, double radian);
void wait_for_vsync();
void wait_for_key_release();

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	key_address = (int *)0xFF200050;
	short int white = 0xFFFF;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	int degree = 180;
	double radian  = (double)degree*3.141592653589/180;
	rotate(160, 200, radian);

    while (1)
    {
      /* Erase any boxes and lines that were drawn in the last iteration */

		clear_screen();
		
		if(*key_address == 1){
			rotate(160, 200, radian);
			wait_for_key_release();
		}
		

		draw_ship(white);
		// code for updating the locations of boxes (not shown)
		
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

void clear_screen() 
{
	for (int i = 0; i < 320; ++i){
		for (int j = 0; j < 240; ++j){
			plot_pixel(i, j, 0x0000);
		}
	}
}	

void draw_line(int x0, int y0, int x1, int y1, short int line_color)
{
	bool is_steep = abs(y1 - y0) > abs(x1 - x0);
	int temp;
	if (is_steep){
		temp = x0;
		x0 = y0;
		y0 = temp;
		temp = x1;
		x1 = y1;
		y1 = temp;
	}
	if (x0 > x1){
		temp = x0;
		x0 = x1;
		x1 = temp;
		temp = y0;
		y0 = y1;
		y1 = temp;
	}
	
	int dx = x1 - x0;
	int dy = abs(y1 - y0);
	int error = -(dx/2);
	int y_step;
	if (y0 < y1){
		y_step = 1;
	}
	else {
		y_step = -1;
	}
	
	int y = y0;
	int x = x0;
	
	for (x; x < x1+1; ++x){
		if (is_steep){
			plot_pixel(y, x, line_color);
		}
		else {
			plot_pixel(x, y, line_color);
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
	}
}

void draw_box(int x, int y, short int color){  //draws a 3x3 box at an x,y position
	for (int x0 = x; x0 < x+3; ++x0){
		for (int y0 = y; y0 < y+3; ++y0){
			plot_pixel(x0, y0, color);
		}
	}
}

void draw_ship(short int color){
	for (int i = 0; i < 57; ++i){
		plot_pixel(x_ship[i], y_ship[i], color);
	}
}

void rotate(int x_pivot, int y_pivot, double radian){
	double t,v;
	double n =  sin(radian);
	double m =n*n;// cos(radian);
	m = 1 - m;
	m = sqrt(m);
	int i;
	for(i=0;i<57;i++){
		t = x_ship[i] - x_pivot;
		v = y_ship[i] - y_pivot;
		x_ship[i] = x_pivot + (t*m-v*n);
		y_ship[i] = y_pivot + (v*m+t*n);
	}
}

void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int*)0xFF203020;
	register int status;
	
	*pixel_ctrl_ptr = 1;
	
	status = *(pixel_ctrl_ptr + 3);
	while ((status & 0x01) != 0){
		status = *(pixel_ctrl_ptr + 3);
	}
}

void wait_for_key_release(){
	while(1){
		if (*key_address == 0){
			return;
		}
	}
}	