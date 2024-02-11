#include <stdio.h>
#include <stdlib.h>

int main()
{
   int num = 4242;
   FILE *fptr;

   // use appropriate location if you are using MacOS or Linux
   fptr = fopen("test.txt","w");

   if(fptr == NULL)
   {
      printf("Error!");   
      exit(1);             
   }

   printf("Enter num: ");

   fprintf(fptr,"%d",num);
   fclose(fptr);

   return 0;
}
