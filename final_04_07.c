#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define big_x  51
#define big_y  51
#define med_x  26
#define med_y  26
#define small_x  11
#define small_y  11

struct big_asteroids
{
    int x_pos;
    int y_pos;
    int x_step;
    int y_step;
    int asteroid[big_x][big_y];
};

struct med_asteroids
{
    int x_pos;
    int y_pos;
    int x_step;
    int y_step;
    int asteroid[med_x][med_y];
};

struct small_asteroids
{
    int x_pos;
    int y_pos;
    int x_step;
    int y_step;
    int asteroid[small_x][small_y];
};

volatile int pixel_buffer_start; // global variable
volatile int * key_address;
volatile int * ps2_address;

void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void wait_for_vsync1(volatile int* pixel_ctrl_ptr);
void wait_for_key_release();

void draw_big_asteroid(struct big_asteroids* b_a);
void clear_big_asteroid(struct big_asteroids* b_a);
void initialize_big_asteroid_array(struct big_asteroids* b_a);
void draw_big_line(int x0, int y0, int x1, int y1, struct big_asteroids* b_a);
void clean_big_line(int i,int y, struct big_asteroids* b_a);

void draw_med_asteroid(struct med_asteroids* b_a);
void clear_med_asteroid(struct med_asteroids* b_a);
void initialize_med_asteroid_array(struct med_asteroids* b_a);
void draw_med_line(int x0, int y0, int x1, int y1, struct med_asteroids* b_a);
void clean_med_line(int i,int y, struct med_asteroids* b_a);

void draw_small_asteroid(struct small_asteroids* b_a);
void clear_small_asteroid(struct small_asteroids* b_a);
void initialize_small_asteroid_array(struct small_asteroids* b_a);
void draw_small_line(int x0, int y0, int x1, int y1, struct small_asteroids* b_a);
void clean_small_line(int i,int y, struct small_asteroids* b_a);

int x_ship[57] = {160, 160, 160, 160, 160, 161, 161, 161, 161, 161, 159, 159, 159, 159, 159, 162, 162, 162, 162, 162, 158, 158, 158, 158, 158, 163, 163, 163, 163, 163, 157, 157, 157, 157, 157, 164, 164, 164, 164, 164, 156, 156, 156, 156, 156, 165, 165, 165, 155, 155, 155, 166, 166, 154, 154, 167, 153};
int y_ship[57] = {200, 199, 198, 197, 196, 201, 200, 199, 198, 197, 201, 200, 199, 198, 197, 202, 201, 200, 199, 198, 202, 201, 200, 199, 198, 203, 202, 201, 200, 199, 203, 202, 201, 200, 199, 204, 203, 202, 201, 200, 204, 203, 202, 201, 200, 202, 203, 204, 202, 203, 204, 203, 204, 203, 204, 204, 204};
int x_ship0[57] = {160, 160, 160, 160, 160, 161, 161, 161, 161, 161, 159, 159, 159, 159, 159, 162, 162, 162, 162, 162, 158, 158, 158, 158, 158, 163, 163, 163, 163, 163, 157, 157, 157, 157, 157, 164, 164, 164, 164, 164, 156, 156, 156, 156, 156, 165, 165, 165, 155, 155, 155, 166, 166, 154, 154, 167, 153};
int y_ship0[57] = {200, 199, 198, 197, 196, 201, 200, 199, 198, 197, 201, 200, 199, 198, 197, 202, 201, 200, 199, 198, 202, 201, 200, 199, 198, 203, 202, 201, 200, 199, 203, 202, 201, 200, 199, 204, 203, 202, 201, 200, 204, 203, 202, 201, 200, 202, 203, 204, 202, 203, 204, 203, 204, 203, 204, 204, 204};
int x_laser = 160;
int y_laser = 196;
int SDRAM_x[58] = {0};
int SDRAM_y[58] = {0};
int ONCHIP_x[58] = {0};
int ONCHIP_y[58] = {0};
int current_degree = 90;
int laser_direction;
int score = 0;

