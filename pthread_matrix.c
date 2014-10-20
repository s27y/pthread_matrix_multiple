#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <pthread.h>


#define MAXTHRDS 124

typedef struct {
int my_start_row;
int my_end_row;
double **matrix_a;
double **matrix_b;
double **my_matrix;
double **global_matrix;
int my_matrix_col_len;
} matrix_multip_t;

typedef struct {
int my_column;
int my_matrix_col_len;
double my_one_norm;
double **matrix;
double *global_one_norm;
pthread_mutex_t *mutex;
} matrix_one_norm_t;

void *matrix_one_norm(void *arg)
{
  int i;
  matrix_one_norm_t *norm_data;
  norm_data = arg;
  for(i = 0; i < norm_data->my_matrix_col_len; i++) 
  {
    //printf("This is the number we want to added to my one norm %f\n",**(norm_data->matrix+ 1*(norm_data->my_matrix_col_len)+ norm_data->my_column));
    //printf("i*norm_data->my_matrix_col_len\n",i*(norm_data->my_matrix_col_len));

    norm_data->my_one_norm += abs(*(*(norm_data->matrix)+i*(norm_data->my_matrix_col_len)+ norm_data->my_column ));
    //printf("Norm%f\n",norm_data->my_one_norm);
  }
  printf("global_one_norm= %f, my_one_norm= %f\n", *(norm_data->global_one_norm),norm_data->my_one_norm);

  // update global
  pthread_mutex_lock(norm_data->mutex);
  if(*(norm_data->global_one_norm) < norm_data->my_one_norm)
    *(norm_data->global_one_norm) = norm_data->my_one_norm;

  pthread_mutex_unlock(norm_data->mutex);
}

void *matrix_multip(void *arg)
{
  int i,j,r,n;
  matrix_multip_t *matrix_data;
  matrix_data = arg;
  printf("Before dgenmm\n");

  //printf("Value of matrix_a[0][0] %f\n",(matrix_data->matrix_a[0][0]));
  n = matrix_data->my_matrix_col_len;

  cblas_dgemm( CblasRowMajor,
                CblasNoTrans, 
                CblasNoTrans, 
                1, matrix_data->my_matrix_col_len, matrix_data->my_matrix_col_len, 1.0,
                (matrix_data->matrix_a[0]), matrix_data->my_matrix_col_len, 
                (matrix_data->matrix_b[0]), matrix_data->my_matrix_col_len, 
                1.0, *matrix_data->global_matrix + matrix_data->my_start_row*n, 
                matrix_data->my_matrix_col_len );
  

  // for(i = 0; i < 1; i++) 
  //       for( j = 0; j < n; j++)
  //           {
  //             printf(" matrix_data->matrix_a[%d][%d]= %f\n",
  //               i,j,matrix_data->matrix_a[i][j]);
  //             //matrix_data->my_matrix[i][j] +=  matrix_data->matrix_a[i][r] * matrix_data->matrix_b[r][j];
  //           }

  //   for(i = 0; i < n; i++) 
  //       for( j = 0; j < n; j++)
  //           {
  //             printf(" matrix_data->matrix_b[%d][%d]= %f\n",
  //               i,j,matrix_data->matrix_b[i][j]);
  //             //matrix_data->my_matrix[i][j] +=  matrix_data->matrix_a[i][r] * matrix_data->matrix_b[r][j];
  //           }
  printf("After dgenmm\n");
  pthread_exit(NULL);
}

void malloc_matrix(double **m, int row, int column)
{
  int i;
  m =(double **)malloc(row*sizeof(double *));
  m[0] = (double *)malloc(row*column*sizeof(double));
  if(!m)
    {
      printf("memory failed \n");
      exit(1);
    }
  for(i=1; i<row; i++)
    {
      m[i] = m[0]+i*column;
      if (!m[i])
    {
    printf("memory failed \n");
    exit(1);
    }
    }
}


