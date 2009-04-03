/*=======================================================================

  Program:  Distance Transforms
  Module:   $Id: distmap-cuisenaire-orig.c,v 1.1 2009-04-03 06:40:10 ricardofabbri Exp $
  Date:     $Date: 2009-04-03 06:40:10 $
  Language: C
  
  Author:   Olivier Cuisenaire, http://ltswww.epfl.ch/~cuisenai
         
  Descr.:   distmap_psn(int *map, int n[2]) 
            distmap_pmn(int *map, int n[2]) 
	    distmap_pmon(int *map, int n[2]) 

   Takes an image (map) of integers of size n[0]*n[1] as input. It 
   computes for all pixels the square of the Euclidean distance to 
   the nearest zero pixel using Cuisenaire's "propagation with 
   multiple neighborhoods" algorithms

   PSN uses a single neighborhood and produces an approx. EDT
   PMN uses multiple neighborhoods
   PMON uses multiple oriented neighrborhood. This is the version
   described in the CVIU paper. 

  Reference for the algorithm: 
  
   O. Cuisenaire and B. Macq, "Fast and exact signed Euclidean distance 
   transformation with linear complexity", ICASSP'99 - IEEE Intl Conference
   on Acoustics, Speech and Signal Processing, Vol. 6, pp. 3293-3296. 
   
  Reference for the implementation:

   Chapter 3 of "Distance transformations: fast algorithms and applications 
   to medical image processing", Olivier Cuisenaire's Ph.D. Thesis, October 
   1999, Université catholique de Louvain, Belgium.
    
  Terms of Use: 

   You can use/modify this program for any use you wish, provided you cite 
   the above references in any publication about it. 
    
  Disclaimer:

   In no event shall the authors or distributors be liable to any party for 
   direct, indirect, special, incidental, or consequential damages arising out 
   of the use of this software, its documentation, or any derivatives thereof, 
   even if the authors have been advised of the possibility of such damage.
   
   The authors and distributors specifically disclaim any warranties, including,
   but not limited to, the implied warranties of merchantability, fitness for a
   particular purpose, and non-infringement.  this software is provided on an 
   "as is" basis, and the authors and distributors have no obligation to provide
   maintenance, support, updates, enhancements, or modifications.
   
=========================================================================*/
#include "distmaps.h"

#include <stdlib.h>
#include <stdio.h>

#define MIN_ERROR_4SED 2
#define MIN_ERROR_8SED 116
#define NUMBER_OF_MASKS 26
/*int min_error[NUMBER_OF_MASKS] = { 2,116,520,2017,4610,10600,18752,34217,52882,*/
/*           84676,120392,167248,238130,314689,426400,*/
/*           567025,680576,912052};*/

int min_error[NUMBER_OF_MASKS] = {
   2,      116,    520,     2017,    4610,    10600,   18752,  34217,
   52882,  84676,  120392,  167248,  238130,  314689,  426400, 567025,
   680576, 
   912052, 1073250, 1394801, 1669282, 2047816, 2411840, 2745585, 
   3378290, 4012100};

#define SIZE 16

typedef struct grid* ptrgrid;

typedef struct grid
{ unsigned short x[SIZE],y[SIZE];
  short dx[SIZE],dy[SIZE];
  int cnt;
  ptrgrid nxt;
} grid;

ptrgrid *master_list;

int distmap_psn( int *map, int n[2])
{
  int maxi, *sq, *dummy;
  dummy=(int*)calloc(4*n[0]+1,sizeof(int)); sq=dummy+2*n[0];

  distmap_initmasterlist(map,n,&maxi,sq);
  distmap_p4sed(map,n,&maxi,sq);
  
  free(master_list);

  return(1);
}