void draw_line(int x0, int y0, int x1, int y1, short int line_color);
void draw_box(int x, int y, short int color);
void draw_ship(short int color);
void erase_ship(int x[], int y[], short int color);
void move_ship(bool memory);
void draw_laser(short int color);
void move_laser();
void erase_laser(int x, int y, short int color);
void rotate(int x_pivot, int y_pivot, double radian, bool clockwise, bool memory);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    key_address = (int *)0xFF200050;
	ps2_address = (int *)0xFF200100;
	short int white = 0xFFFF;
	short int black = 0x0000;
	short int red = 0xFC00;
	bool laser_on_screen = false;
	bool turn_left_released = false;
	bool turn_right_released = false;
	int PS2_data, RVALID;
	char keyboard_byte;
	char previous_byte;
	char prev_prev_byte;

    /*Initialize the arrays on the new instances of asteroids*/
    struct big_asteroids b_a1;
    b_a1.x_pos = 2;
    b_a1.y_pos = 2;
    b_a1.x_step = 1;
    b_a1.y_step = 1;
	initialize_big_asteroid_array(&b_a1);

	struct big_asteroids b_a2;
    b_a2.x_pos = 230;
    b_a2.y_pos = 2;
    b_a2.x_step = -1;
    b_a2.y_step = 1;
	initialize_big_asteroid_array(&b_a2);

    struct med_asteroids m_a1;
    m_a1.x_pos = 150;
    m_a1.y_pos = 20;
    m_a1.x_step = 0;
    m_a1.y_step = 1;
	initialize_med_asteroid_array(&m_a1);

    struct small_asteroids s_a1;
	s_a1.x_pos = 150;
    s_a1.y_pos = 240;
    s_a1.x_step = 0;
    s_a1.y_step = -1;
	initialize_small_asteroid_array(&s_a1);

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
										
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync1(pixel_ctrl_ptr);
	
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	clear_screen(); // pixel_buffer_start points to the pixel buffer

    int degree = 90;
	double radian  = (double)degree*3.141592653589/180;
	bool memory = false;  //false is ONCHIP, true is SDRAM
	int counter = 0;
	int index = 0;
			
    while (1)
    {
		PS2_data = *(ps2_address);
		RVALID = PS2_data & 0x8000;
		if(RVALID){
			if(keyboard_byte != (0xF0)){
				prev_prev_byte = previous_byte;
				previous_byte = keyboard_byte;
				keyboard_byte = PS2_data & 0xFF;
			}
			else{
				prev_prev_byte = previous_byte;
				previous_byte = keyboard_byte;
				keyboard_byte = 0;
			}
		}
		
		if((prev_prev_byte == 0x74)&&(previous_byte == 0xE0)&&(keyboard_byte == 0xF0)){
			turn_right_released = true;
		}
		
		if((prev_prev_byte == 0x6B)&&(previous_byte == 0xE0)&&(keyboard_byte == 0xF0)){
			turn_left_released = true;
		}
		
		if(counter == 1){
			counter = 0;
			clear_screen();
		}
		if(x_laser <= 5 || x_laser >= 315 || y_laser <= 10 || y_laser >= 235){
			laser_on_screen = false;
			clear_screen();
			counter = 1;
			x_laser = x_ship[0];
			y_laser = y_ship[4];
		}
		if(memory){
			erase_ship(SDRAM_x, SDRAM_y, black);
			if(laser_on_screen){
				erase_laser(SDRAM_x[57], SDRAM_y[57], black);
			}
		}
		else{
			erase_ship(ONCHIP_x, ONCHIP_y, black);
			if(laser_on_screen){
				erase_laser(ONCHIP_x[57], ONCHIP_y[57], black);
			}
		}
		

		if(!memory){
			for(int i = 0; i < 57; ++i){
				ONCHIP_x[i] = x_ship[i];
				ONCHIP_y[i] = y_ship[i];
			}
			ONCHIP_x[57] = x_laser;
			ONCHIP_y[57] = y_laser;
		}
		else{
			for(int i = 0; i < 57; ++i){
				SDRAM_x[i] = x_ship[i];
				SDRAM_y[i] = y_ship[i];
			}
			SDRAM_x[57] = x_laser;
			SDRAM_y[57] = y_laser;
		}
		
		draw_ship(white);
		// code for updating the locations of boxes (not shown)
		if(laser_on_screen){
			draw_laser(red);
		}

		if(laser_on_screen){
			move_laser();
		}
		if(turn_right_released){
			rotate(x_ship[0], y_ship[0], radian, true, memory);
			turn_right_released = false;
			//wait_for_key_release();
		}
		else if(keyboard_byte == 0x75){
			move_ship(memory);
			//wait_for_key_release();
		}
		else if(turn_left_released){
			rotate(x_ship[0], y_ship[0], radian*-1, false, memory);
			turn_left_released = false;
			//wait_for_key_release();
		}
		else if((keyboard_byte == 0x29) && !laser_on_screen){
			x_laser = x_ship[0];
			y_laser = y_ship[4];
			laser_direction = current_degree;
			//draw_laser(red);
			laser_on_screen = true;
			//wait_for_key_release();
		}	
		memory = !memory;

        //asteroid 1
		clear_big_asteroid(&b_a1);
		draw_big_asteroid(&b_a1);
		b_a1.x_pos = b_a1.x_step+b_a1.x_pos;
		b_a1.y_pos = b_a1.y_step+b_a1.y_pos; 
		if((x_laser>=b_a1.x_pos)&&(x_laser<(b_a1.x_pos+big_x))&&(y_laser>=b_a1.y_pos)&&(y_laser<(b_a1.y_pos+big_y)))//laser hits the asteroid
		{
			clear_big_asteroid(&b_a1);
			b_a1.x_pos = 2;
            b_a1.y_pos = 2;
			++score;
		}
        if(b_a1.y_pos>239)
        {
            b_a1.x_pos = 2;
            b_a1.y_pos = 2;
        }
		if(((b_a1.x_pos<x_ship[4])&&((b_a1.x_pos+big_x>x_ship[4]))&&(b_a1.y_pos<y_ship[4])&&((b_a1.y_pos+big_y)>y_ship[4]))||
		((b_a1.x_pos<x_ship[56])&&((b_a1.x_pos+big_x>x_ship[56]))&&(b_a1.y_pos<y_ship[56])&&((b_a1.y_pos+big_y)>y_ship[56]))||
		((b_a1.x_pos<x_ship[55])&&((b_a1.x_pos+big_x>x_ship[55]))&&(b_a1.y_pos<y_ship[55])&&((b_a1.y_pos+big_y)>y_ship[55])))
		{
			clear_screen();
			score = 0;
			laser_direction = 90;
			for(index = 0; index<57; ++index)
			{
				y_ship[index] = y_ship0[index];
				x_ship[index] = x_ship0[index];
			}
		}

		//asteroid 2
		if(score>2){
			clear_big_asteroid(&b_a2);
			draw_big_asteroid(&b_a2);
			b_a2.x_pos = b_a2.x_step+b_a2.x_pos;
			b_a2.y_pos = b_a2.y_step+b_a2.y_pos;
			if((x_laser>=b_a2.x_pos)&&(x_laser<(b_a2.x_pos+big_x))&&(y_laser>=b_a2.y_pos)&&(y_laser<(b_a2.y_pos+big_y)))
			{
				clear_big_asteroid(&b_a2);
				b_a2.x_pos = 250;
				b_a2.y_pos = 2;
				++score;
			}  
			if(b_a2.y_pos>239)
			{
				b_a2.x_pos = 250;
				b_a2.y_pos = 2;
			}
			if(((b_a2.x_pos<x_ship[4])&&((b_a2.x_pos+big_x>x_ship[4]))&&(b_a2.y_pos<y_ship[4])&&((b_a2.y_pos+big_y)>y_ship[4]))||
			((b_a2.x_pos<x_ship[56])&&((b_a2.x_pos+big_x>x_ship[56]))&&(b_a2.y_pos<y_ship[56])&&((b_a2.y_pos+big_y)>y_ship[56]))||
			((b_a2.x_pos<x_ship[55])&&((b_a2.x_pos+big_x>x_ship[55]))&&(b_a2.y_pos<y_ship[55])&&((b_a2.y_pos+big_y)>y_ship[55])))
			{
				clear_screen();
				score = 0;
				laser_direction = 90;
				for(index = 0; index<57; ++index)
				{
					y_ship[index] = y_ship0[index];
					x_ship[index] = x_ship0[index];
				}
			}
		}
		
		//asteroid 3
		if(score>5){
			clear_med_asteroid(&m_a1);
			draw_med_asteroid(&m_a1);
			m_a1.x_pos = m_a1.x_step + m_a1.x_pos;
			m_a1.y_pos = m_a1.y_step + m_a1.y_pos;  
			if((x_laser>=m_a1.x_pos)&&(x_laser<(m_a1.x_pos+med_x))&&(y_laser>=m_a1.y_pos)&&(y_laser<(m_a1.y_pos+med_y)))
			{
				clear_med_asteroid(&m_a1);
				m_a1.x_pos = 150;
				m_a1.y_pos = 2;
				++score;
			} 
			if(m_a1.y_pos>239)
			{
				m_a1.x_pos = 150;
				m_a1.y_pos = 2;
			}
			if(((m_a1.x_pos<x_ship[4])&&((m_a1.x_pos+big_x>x_ship[4]))&&(m_a1.y_pos<y_ship[4])&&((m_a1.y_pos+big_y)>y_ship[4]))||
			((m_a1.x_pos<x_ship[56])&&((m_a1.x_pos+big_x>x_ship[56]))&&(m_a1.y_pos<y_ship[56])&&((m_a1.y_pos+big_y)>y_ship[56]))||
			((m_a1.x_pos<x_ship[55])&&((m_a1.x_pos+big_x>x_ship[55]))&&(m_a1.y_pos<y_ship[55])&&((m_a1.y_pos+big_y)>y_ship[55])))
			{
				clear_screen();
				score = 0;
				laser_direction = 90;
				for(index = 0; index<57; ++index)
				{
					y_ship[index] = y_ship0[index];
					x_ship[index] = x_ship0[index];
				}
			}
		}

		//asteroid 4
		if(score>8){
			clear_small_asteroid(&s_a1);
			draw_small_asteroid(&s_a1);
			s_a1.x_pos = s_a1.x_step + s_a1.x_pos;
			s_a1.y_pos = s_a1.y_step + s_a1.y_pos;
			if((x_laser>=s_a1.x_pos)&&(x_laser<(s_a1.x_pos+small_x))&&(y_laser>=s_a1.y_pos)&&(y_laser<(s_a1.y_pos+small_y)))
			{
				clear_small_asteroid(&s_a1);
				s_a1.x_pos = 150;
				s_a1.y_pos = 230;
				++score;
			}    
			if(s_a1.y_pos<0)
			{
				s_a1.x_pos = 150;
				s_a1.y_pos = 230;
			}
			if(((s_a1.x_pos<x_ship[4])&&((s_a1.x_pos+big_x>x_ship[4]))&&(s_a1.y_pos<y_ship[4])&&((s_a1.y_pos+big_y)>y_ship[4]))||
			((s_a1.x_pos<x_ship[56])&&((s_a1.x_pos+big_x>x_ship[56]))&&(s_a1.y_pos<y_ship[56])&&((s_a1.y_pos+big_y)>y_ship[56]))||
			((s_a1.x_pos<x_ship[55])&&((s_a1.x_pos+big_x>x_ship[55]))&&(s_a1.y_pos<y_ship[55])&&((s_a1.y_pos+big_y)>y_ship[55])))
			{
				clear_screen();
				score = 0;
				laser_direction = 90;
				for(index = 0; index<57; ++index)
				{
					y_ship[index] = y_ship0[index];
					x_ship[index] = x_ship0[index];
				}
			}
		}

        wait_for_vsync1(pixel_ctrl_ptr); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

/*This function plots the pixel at a position*/
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

/*This function sets a black background on the screen*/
void clear_screen()
{
	int x;
	int y;
	for(x=0; x<320; ++x)
	{	
		for(y=0; y<240; ++y)
		{
			plot_pixel(x,y,0x0000);
		}
	}
}

/*This function is used to draw asteroids*/
void draw_big_asteroid(struct big_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<big_x; ++i)
	{
		for(j=0; j<big_y; ++j)
		{
			if(b_a->asteroid[i][j] > 0)
			{
				m = i+(b_a->x_pos);
				n = j+(b_a->y_pos);
				if((m>319)||(n>239))
				{
                    // m = m-319;
                    // n = n-239;
                    // plot_pixel(m,n,0xFFFF);
                }
				else
					plot_pixel(m,n,0xFFFF);
			}
		}
	}
}

