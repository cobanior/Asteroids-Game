#include <stdbool.h>
#include <stdlib.h>
	
volatile int pixel_buffer_start; // global variable
void clear_screen();
void draw_asteroid(int x, int y);
void wait_for_vsync(volatile int* pixel_ctrl_ptr);

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

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
	
	int x0 = 27;
	int y0 = 50;
	int x1 = 200;
	int y1 = 200;
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

    while (1)
    {	
        // code for drawing the boxes and lines (not shown)
        // code for updating the locations of boxes (not shown)
		if (is_steep){
			draw_asteroid(x,y);
		}
		else {
			draw_asteroid(x,y);
		}
		error = error + dy;
		if (error >= 0){
			y = y + y_step;
			error = error - dx;
		}
		++x;
		
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

void draw_asteroid(int x, int y)
{
	int i,j,m;
	int size = 21;

    clear_screen();

	for(i=0; i<size; ++i)
	{
		if(i<=size/2)
			m = i;
		else
			--m;
		
		for(j=(size/2)-m; j<=(size/2)+m; ++j)
		{
			plot_pixel(x+j, y+i, 0xFFFF);
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