int distmap_pmn( int *map, int n[2])
{
  int maxi, *sq, *dummy;
  dummy=(int*)calloc(4*n[0]+1,sizeof(int)); sq=dummy+2*n[0];

  distmap_initmasterlist(map,n,&maxi,sq);
  distmap_p4sed(map,n,&maxi,sq);
  distmap_p8sed(map,n,maxi,sq);
  distmap_pNxN(map,n,maxi,sq);

  free(master_list);

  return(1);
}

int distmap_pmon( int *map, int n[2])
{
  int maxi, *sq, *dummy;
  dummy=(int*)calloc(4*n[0]+1,sizeof(int)); sq=dummy+2*n[0];

  distmap_initmasterlist(map,n,&maxi,sq);
  distmap_p4sed(map,n,&maxi,sq);
  distmap_p8sed(map,n,maxi,sq);
  distmap_poNxN(map,n,maxi,sq);

  free(master_list);

  return(1);
}

void addtolist(unsigned short x, unsigned short y, short dx, short dy, int val)
{
  /* imported global variables */
  
  extern ptrgrid *master_list;
  
  /* local variables */
  
  ptrgrid tmp;
  int cnt;
	
  /* function body */
  
  tmp=master_list[val];
  if(tmp==NULL) 
    {
      tmp=(grid *)malloc(sizeof(grid));
      tmp->cnt=0;
      tmp->nxt=NULL;
      master_list[val]=tmp;
    };      
  cnt=tmp->cnt;
  if (cnt==SIZE)
    {
      tmp=(grid *)malloc(sizeof(grid));
      if(tmp==NULL) fprintf(stderr,"ldistmap.c: Not enough memory for addtolist() \n");
      tmp->cnt=0;
      tmp->nxt=master_list[val];
      master_list[val]=tmp;
      cnt=0;
    };                      
  tmp->x[cnt]=x;
  tmp->y[cnt]=y;
  tmp->dx[cnt]=dx;
  tmp->dy[cnt]=dy;
  tmp->cnt++;

}

int distmap_initmasterlist(int *map, int n[2], int *max, int *sq)
{
  int maxi;
  
  extern ptrgrid *masterlist;
  ptrgrid *list_ptr;    
  
  int i;

  unsigned short maxx=n[0]-1;   
  unsigned short maxy=n[1]-1;  
  unsigned short x,y;
  unsigned short n1=n[0];
  int *pt;

  for(i=2*n1;i>=0;i--) sq[-i]=sq[i]=i*i;

  /* initialisation */

  maxi=n[0]*n[0]+n[1]*n[1];
  
  master_list=(ptrgrid *)calloc(maxi+2*n[0],sizeof(ptrgrid));
  for(list_ptr=master_list+maxi;list_ptr>=master_list;list_ptr--)
    *list_ptr=NULL;
  
  for(y=0,pt=map;y<=maxy;y++)
    for(x=0;x<=maxx;x++,pt++) {
      if(*pt==0)
	{ 
	  if(x<maxx) if(*(pt+1)>1) 
	    {
	      addtolist(x+1,y,1,0,1);
	      *(pt+1)=1;
	    }
	  if(x>0) if(*(pt-1)>1) 
	    {
	      addtolist(x-1,y,-1,0,1);
	      *(pt-1)=1;
	    }
	  if(y<maxy) if(*(pt+n1)>1) 
	    {
	      addtolist(x,y+1,0,1,1);
	      *(pt+n1)=1;
	    }
	  if(y>0) if(*(pt-n1)>1) 
	    {
	      addtolist(x,y-1,0,-1,1);
	      *(pt-n1)=1;
	    }
	}
      else if(*pt>1) *pt=maxi;
    }
  *max = maxi;
}

