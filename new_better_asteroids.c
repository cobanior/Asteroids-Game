#include <stdbool.h>
#include <stdlib.h>

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
void plot_pixel(int x, int y, short int line_color);
void clear_screen();
void wait_for_vsync(volatile int* pixel_ctrl_ptr);

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

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

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
    wait_for_vsync(pixel_ctrl_ptr);
	
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
	
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	
    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
	
	clear_screen(); // pixel_buffer_start points to the pixel buffer
			
    while (1)
    {	//asteroid 1
		clear_big_asteroid(&b_a1);
		draw_big_asteroid(&b_a1);
		b_a1.x_pos = b_a1.x_step+b_a1.x_pos;
		b_a1.y_pos = b_a1.y_step+b_a1.y_pos; 
        if(b_a1.y_pos>239)
        {
            b_a1.x_pos = 2;
            b_a1.y_pos = 2;
        }

		//asteroid 2
		clear_big_asteroid(&b_a2);
		draw_big_asteroid(&b_a2);
		b_a2.x_pos = b_a2.x_step+b_a2.x_pos;
		b_a2.y_pos = b_a2.y_step+b_a2.y_pos;  
        if(b_a2.y_pos>239)
        {
            b_a2.x_pos = 250;
            b_a2.y_pos = 2;
        }
		
		//asteroid 3
		clear_med_asteroid(&m_a1);
		draw_med_asteroid(&m_a1);
		m_a1.x_pos = m_a1.x_step + m_a1.x_pos;
		m_a1.y_pos = m_a1.y_step + m_a1.y_pos;   
        if(m_a1.y_pos>239)
        {
            m_a1.x_pos = 150;
            m_a1.y_pos = 2;
        }

		//asteroid 4
		clear_small_asteroid(&s_a1);
		draw_small_asteroid(&s_a1);
		s_a1.x_pos = s_a1.x_step + s_a1.x_pos;
		s_a1.y_pos = s_a1.y_step + s_a1.y_pos;    
        if(s_a1.y_pos<0)
        {
            s_a1.x_pos = 150;
            s_a1.y_pos = 230;
        }

        wait_for_vsync(pixel_ctrl_ptr); // swap front and back buffers on VGA vertical sync
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