#ifdef INSERT_SINGLEBF_FAULT
#include "insert_single.h"
#elif defined(INSERT_RANDOM_FAULT)
#include "insert_random.h"
#else
#include "insert_multi.h"
#endif 

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long inserted = 0;
extern long liter;
extern int isinsert;
extern long iiternum;
extern long ivpos;
extern int inumbfs;
extern int bfposes[64];
extern int istatnum;
extern int ivecid;

long load_tot_iters(char *bin, const char dataset[]){
   long res;
   char method[2048];
   char ldataset[2048];
   if(bin == NULL || dataset == NULL){
      return -1; 
   }

   FILE *fp = fopen(bin, "rb+");
   if(fp == NULL) return -2;
   while(fscanf(fp, "%[^,]%*c%[^,]%*c%ld", method, ldataset, &res)!= EOF){
      if(strcmp(ldataset, dataset) == 0){
         break;
      }
      res = -3;
   }
   return res;
}

unsigned long load_rand_num(long ptime, float index, long pmag){
   unsigned long res;
   char file[2048];

   sprintf(file, "nums/rand_%d_%.1f_%d.rd", ptime, index, pmag);

   FILE *fp = fopen(file, "rb+");
   if(fp == NULL){
       printf ("Cannot open file: %s\n", file);
       exit(1);
   };
   fscanf(fp, "%Ld", &res);
   //printf("Random number loaded: %Ld\n", res);
   return res;
}

void print_params(){
#if defined(INSERT_SINGLEBF_FAULT) || defined(INSERT_RANDOM_FAULT)
   printf("Baseline iterations: %ld\n", iters);
   printf("Place in iterations space (when): %ld\n", ptime);
   printf("Place in vector (where): %.2lf\n", ppos * 100);
   printf("Value to be altered by: %.2lf\n", pmag);
   printf("Number of faults that were inserted: %ld\n", inserted);
   printf("Iteration Check: %s\n", iters==liter?"PASSED":"FAILED");     
#endif

#if defined(INSERT_MULTIBF_FAULT)
   printf("Baseline iterations: %ld\n", iters);
   printf("Iteration Check: %s\n", iters==liter?"PASSED":"ANOMALIES");
   if (liter > 2 * iters){
     printf("Iteration Check: %s\n", "ADVERB");
   }

   if (isinsert == 1){
     printf("Place in iterations space (when): %ld\n", iiternum);
     printf("Place in vector (where): %ld\n", ivpos);
     printf("Place among statements: %ld\n", istatnum);
     printf("Place among vectors: %ld\n", ivecid);
     printf("Bit flips: ");
     for (int i=0; i < inumbfs; i++){
       printf("%d ", bfposes[i]);
     }
     printf("\n");
    }
#endif

}