int distmap_p4sed(   int *map, int n[3], int *max, int *sq)  
{
  /* global variables */
  
  extern ptrgrid *master_list;
  
  /* internal variables */
  
  int currval;
  int *pt;
  
  int newval, *testpt;
  unsigned short x,y,*X,*Y;
  short dx,dy,*DX,*DY;
  
  ptrgrid tmp,old;
  
  int cnt;     /* counters */
  
  int propa;
  int n_empty_list=0;

  int maxi=*max;

  /* short cuts */
  
  int n1=n[0];            
  unsigned short maxx=n[0]-1;   
  unsigned short maxy=n[1]-1;        

  int counter=0;
  
  /* main loop */ 
  
  for(currval=1;currval<maxi;currval++)
    {
      
      tmp=master_list[currval];   /* debut liste de points de valeur currval */
      master_list[currval]=NULL;
      
      if(tmp==NULL) 
	{
	  n_empty_list++;
	  if(n_empty_list>n1) maxi=currval;
	}
      else
	{
	  n_empty_list=0;
	  
	  while(tmp!=NULL)
	    {   
	      X=tmp->x; Y=tmp->y; DX=tmp->dx; DY=tmp->dy;
	      for(cnt=tmp->cnt-1;cnt>=0;cnt--)
		{                       /* pout tous les points de cette liste */
		  x=X[cnt]; y=Y[cnt];
		  pt=map+x+n1*y;
      		  dx=DX[cnt]; dy=DY[cnt];
		  propa=0;
		  
		  /* x+1 */
		  if(dx>0)
		    {
		      if(x<maxx) 
			{
			  newval=sq[dx+1]+sq[dy];
			  testpt=pt+1;
			  if(newval<*testpt)
			    { 
			      addtolist(x+1,y,dx+1,dy,newval); 
			      *testpt=newval; 
			      propa=1;
			    }
			}
		    }
		  else
		    /* x-1 */
		    if(dx<0) 
		      {
			if(x>0) 
			  {
			    newval=sq[dx-1]+sq[dy];
			    testpt=pt-1;
			    if(newval<*testpt)
			      { 
				addtolist(x-1,y,dx-1,dy,newval); 
				*testpt=newval; 
				propa=1;
			      }
			  }
		      }
		    else
		      /* dx==0 only needs to be propagated transversally
			 for the first two iterations */
		      if(currval==1)
			{
			  if(x<maxx) 
			    {
			      testpt=pt+1;
			      if(2<*testpt)
				{ 
				  addtolist(x+1,y,+1,dy,2); 
				  *testpt=2; 
				  propa=1;
				}
			    }
			  if(x>0) 
			    {
			      testpt=pt-1;
			      if(2<*testpt)
				{ 
				  addtolist(x-1,y,-1,dy,2); 
				  *testpt=2; 
				  propa=1;
				}
			    }
			}
		  
		  /* y+1 */
		  if(dy>0) 
		    {
		      if(y<maxy)
			{
			  newval=sq[dx]+sq[dy+1];
			  testpt=pt+n1;
			  if(newval<*testpt)
			    { 
			      addtolist(x,y+1,dx,dy+1,newval);
			      *testpt=newval; 
			      propa=1;
			    }
			}
		    }
		  else
		    /* y-1 */
		    if(dy<0) 
		      {
			if(y>0) 
			  {
			    newval=sq[dx]+sq[dy-1];
			    testpt=pt-n1;
			    if(newval<*testpt)
			      { 
				addtolist(x,y-1,dx,dy-1,newval);
				*testpt=newval; 
				propa=1;
			      }
			  }
		      }
		    else
		      /* dy==0 only needs to be propagated transversally
		     for the first two iterations */
		      if(currval==1)
			{
			  if(y<maxy)
			    {
			      testpt=pt+n1;
			      if(2<*testpt)
				{ 
				  addtolist(x,y+1,dx,+1,2);
				  *testpt=2; 
				  propa=1;
				}
			    }
			  if(y>0) 
			    {
			      testpt=pt-n1;
			      if(2<*testpt)
				{ 
				  addtolist(x,y-1,dx,-1,2);
				  *testpt=2; 
				  propa=1;
				}
			    }
			} 
		  if(propa==0) if(currval>=MIN_ERROR_4SED) 
		    {
		      addtolist(x,y,dx,dy,0);    
		      counter++;
		    }
		}
	      old=tmp;
	      tmp=tmp->nxt;
	      X=Y=DX=DY=NULL;
	      free(old);
	    }	  
	  master_list[currval]=master_list[0];
	  master_list[0]=NULL;
       }
   }      
  
  *max=maxi; 

  return(1);
}