/*This function is used to draw asteroids*/
void draw_med_asteroid(struct med_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<med_x; ++i)
	{
		for(j=0; j<med_y; ++j)
		{
			if(b_a->asteroid[i][j] > 0)
			{
				m = i+(b_a->x_pos);
				n = j+(b_a->y_pos);
				if((m>319)||(n>239))
				{
                    // m = m -319;
                    // n = n -239;
                    // plot_pixel(m,n,0xFFFF);
                }
				else
					plot_pixel(m,n,0xFFFF);
			}
		}
	}
}

/*This function is used to draw asteroids*/
void draw_small_asteroid(struct small_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<small_x; ++i)
	{
		for(j=0; j<small_y; ++j)
		{
			if(b_a->asteroid[i][j] > 0)
			{
				m = i+(b_a->x_pos);
				n = j+(b_a->y_pos);
				if((m>319)||(n>239))
				{
                    // m = m -319;
                    // n = n -239;
                    // plot_pixel(m,n,0xFFFF);
                }
				else
					plot_pixel(m,n,0xFFFF);
			}
		}
	}
}

/*This function clears the draw of an asteroid*/
void clear_big_asteroid(struct big_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<big_x; ++i)
	{
		for(j=0; j<big_y; ++j)
		{
			if(b_a->asteroid[i][j] >0){
				m = i+(b_a->x_pos)-(2*(b_a->x_step));
				n = j+(b_a->y_pos)-(2*(b_a->y_step));
				if((m>319)||(n>239))
				{
                }
				else
					plot_pixel(m,n,0x0000);}
		}
	}
}

