/*This version is able to draw asteroids and have them flying without crashing towards the center of the screen*/

#include <stdbool.h>
#include <stdlib.h>
	
const int big_x = 51;
const int big_y = 51;
const int med_x = 26;
const int med_y = 26;
const int small_x = 11;
const int small_y = 11;

volatile int pixel_buffer_start; // global variable
void clear_screen();

void draw_asteroid(int x, int y, int big_asteroids[51][51]);
void wait_for_vsync(volatile int* pixel_ctrl_ptr);
void draw_line(int x0, int y0, int x1, int y1, int asteroids[51][51]);
void clean_line(int i,int y, int big_asteroids[51][51]);
void clear_asteroid(int x, int y, int asteroids[51][51]);
void initialize_asteroid_array(int big_asteroids[51][51]);
//int asteroids[320][240] = {{0},{0}};

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

	int big_asteroids[51][51] = {{0},{0}};
	int med_asteroids[26][26] = {{0},{0}};
	int small_asteroids[26][26] = {{0},{0}};
	
    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer
										
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync(pixel_ctrl_ptr);
	
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	clear_screen(); // pixel_buffer_start points to the pixel buffer
	
	initialize_asteroid_array(big_asteroids);
			
	int x=2,y=2;
    while (1)
    {
	//	clear_screen();
		//draw_asteroid(x,y,big_asteroids);
        // code for drawing the boxes and lines (not shown)
        // code for updating the locations of boxes (not shown)
		
		clear_asteroid(x-2, y-2, big_asteroids);
		
		draw_asteroid(x,y,big_asteroids);
		++x;
		++y;
		
        wait_for_vsync(pixel_ctrl_ptr); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }
}

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

void draw_asteroid(int x, int y, int asteroids[big_x][big_y])
{   
	int i,j;
	int m,n;
	for(i=0; i<50; ++i)
	{
		for(j=0; j<50; ++j)
		{
			if(asteroids[i][j] > 0)
			{
				m = i+x;
				n = j+y;
				if((m>319)||(n>239))
				{}
				else
					plot_pixel(m,n,0xFFFF);
			}
		}
	}
}


/*This function waits for the DMA controller to finish drawing*/
void wait_for_vsync(volatile int* pixel_ctrl_ptr)
{
	register int status; // used to store the status register value
	*pixel_ctrl_ptr = 1; //start synchronization proccess
	
	status = *(pixel_ctrl_ptr + 3);
	while((status & 0x01) != 0) //wait for s to become 0
	{
		status = *(pixel_ctrl_ptr + 3);
	}
}

/*This function draws a line from (x1,y1) to (x0,y0) with color line_color*/
void draw_line(int x0, int y0, int x1, int y1, int asteroids[big_x][big_y])
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
			asteroids[y][x] = 1;
		}
		else {
			asteroids[x][y] = 1;
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
	}
}

void clean_line(int i,int y,int asteroids[big_x][big_y]){
	int j;
	for(j=0; j<y; ++j)
	{
		asteroids[i][j] = 0;
	}
}

void initialize_asteroid_array(int big_asteroids[big_x][big_y])
{
	int x = -50;
	int y = -50;
	draw_line(50+x,70+y,70+x,50+y,big_asteroids);
	draw_line(70+x,50+y,90+x,70+y,big_asteroids);
	draw_line(90+x,70+y,80+x,100+y,big_asteroids);
	draw_line(80+x,100+y,60+x,90+y,big_asteroids);
	draw_line(60+x,90+y,50+x,70+y,big_asteroids);
	
	int i,j;
	bool draw =0;
	
	for(i=0; i<50; ++i)
	{
		for(j=0; j<50; ++j)
		{
			if((big_asteroids[i][j] == 1)&&(big_asteroids[i][j+1]==0))
				draw = !draw;
			
			if(draw)
				big_asteroids[i][j] = 2;
		}
		if((i==0)||(i==40))
		{
			clean_line(i,50,big_asteroids);
		}
		draw = 0;
	}
	
}

void clear_asteroid(int x, int y, int asteroids[big_x][big_y])
{   
	int i,j;
	int m,n;
	for(i=0; i<50; ++i)
	{
		for(j=0; j<50; ++j)
		{
			if(asteroids[i,j] >0){
				m = i+x;
				n = j+y;
				if((m>319)||(n>239))
				{}
				else
					plot_pixel(m,n,0x0000);}
		}
	}
}
