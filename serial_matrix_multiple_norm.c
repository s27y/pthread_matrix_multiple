#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <sys/time.h>
#include <math.h>
#include "io.c"

double Multiply(int n, double** a, double** b, double** c)
{
     int i=0;
     int j=0;
     int r=0;
     double single_row_total =0;
     double my_max=0;
  
     struct timeval tv1, tv2;
     struct timezone tz;

      gettimeofday(&tv1, &tz);	
     cblas_dgemm( CblasRowMajor,  CblasTrans, CblasNoTrans, n, n, n,
                   1.0, a[0], n, a[0], n, 1.0, c[0], n );
      for(i = 0; i<n ;i++)
      {
        for(j=0;j<n; j++)
        {
          single_row_total += c[i][j];
        }
        if(single_row_total>my_max)
          my_max = single_row_total;
        single_row_total=0;
      }

gettimeofday(&tv2, &tz);

//printf(" at <%ld.%06ld>\n", (long int)(tv2.tv_sec), (long int)(tv2.tv_usec));

double elapsed = (double) (tv2.tv_sec-tv1.tv_sec) + (double) (tv2.tv_usec-tv1.tv_usec) * 1.e-6;

printf("Multiply taks %lf sec\n", elapsed);
          printf("Max norm: %f\n", my_max);


return elapsed;
}



int main(void)
{
  char file_name[40]="out_serial_matrix_one_norm.csv";
   deleteOutputFile(file_name);
   int x;
   int n;
   FILE *ofp;

   for(x=0;x<15;x++)
   {
   n = pow(2, x);
   double** A;
   double** B;
   double** C; 
   int numreps = 2;
   
   int i=0;
   int j=0;

   
   double elapsed;
   double sumTime=0;
   double average;

   
   printf ("n=%d\n", n); 
   //printf ("Please enter matrix dimension n : "); 
   //scanf("%d", &n);
   

   // allocate memory for the matrices
   
   // note that for each matrix there
   // are only two memory allocations
   
   ///////////////////// Matrix A //////////////////////////
   A =(double **)malloc(n*sizeof(double *));
	A[0] = (double *)malloc(n*n*sizeof(double));
	if(!A)
    {
      printf("memory failed \n");
      exit(1);
    }
	for(i=1; i<n; i++)
    {
      A[i] = A[0]+i*n;
      if (!A[i])
	  {
		printf("memory failed \n");
		exit(1);
	  }
    }
     ///////////////////// Matrix B //////////////////////////
  
  B =(double **)malloc(n*sizeof(double *));
  B[0] = (double *)malloc(n*n*sizeof(double));
  if(!B)
    {
      printf("memory failed \n");
      exit(1);
    }
  for(i=1; i<n; i++)
    {
      B[i] = B[0]+i*n;
      if (!B[i])
	{
	  printf("memory failed \n");
	  exit(1);
	}
    }
	
	///////////////////// Matrix C //////////////////////////
  C =(double **)malloc(n*sizeof(double *));
  C[0] = (double *)malloc(n*n*sizeof(double));
  if(!C)
    {
      printf("memory failed \n");
      exit(1);
    }
  for(i=1; i<n; i++)
    {
      C[i] = C[0]+i*n;
      if (!C[i])
	{
	  printf("memory failed \n");
	  exit(1);
	}
    } 

      
	
   // initialize the matrices
   // we can also create random matrices using the rand() method from the math.h library
   for(i=0; i<n; i++)
   {
		for(j=0; j<n; j++)
	   {
		  A[i][j] = i+j;
		  B[i][j] = i+j;
	   }
   }

  //multuply matrices
  printf("Multiply matrices %d times...\n", numreps);
  for (i=0; i<numreps; i++)
  {
   elapsed = Multiply(n,A,B,C);
   sumTime+=elapsed;
  }
   average = sumTime/numreps;
  printf("Done ...\n");
sumTime=0;
  WriteData(file_name, n, average);
   
    //deallocate memory
   free(A[0]);
   free(A);
   free(B[0]);
   free(B);
   free(C[0]);
   free(C);

   }
   return 0;
}

   

   



