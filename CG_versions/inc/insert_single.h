#ifndef __INSERT_H
#define __INSERT_H
#ifdef INSERT_SINGLEBF_FAULT
extern "C" {
#ifdef INSERT_SINGLEBF_FAULT
/*******/
union fltype {long unsigned int bytes; double value;};
#define create_label(x, y) x ## y
#define create_labelu(x, y) create_label(x, y)

#define insert_fault(x, curr_it, pprob)             \
  { long iteration, index, elesize = x.dim();	\
   double pnum;                          \
   fltype magnitude; \
   switch (ptime){                                  \
      case 0:                                       \
         goto create_labelu(END, __LINE__);                                  \
         break;                                     \
      case 1:                                       \
         iteration = 1;                             \
         break;                                     \
      case 2:                                       \
         iteration = (long)(iters * 0.5);           \
         break;                                     \
      case 3:                                       \
         iteration = iters - 1;                     \
         break;                                     \
      default:                                      \
         goto create_labelu(END, __LINE__);                                  \
         break;                                     \
   }                                                \
   if(ppos < 0.0 || ppos > 1.0) goto create_labelu( END, __LINE__);           \
   if(pprob < 0.0 || pprob > 1.0) goto create_labelu(END, __LINE__);         \
   if(pmag < 0.0) goto create_labelu(END, __LINE__);                         \
   index = (long)(elesize * (ppos));                \
   pnum = rand() / (double)(RAND_MAX);              \
/*   magnitude = pmag * x[index];                    */ \
   magnitude.value = x[index]; \
   /*printf("Old:%0.12le\n", magnitude.value);*/	\
   /*magnitude.bytes ^= pmag;		     */	\
   magnitude.bytes ^= 1UL << pmag;                      \
   /*printf("New:%0.12le\n", magnitude.value);	*/	\
   if(curr_it == iteration && pnum < pprob){        \
     printf("Inserted in iteration: %d, ptime: %d; ppos: %f; pmag: %d %0.12le %0.12le\n", curr_it, ptime, ppos, pmag, x[index], magnitude.value); \
      /*x[index] += magnitude;*/ x[index] = magnitude.value;                        \
      inserted ++;                                  \
   }                                                \
   create_labelu(END, __LINE__):                                             \
   ;                                                \
}                                                                                
   

extern long iters;
extern long ptime;
extern double ppos;
extern long pmag;
extern long inserted;
long load_tot_iters(char *bin, const char dataset[]);
void print_params();
#else
#define insert_fault(x, curr_it, pprob)
#endif
};
#else
#define insert_fault(x, cit, pprob) ;
#endif
#endif
