#include <stdlib.h>
#include <stdio.h>

void WriteData(char *fn, FILE *fp, int n, double d)
{

   fp = fopen(fn,"a");
   fprintf(fp, "%d,%lf\n", n, d);
   fclose(fp);
}

void WriteArrayDataWithIndex(char *fn, FILE *fp, int *indexArray,double *dataArray)
{

   fp = fopen(fn,"a");
   for(i= 0;i< sizeof(*a)/sizeof(a[0]);.l)
   fprintf(fp, "%d,%lf\n", n, d);
   fclose(fp);
}

void deleteOutputFile(char *fn)
{
   int status;
   
 
   status = remove(fn);
 
   if( status == 0 )
      printf("%s file deleted successfully.\n",fn);
   else
   {
      printf("Unable to delete the file\n");
      perror("Error");
   }
}
