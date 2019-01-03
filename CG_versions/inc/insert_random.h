#ifndef __INSERT_RANDOM_H
#define __INSERT_RANDOM_H
#ifdef INSERT_RANDOM_FAULT

#include <stdint.h> 
extern "C" {
#ifdef INSERT_RANDOM_FAULT
#define create_label(x, y) x ## y
#define create_labelu(x, y) create_label(x, y)
  
#define MANTISA 0xFFFFFFFFFFFFF
  
#define insert_fault(x, curr_it, pprob)		     \
  {  long iteration, index, elesize = x.dim();	     \
    double pnum;				     \
    long part= iters / 5;			     \
    switch (ptime){				     \
    case 0:								\
      goto create_labelu(END, __LINE__);				\
      break;								\
    case 1:								\
      iteration = 1;							\
      break;								\
      case 2:                                       \
	iteration = (long)(part * 2);		    \
	break;					    \
    case 3:					    \
      iteration = (long)(part * 3);		    \
      break;					    \
    case 4:					    \
      iteration = (long)(part * 4);		    \
      break;					    \
    case 5:					    \
      iteration = iters - 1;			    \
      break;								\
    default:								\
      goto create_labelu(END, __LINE__);				\
      break;								\
    }									\
    if(ppos < 0.0 || ppos > 1.0) goto create_labelu( END, __LINE__);	\
    if(pprob < 0.0 || pprob > 1.0) goto create_labelu(END, __LINE__);	\
    if(pmag < 0.0) goto create_labelu(END, __LINE__);			\
    index = (long)(elesize * (ppos));					\
    if(curr_it == iteration){						\
      printf("Random Number Inserted\n");				\
      printf("Inserted in iteration: %d\n", curr_it);			\
      unsigned long long rnum_64= (long long)load_rand_num(ptime, ppos, pmag);		\
      /*printf("rnum: %llu\n", rnum_64);*/				\
      rnum_64 = rnum_64 & MANTISA;					\
      unsigned long long prev_val= *(unsigned long long*) &x[index];	\
      /*printf("prev: %llu\n", prev_val);	*/			\
      prev_val = prev_val & ~MANTISA;					\
      unsigned long long new_val = rnum_64 | prev_val;			\
      /*printf("new_val: %llu\n",new_val); */				\
      x[index] = *(double*)&new_val;					\
      inserted ++;							\
   }									\
  create_labelu(END, __LINE__):						\
    ;									\
  }
  
  extern long iters;
  extern long ptime;
  extern double ppos;
  extern long pmag;
  extern long inserted;
  long load_tot_iters(char *bin, const char dataset[]);
  unsigned long load_rand_num(long ptime, float index, long pmag);
  void print_params();
  extern uint64_t rand_uint64(void);
#else
#define insert_fault(x, curr_it, pprob)
#endif
};
#else
#define insert_fault(x, cit, pprob) ;
#endif
#endif