int main()
{
	double **A, **B, **C;
  pthread_t *working_thread;
  matrix_multip_t *thrd_matrix_multip_data;
  void *status;

	int num_of_thrds;
	int matrix_size;
	int submatrix_size;
	int i,j,n;
  time_t t;   
   /* Intializes random number generator */
  srand((unsigned) time(&t));

  double one_norm = 0.0;
  matrix_one_norm_t *thrd_matrix_one_norm_data;
  pthread_mutex_t *mutex_max_one_norm;

	  printf("Number of processors = ");
    if(scanf("%d", &num_of_thrds) < 1 || num_of_thrds > MAXTHRDS){
      printf("Check input for number of processors. Bye.\n");
      return -1;
    }

    printf("Matrix size = ");
    if(scanf("%d", &matrix_size)<1 || matrix_size%num_of_thrds != 0){
      printf("Check input for Matrix size. Bye.\n");
      return -1;
    }
    printf("After user input");
    n = matrix_size;



    submatrix_size = matrix_size/num_of_thrds;
    printf("There will be %d threads multiple two size %d matrix.\n The sub matrix size is %d * %d\n", num_of_thrds, n,submatrix_size,n);


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
      B[i][j] = i+j+10;
      printf("Value of A[%d][%d] %f\n",i,j,A[i][j]);
      printf("Value of B[%d][%d] %f\n",i,j,B[i][j]);
     }
   }


   working_thread = malloc(num_of_thrds*sizeof(pthread_t));
   thrd_matrix_multip_data = malloc(num_of_thrds*sizeof(matrix_multip_t));

   for(i=0; i<num_of_thrds; i++)
   {
    //printf("Value of A[%d] %f\n",0,**(A+submatrix_size * i));
    //double ** p = &thrd_matrix_multip_data[i].my_matrix;
    //There is warning
    thrd_matrix_multip_data[i].my_start_row = (submatrix_size * i);
    thrd_matrix_multip_data[i].my_end_row = (submatrix_size * i+submatrix_size -1);
    thrd_matrix_multip_data[i].matrix_a = A + (submatrix_size * i);
    thrd_matrix_multip_data[i].matrix_b = B;
    thrd_matrix_multip_data[i].my_matrix_col_len = n;
    thrd_matrix_multip_data[i].global_matrix = C;
    //=========
    // double *my_m = malloc (1*n*sizeof(double));
    // thrd_matrix_multip_data[i].my_matrix = &my_m;
    // printf("address of  my_matrix%x\n", &my_m);
    //global_matrix
    //=========
    //printf("p[0][0] %d\n",*p);
    
    pthread_create(&working_thread[i], NULL, matrix_multip,(void*)&thrd_matrix_multip_data[i]);
   }
   printf("This is before pthread_join\n");

   for(i=0; i<num_of_thrds; i++)
     pthread_join(working_thread[i], &status);

   printf("Done...\n");
   for(i = 0; i < 2; i++) 
        for( j = 0; j < n; j++)
            {
              printf("global_matrix[%d][%d]= %f\n",
                i,j,C[i][j]);
              //matrix_data->my_matrix[i][j] +=  matrix_data->matrix_a[i][r] * matrix_data->matrix_b[r][j];
            }

   
   
   free(working_thread);
   free(thrd_matrix_multip_data);

   

   working_thread = malloc(num_of_thrds*sizeof(pthread_t));
   thrd_matrix_one_norm_data = malloc(num_of_thrds*sizeof(matrix_one_norm_t));

   mutex_max_one_norm = malloc(sizeof(pthread_mutex_t));
   pthread_mutex_init(mutex_max_one_norm, NULL);



   for(i=0; i<num_of_thrds; i++)
   {
    thrd_matrix_one_norm_data[i].my_column =  i;
    thrd_matrix_one_norm_data[i].my_matrix_col_len =  n;
    thrd_matrix_one_norm_data[i].my_one_norm = 0;
    thrd_matrix_one_norm_data[i].matrix =  C;
    thrd_matrix_one_norm_data[i].mutex =  mutex_max_one_norm;
    thrd_matrix_one_norm_data[i].global_one_norm = &one_norm;
    pthread_create(&working_thread[i], NULL, matrix_one_norm,(void*)&thrd_matrix_one_norm_data[i]);


      printf("my_column=%d,my_matrix_col_len=%d\n",thrd_matrix_one_norm_data[i].my_column,thrd_matrix_one_norm_data[i].my_matrix_col_len);

   }
for(i = 0; i < 2; i++) 
     for(j = 0; j < 2; j++) 
            {
              printf("matrix+%d= %f\n",i,*(*C+i*2+j));
              //matrix_data->my_matrix[i][j] +=  matrix_data->matrix_a[i][r] * matrix_data->matrix_b[r][j];
            }
printf("This is the problem. \n");
  for(i=0; i<num_of_thrds; i++)
     pthread_join(working_thread[i], &status);
   printf("Max one norm = %f\n", one_norm);

free(A);
   free(B);
free(C);
pthread_mutex_destroy(mutex_max_one_norm);
free(mutex_max_one_norm);

   printf("System exit. \n");
   exit(0);

}