/*This function clears the draw of an asteroid*/
void clear_med_asteroid(struct med_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<med_x; ++i)
	{
		for(j=0; j<med_y; ++j)
		{
			if(b_a->asteroid[i][j] >0){
				m = i+(b_a->x_pos)-(2*(b_a->x_step));
				n = j+(b_a->y_pos)-(2*(b_a->y_step));
				if((m>319)||(n>239))
				{
                }
				else
					plot_pixel(m,n,0x0000);}
		}
	}
}

/*This function clears the draw of an asteroid*/
void clear_small_asteroid(struct small_asteroids* b_a)
{   
	int i,j;
	int m,n;
	for(i=0; i<small_x; ++i)
	{
		for(j=0; j<small_y; ++j)
		{	if((b_a->y_pos==230)||(b_a->y_pos==229)){
				m = 150 + i;
				n = j;
				plot_pixel(m,n,0x0000);
			}
			else{
				if(b_a->asteroid[i][j] >0){
					m = i+(b_a->x_pos)-(2*(b_a->x_step));
					n = j+(b_a->y_pos)-(2*(b_a->y_step));
					if((m>319)||(n>239))
					{
					}
					else
						plot_pixel(m,n,0x0000);
				}
			}
		}
	}
}

/*This function waits for the DMA controller to finish drawing*/
void wait_for_vsync1(volatile int* pixel_ctrl_ptr)
{
	register int status; // used to store the status register value
	*pixel_ctrl_ptr = 1; //start synchronization proccess
	
	status = *(pixel_ctrl_ptr + 3);
	while((status & 0x01) != 0) //wait for s to become 0
	{
		status = *(pixel_ctrl_ptr + 3);
	}
}

