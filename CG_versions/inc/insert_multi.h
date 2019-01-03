#ifndef __INSERT_H
#define __INSERT_H
#ifdef INSERT_MULTIBF_FAULT
extern "C" {
#ifdef INSERT_MULTIBF_FAULT


#define create_label(x, y) x ## y
#define create_labelu(x, y) create_label(x, y)
#define isclose(a, b) std::abs(a-b) <= std::max(1e-30 * std::max(std::abs(a), std::abs(b)), 0.0)
  //#define isclose(a, b, rel_tol, abs_tol) std::abs(a-b) <= max(rel_tol * max(std::abs(a), std::abs(b)), abs_tol)
                                                                                                                                                        


#define marg_difference(a,b){				\
    if (isclose(a,b)){					\
	err_margin=0.0;					\
      }							\
    else{						\
      if (isclose(a,0.0)){				\
	  err_margin = std::abs((b-a)/b);		\
	}						\
      else if (isclose(b,0.0)){				\
	  err_margin=std::abs((a-b)/a);			\
	}						\
      else						\
	err_margin=std::abs((a-b)/a);			\
    }							\
    printf("err_margin: %.*f\n",15,err_margin);		\
  }							\
  
union fltype {long unsigned int bytes; double value;};
  #define insert_fault(x, curr_it){					\
  fltype magnitude;							\
  double mag2;								\
  if(isinsert == 1) { 							\
  if (curr_it == iiternum) {						\
    magnitude.value  = x[ivpos];					\
    mag2 = x[ivpos];							\
    for (int im=0; im < inumbfs; im++){					\
      magnitude.bytes ^= 1UL << bfposes[im];				\
    }				\    
  x[ivpos]= magnitude.value;						\
  marg_difference(mag2,x[ivpos]);					\
  printf("Fault inserted\n");						\
}									\
} 									\
create_labelu(END, __LINE__):						\
			    ;						\
}

#define insert_fault_vector(vecs,curr_line,curr_it) { \
    if( isinsert == 1 && curr_it == iiternum && curr_line == istatnum){	\
      gettimeofday(&tinj, 0);						\
      Vector ivec = *vecs[ivecid];					\
      insert_fault(ivec,curr_it);					\
      *vecs[ivecid] = ivec;                                             \
    }									\
  }									\
    
extern double ilimit; //Limit after activation
extern long iters;
extern  int isinsert;
extern  long iiternum;
extern  long ivpos;
extern  int inumbfs;
extern  int bfposes[64];
extern double err_margin;
extern struct timeval tinj;

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
