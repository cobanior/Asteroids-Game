/*
*In this file is the implemetation on how to draw an asteroid at a (x,y) coordinate.
*Currently the shape of the asteroid is a rombus.
*/

#include <stdbool.h>
#include <stdlib.h>

volatile int pixel_buffer_start; // global variable
void clear_screen();

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
	int i,j,m;
	int x = 23;
	int y = 57;
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

// code not shown for clear_screen() and draw_line() subroutines

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