/*This function draws the shape of the asteroid in the 2D array inside the struct*/
void initialize_big_asteroid_array(struct big_asteroids* b_a)
{
    int i,j;
    for(i=0;i<big_x;++i){
        for(j=0;j<big_y;++j){
            b_a->asteroid[i][j]=0;
        }
    }

	draw_big_line(0,20,20,0,b_a);
	draw_big_line(20,0,40,20,b_a);
	draw_big_line(40,20,30,50,b_a);
	draw_big_line(30,50,10,40,b_a);
	draw_big_line(10,40,0,20,b_a);
	
	bool draw =0;
	
	for(i=0; i<big_x-1; ++i)
	{
		for(j=0; j<big_y-1; ++j)
		{
			if((b_a->asteroid[i][j] == 1)&&(b_a->asteroid[i][j+1]==0))
				draw = !draw;
			
			if(draw)
				b_a->asteroid[i][j] = 2;
		}
		if((i==0)||(i==40))
		{
			clean_big_line(i,big_y-1,b_a);
		}
		draw = 0;
	}
}

/*This function draws the shape of the asteroid in the 2D array inside the struct*/
void initialize_med_asteroid_array(struct med_asteroids* b_a)
{
    int i,j;
    for(i=0;i<med_x;++i){
        for(j=0;j<med_y;++j){
            b_a->asteroid[i][j]=0;
        }
    }

	draw_med_line(0,10,10,0,b_a);
	draw_med_line(10,0,20,10,b_a);
	draw_med_line(20,10,15,25,b_a);
	draw_med_line(15,25,5,20,b_a);
	draw_med_line(5,20,0,10,b_a);
	
	bool draw =0;
	
	for(i=0; i<med_x-1; ++i)
	{
		for(j=0; j<med_y-1; ++j)
		{
			if((b_a->asteroid[i][j] == 1)&&(b_a->asteroid[i][j+1]==0))
				draw = !draw;
			
			if(draw)
				b_a->asteroid[i][j] = 2;
		}
		if((i==0)||(i==20))
		{
			clean_med_line(i,med_y-1,b_a);
		}
		draw = 0;
	}
}