int distmap_p8sed(   int *map, int n[3], int maxi, int *sq) 
{

  /* global variables */
  
  extern ptrgrid *master_list;

  /* internal variables */
  
  int currval;
  int *pt;
  
  int newval, *testpt;
  unsigned short x,y,*X,*Y;
  short dx,dy,*DX,*DY;
  
  ptrgrid tmp,old;
  
  int cnt;     /* counters */
  
  
  /* short cuts */
  
  int n1=n[0]; int nm1=n1-1; int np1=n1+1; 
  unsigned short maxx=n[0]-1;   
  unsigned short maxy=n[1]-1;        

  /* main loop */ 
  

  for(currval=MIN_ERROR_4SED;currval<MIN_ERROR_8SED;currval++)
    {
      
      tmp=master_list[currval];   /* debut liste de points de valeur currval */
      master_list[currval]=NULL;
	  
      while(tmp!=NULL)
	{   
	  X=tmp->x; Y=tmp->y; DX=tmp->dx; DY=tmp->dy;
	  for(cnt=tmp->cnt-1;cnt>=0;cnt--)
	    {                       /* pout tous les points de cette liste */
	      x=X[cnt]; y=Y[cnt];
	      pt=map+x+n1*y;
	      
	      dx=DX[cnt]; dy=DY[cnt];
	     
		  
	      if(dx>0)
		{
		  if(dy>0)
		    {
		      if((x<maxx)&&(y<maxy)) 
			{
			  testpt=pt+np1;
			  newval=sq[dx+1]+sq[dy+1];
			  if(newval<*testpt)
			    { 
			      addtolist(x+1,y+1,dx+1,dy+1,newval); 
			      *testpt=newval; 
			    }
			}
		    }
		  else
		    if(dy<0) 
		      if((x<maxx)&&(y>0)) 
			{
			  newval=sq[dx+1]+sq[dy-1];
			  testpt=pt-nm1;
			  if(newval<*testpt)
			    { 
			      addtolist(x+1,y-1,dx+1,dy-1,newval); 
			      *testpt=newval; 
			    }
			}
		}
	      else
		/* x-1 */
		if(dx<0) 
		  {	 
		    if(dy>0) 
		      {
			if((x>0)&&(y<maxy)) 
			  {	
			    newval=sq[dx-1]+sq[dy+1];
			    testpt=pt+nm1;
			    if(newval<*testpt)
			      { 
				addtolist(x-1,y+1,dx-1,dy+1,newval); 
				*testpt=newval; 
			      }
			  }
		      }
		    else
		      if(dy<0) 
			if((x>0)&&(y>0)) 
			  {
			    newval=sq[dx-1]+sq[dy-1];
			    testpt=pt-np1;
			    if(newval<*testpt)
			      { 
				addtolist(x-1,y-1,dx-1,dy-1,newval); 
				*testpt=newval; 
			      }
			  }
		  }
	    }	  
	
	  old=tmp;
	  tmp=tmp->nxt;
	  X=Y=DX=DY=NULL;
	  free(old);
	}
    }   
  
  return(1);
}

