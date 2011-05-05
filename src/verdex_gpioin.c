/* verdex_gpioin
   
   take input from gumstix verdex gpio

   Copyright (C) 2011 Tim Redfern
   tim@eclectronics.org

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1
   of the License, or (at your option) any later version.
   
   allows use of LDD pins on audiostix board as inputs
   creation parameter 1 - LDD number

   WARNING only use on Gumstix Verdex hardware without LCD fitted
   You could damage hardware!
*/

#include "m_pd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#define max(X,Y) ((X) > (Y) ? (X) : (Y))


t_class *verdex_gpioin_class;

typedef struct verdex_gpioin
{
	t_object x_obj;
	int gpioNum;
	char procfile[18];
	char buffer[11];
	FILE *fs;
	int set; //0 or 1
} t_verdex_gpioin;

void verdex_gpioin_bang(t_verdex_gpioin *x)
{
  //fseek( x->fs, 0, SEEK_SET );
	if (( x->fs = fopen( x->procfile, "r" )) < 0 )
	{
		error( "Unable to open procfile" );
		exit( 1 );
	}
  fread( x->buffer, sizeof(char), 11, x->fs );
  int current=(x->buffer[10]=='s');
  if (current!=x->set) {
	t_float f=current;
	outlet_float(x->x_obj.ob_outlet, f);
	x->set=current;
  }
  fclose(x->fs);
}

void verdex_gpioin_free(t_verdex_gpioin *x)
{
	//
}

void *verdex_gpioin_new(t_floatarg f)
{
    t_verdex_gpioin *x = (t_verdex_gpioin *)pd_new(verdex_gpioin_class);
    outlet_new(&x->x_obj, &s_float);
	int nums[]={58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73};
	x->gpioNum=nums[min(15,max(0,(int) f))]; //maps LDD number to GPIO number
	post("verdex_gpoin: using GPIO(%d): LDD%02d\n",x->gpioNum,min(15,max(0,(int) f)));
	sprintf(x->procfile,"/proc/gpio/GPIO%d",x->gpioNum);
	post("procfile: %s\n",x->procfile);

	if (( x->fs = fopen( x->procfile, "w" )) < 0 )
	{
		error( "Unable to open procfile" );
		exit( 1 );
	}
	fwrite( "AF2 in", sizeof(char), 6, x->fs );
	fclose(x->fs);
	if (( x->fs = fopen( x->procfile, "r" )) < 0 )
	{
		error( "Unable to open procfile" );
		exit( 1 );
	}
	fread( x->buffer, sizeof(char), 11, x->fs );
	x->set=(x->buffer[10]=='s');
	fclose(x->fs);
    return (void *)x;
}

void verdex_gpioin_setup(void)
{
  verdex_gpioin_class = class_new(gensym("verdex_gpioin"), (t_newmethod)verdex_gpioin_new, (t_method)verdex_gpioin_free, sizeof(t_verdex_gpioin), 0, A_DEFFLOAT, 0);
  class_addbang(verdex_gpioin_class,verdex_gpioin_bang);
}