/*This function draws the shape of the asteroid in the 2D array inside the struct*/
void initialize_small_asteroid_array(struct small_asteroids* b_a)
{
    int i,j;
    for(i=0;i<small_x;++i){
        for(j=0;j<small_y;++j){
            b_a->asteroid[i][j]=0;
        }
    }

	draw_small_line(0,4,4,0,b_a);
	draw_small_line(4,0,8,4,b_a);
	draw_small_line(8,4,6,10,b_a);
	draw_small_line(6,10,2,8,b_a);
	draw_small_line(2,8,0,4,b_a);
	
	bool draw =0;
	
	for(i=0; i<small_x-1; ++i)
	{
		for(j=0; j<small_y-1; ++j)
		{
			if((b_a->asteroid[i][j] == 1)&&(b_a->asteroid[i][j+1]==0))
				draw = !draw;
			
			if(draw)
				b_a->asteroid[i][j] = 2;
		}
		if((i==0)||(i==8))
		{
			clean_small_line(i,small_y-1,b_a);
		}
		draw = 0;
	}
}

/*cleans the line*/
void clean_big_line(int i,int y, struct big_asteroids* b_a){
	int j;
	for(j=0; j<y; ++j)
	{
		b_a->asteroid[i][j] = 0;
	}
}

/*cleans the line*/
void clean_med_line(int i,int y, struct med_asteroids* b_a){
	int j;
	for(j=0; j<y; ++j)
	{
		b_a->asteroid[i][j] = 0;
	}
}

/*cleans the line*/
void clean_small_line(int i,int y, struct small_asteroids* b_a){
	int j;
	for(j=0; j<y; ++j)
	{
		b_a->asteroid[i][j] = 0;
	}
}