int distmap_pNxN( int *map, int n[3], int maxi, int *sq)   
{

  /* global variables */  

  extern int min_error[NUMBER_OF_MASKS];
  extern ptrgrid *master_list;

  /* internal variables */

  int mask;
  
  int currval,startval,endval;
  int *pt;
  
  ptrgrid tmp,old;
  int cnt;  
  
  int newval, *testpt;
  unsigned short x,y,*X,*Y;
  short dx,dy,*DX,*DY;

  int sdx,sdy;

  int end_i,end_j;
  int i,j,si,sj;
  
  int n1=n[0]; 
  unsigned short maxx=n[0]-1;   
  unsigned short maxy=n[1]-1;        

  /* main loop */ 
  
  if(maxi>min_error[NUMBER_OF_MASKS-1])
    {
      printf("Warning, perfect map not guaranteed \n");
      printf("     maxi: %d min_error: %d",maxi, min_error[NUMBER_OF_MASKS-1]);
      fprintf(stderr,"Warning, perfect map not guaranteed \n");
      fprintf(stderr,"     maxi: %d max(dnp_table): %d",maxi, min_error[NUMBER_OF_MASKS-1]);

      min_error[NUMBER_OF_MASKS-1]=maxi+1;
    }

  for(mask=2;mask<NUMBER_OF_MASKS;mask++)
    {
      startval=min_error[mask-1]; endval=min_error[mask];
      if(endval>maxi) endval=maxi;


      for(currval=startval;currval<endval;currval++)
	{


	  tmp=master_list[currval];   /* debut liste de points de valeur currval */	  
	  master_list[currval]=NULL;
	  while(tmp!=NULL)
	    {   

	      X=tmp->x; Y=tmp->y; DX=tmp->dx; DY=tmp->dy;
	      for(cnt=tmp->cnt-1;cnt>=0;cnt--)
		{                       /* pout tous les points de cette liste */

		  x=X[cnt]; y=Y[cnt];
		  pt=map+x+n1*y;

		
		  dx=DX[cnt]; dy=DY[cnt]; 

		  if(dx>0) { sdx=1; end_i=maxx-x; }
		  else { sdx=-1; end_i=x; }
		  if(end_i>mask) end_i=mask;

		  if(dy>0) { sdy=1; end_j=maxy-y; }
		  else { sdy=-1;  end_j=y; }
		  if(end_j>mask) end_j=mask;
		  
		  for(i=1;i<=end_i;i++)
		    for(j=1; j<=end_j; j++)
		      {
			si=sdx*i; sj=sdy*j; /* signed i,j */
			
			newval=sq[dx+si]+sq[dy+sj];
			testpt=pt+si+sj*n1;
			if(newval<*testpt)
			  {
			    addtolist(x+si,y+sj,dx+si,dy+sj,newval);
			    *testpt=newval;
			  }
		      }
		}
	      
	      
	      old=tmp;	      
	      tmp=tmp->nxt;
	      X=Y=DX=DY=NULL;
	      free(old);

	    };
	  
	};      
    }
      
  return(1);
}

