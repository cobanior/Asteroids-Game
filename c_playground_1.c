// Hello world! Cplayground is an online sandbox that makes it easy to try out
// code.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int asteroids[320][240] = {{0},{0}};

void clean_line(int i){
	int j;
	for(j=0; j<240; ++j)
	{
		asteroids[i][j] = 0;
	}
}

/*This function draws a line from (x1,y1) to (x0,y0) with color line_color*/
void draw_line(int x0, int y0, int x1, int y1)
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

int main() {
    int i,j;
    int r = 5;
	draw_line(50/r,70/r,70/r,50/r);
	draw_line(70/r,50/r,90/r,70/r);
	draw_line(90/r,70/r,80/r,100/r);
	draw_line(80/r,100/r,60/r,90/r);
	draw_line(60/r,90/r,50/r,70/r);
    int m,n;
	bool draw =0;
	
	for(m=0; m<320; ++m)
	{
		for(n=0; n<240; ++n)
		{
			if((asteroids[m][n] == 1)&&(asteroids[m][n+1]==0))
				draw = !draw;
			
			if(draw)
				asteroids[m][n] = 2;
		}
		if(draw)
		{
		    clean_line(m);
		    draw = 0;
		}
	}
	
	int k=0;
	int max = 0;
    for(i=0;i<320;++i)
    {
        for(j=0;j<240;++j)
        {
            if(asteroids[i][j]>0)
                ++k;
        }
        if(k>max)
            max = k;
        printf("%d\n",k);
        k = 0;
    }
    
    k=0;
	int max1 = 0;
    for(i=0;i<240;++i)
    {
        for(j=0;j<320;++j)
        {
            if(asteroids[j][i]>0)
                ++k;
        }
        if(k>max1)
            max1 = k;
        printf("%d\n",k);
        k = 0;
    }
    
	printf("%d\n",max);
	printf("%d\n",max1);
/*    for(i=0;i<320;++i)
    {
        for(j=0;j<240;++j)
        {
            printf("%d",asteroids[i][j]);
        }
        printf("\n");
    }
    
    printf("%d\n",k);*/
    return 0;
}