/*This function draws a line from (x1,y1) to (x0,y0) with color line_color*/
void draw_big_line(int x0, int y0, int x1, int y1, struct big_asteroids* b_a)
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
	int x;
	
	for (x=x0; x < x1+1; ++x){
		if (is_steep){
			b_a->asteroid[y][x] = 1;
		}
		else {
			b_a->asteroid[x][y] = 1;
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
	}
}

/*This function draws a line from (x1,y1) to (x0,y0) with color line_color*/
void draw_med_line(int x0, int y0, int x1, int y1, struct med_asteroids* b_a)
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
	int x;
	
	for (x=x0; x < x1+1; ++x){
		if (is_steep){
			b_a->asteroid[y][x] = 1;
		}
		else {
			b_a->asteroid[x][y] = 1;
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
	}
}

/*This function draws a line from (x1,y1) to (x0,y0) with color line_color*/
void draw_small_line(int x0, int y0, int x1, int y1, struct small_asteroids* b_a)
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
	int x;
	
	for (x=x0; x < x1+1; ++x){
		if (is_steep){
			b_a->asteroid[y][x] = 1;
		}
		else {
			b_a->asteroid[x][y] = 1;
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
	}
}


/*The following functions are for controlling the ship*/
/******************************************************/
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

void erase_ship(int x[], int y[], short int color){
	for (int i = 0; i < 57; ++i){
		plot_pixel(x[i], y[i], color);
	}
}

void move_ship(bool memory){
	if(current_degree == 0){
		for(int i = 0; i < 57; ++i){
			x_ship[i] = x_ship[i] + 3;
		}
	}
	else if(current_degree == 90 || current_degree == -270){
		for(int i = 0; i < 57; ++i){
			y_ship[i] = y_ship[i] - 3;
		}
	}
	else if(current_degree == 180 || current_degree == -180){
		for(int i = 0; i < 57; ++i){
			x_ship[i] = x_ship[i] - 3;
		}
	}
	else if(current_degree == 270 || current_degree == -90){
		for(int i = 0; i < 57; ++i){
			y_ship[i] = y_ship[i] + 3;
		}
	}
}

void draw_laser(short int color){
	if(laser_direction == 0){
		draw_line(x_laser, y_laser, x_laser + 5, y_laser, color);
	}
	else if(laser_direction == 90 || laser_direction == -270){
		draw_line(x_laser, y_laser, x_laser, y_laser - 5, color);
	}
	else if(laser_direction == 180 || laser_direction == -180){
		draw_line(x_laser, y_laser, x_laser - 5, y_laser, color);
	}
	else if(laser_direction == 270 || laser_direction == -90){
		draw_line(x_laser, y_laser, x_laser, y_laser + 5, color);
	}
}

void move_laser(){
	if(laser_direction == 0){
		x_laser = x_laser + 5;
	}
	else if(laser_direction == 90 || laser_direction == -270){
		y_laser = y_laser - 5;
	}
	else if(laser_direction == 180 || laser_direction == -180){
		x_laser = x_laser - 5;
	}
	else if(laser_direction == 270 || laser_direction == -90){
		y_laser = y_laser + 5;
	}
}

void erase_laser(int x, int y, short int color){
	if(laser_direction == 0){
		draw_line(x_laser, y_laser, x_laser + 5, y_laser, color);
	}
	else if(laser_direction == 90 || laser_direction == -270){
		draw_line(x_laser, y_laser, x_laser, y_laser - 5, color);
	}
	else if(laser_direction == 180 || laser_direction == -180){
		draw_line(x_laser, y_laser, x_laser - 5, y_laser, color);
	}
	else if(laser_direction == 270 || laser_direction == -90){
		draw_line(x_laser, y_laser, x_laser, y_laser + 5, color);
	}
}

void rotate(int x_pivot, int y_pivot, double radian, bool clockwise, bool memory){
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
	if(clockwise){
		current_degree -= 90;
		current_degree = current_degree % 360;
	}
	else{
		current_degree += 90;
		current_degree = current_degree % 360;
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