int distmap_poNxN(   int *map, int n[3], int maxi,int *sq)   
{

  /* global variables */  

  extern int min_error[NUMBER_OF_MASKS];
  extern ptrgrid *master_list;

  /* internal variables */

  int mask;
  
  int currval,startval,endval;
  int *pt;
  
  ptrgrid tmp,old;
  int cnt;  
  
  int newval, *testpt;
  unsigned short x,y,*X,*Y;
  short dx,dy,*DX,*DY;

  int sdx,sdy;
  float adx,ady;
  float start,end;
  float step_start,step_end;
  
  int end_main,max;
  int i,j,si,sj;
  
  int n1=n[0]; 
  unsigned short maxx=n[0]-1;   
  unsigned short maxy=n[1]-1;        

  /* main loop */ 
  
  if(maxi>min_error[NUMBER_OF_MASKS-1])
    {
      fprintf(stderr,"Warning, perfect map not guaranteed \n");
      min_error[NUMBER_OF_MASKS-1]=maxi+1;
    }

  for(mask=2;mask<NUMBER_OF_MASKS;mask++)
    {
      startval=min_error[mask-1]; endval=min_error[mask];
      if(endval>maxi) endval=maxi;


      for(currval=startval;currval<endval;currval++)
	{


	  tmp=master_list[currval];   /* debut liste de points de valeur currval */	  
	  master_list[currval]=NULL;
	  while(tmp!=NULL)
	    {   

	      X=tmp->x; Y=tmp->y; DX=tmp->dx; DY=tmp->dy;
	      for(cnt=tmp->cnt-1;cnt>=0;cnt--)
		{                       /* pout tous les points de cette liste */

		  x=X[cnt]; y=Y[cnt];
		  pt=map+x+n1*y;

		
		  dx=DX[cnt]; dy=DY[cnt];
	
		  		
		  /* sdx,sdy, signes of dx and dy;  adx,ady, absolute values of dx and dy */ 

		  if(dx>0) { sdx=1; adx=dx; }
		  else { sdx=-1; adx=-dx; }

		  if(dy>0) { sdy=1; ady=dy; }
		  else { sdy=-1; ady=-dy; }

	

		  if(adx>ady)
		    {
		      if(dx>0) end_main=maxx-x; else end_main=x;
		      if(end_main>mask) end_main=mask;

		      step_start = ady/adx; 
		      step_end = ady/(adx-1); 
		      start = -step_start ; end= 1 ;
		      
		      if(dy>0) max=maxy-y; else max=y;

		      for(i=1;i<=end_main;i++)
			{
			  start += step_start;  
			  end += step_end;      if(end>max) end=max;
			  
			  for(j=start; j<=end; j++)
			    {
			      si=sdx*i; sj=sdy*j; /* signed i,j */

			      newval=sq[dx+si]+sq[dy+sj];
			      testpt=pt+si+sj*n1;
			      if(newval<*testpt)
				{
				  
				
				  addtolist(x+si,y+sj,dx+si,dy+sj,newval);
				  *testpt=newval;
				}
			    }
			}
		    }
		  else /* ady>adx */
		    {
		     
	

		      if(dy>0) end_main=maxy-y; else end_main=y;
		      if(end_main>mask) end_main=mask;

		      step_start = adx/ady; 
		      step_end = adx/(ady-1); 
		      start = -step_start ; end= 1 ;
		      
		      if(dx>0) max=maxx-x; else max=x;


		      for(j=1;j<=end_main;j++)
			{	

			  start += step_start;  
			  end += step_end;      if(end>max) end=max;
			  
			  for(i=start; i<=end; i++)
			    {
			      si=sdx*i; sj=sdy*j; /* signed i,j */
			      newval=sq[dx+si]+sq[dy+sj];
			      testpt=pt+si+sj*n1;
			      if(newval<*testpt)
				{
	
				  addtolist(x+si,y+sj,dx+si,dy+sj,newval);
				  *testpt=newval;
				}
			    }
			}
		    }
		}
	      old=tmp;	      
	      tmp=tmp->nxt;
	      X=Y=DX=DY=NULL;
	      free(old);
	    }
	}
    }
      
  return(1);
}



AnimalExport bool
edt_cuisenaire_pmn_orig(ImgPUInt32 *im)
{
   puint32 *sq, max_dist;
   char *fname="edt_cuisenaire_psn";
   bool stat;

/*   printf("*****  HERE!!! ************ \n");*/
   assert(im->isbinary);

   // convert to int
   int *image;
  int r = im->rows;
  int c = im->cols;

  image = (int*) calloc ( r*c, sizeof(int));
  if (image == NULL)
    return false;

  int infty = INT_MAX - r*r - c*c;
  int i;
  for (i=0; i < r*c; ++i)
    image[i] = (DATA(im)[i])? infty : 0;

  int size[2];
  size[0] = r;
  size[1] = c;

  distmap_pmn(image,size);

  for (i=0; i < r*c; ++i)
    DATA(im)[i] = image[i];

  free(image);
  im->isbinary = false;

  return true;
}
