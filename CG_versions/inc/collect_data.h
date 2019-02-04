#ifndef __COLLECT_DATA_H
#define __COLLECT_DATA_H

#define MAX_NUM_ITERATION 60000
FILE *fp = NULL;

void dump(double * r, int max_iter){
  
  for(int i = 0; i < max_iter; i++){
    printf("%0.12le;",r[i]);
  }
  printf("\n");
 

  /* Write to binary file*/
  if (fp == NULL){
    fp=fopen("test", "wb");
    fwrite(&max_iter, sizeof(int), 1, fp); //length of the array
  }

  fwrite(r, sizeof(double), max_iter, fp); // data

  // rewind(fp);
  //fread(testa, sizeof(double), max_iter, fp);
  // printf("r%0.12le;\n",r[0]);
  //printf("testa%0.12le;\n",testa[0]);
}


void collect_residual(double resid, int iter, double* resids){
  resids[iter] = resid;
}

void collect_expressions(double exp, int iter, double* vec_exps){
  vec_exps[iter] = exp;
}

template <class Vector> 
void dump_residual_vector(Vector r, int iter){
  /*
    printf("RESIDUAL_VEC;%d;",iter);
    for(int i = 0; i < r.dim(); i++){
    printf("%0.12le;",r[i]);
    }
    printf("\n");
  */
}

template <class Vector> 
void dump_vector(Vector r, int iter,char * text){
 
    printf("%s;%d;",text,iter);
    for(int i = 0; i < r.dim(); i++){
    printf("%0.12le;",r[i]);
    }
    printf("\n");
}

template <class Vector> 
void dump_vector_at_itr(Vector r, int iter,char * text){
 
  if(iter == iiternum || iiternum == -1){
    printf("%s;%d;",text,iter);
    for(int i = 0; i < r.dim(); i++){
      printf("%0.12le;",r[i]);
    }
    printf("\n");
  }
}

void dump_residuals(double * res, int max_iter){
  printf("RESIDUAL_NORM;");
  dump(res, max_iter);
}

void dump_orth_ex1(double * exp1, int max_iter){
  printf("ORTH_EX1;");
  dump(exp1, max_iter);
}

void dump_orth_ex2(double * exp2, int max_iter){
  printf("ORTH_EX2;");
  dump(exp2, max_iter);
}

void dump_newsum_ex1(double * exp1, int max_iter){
  printf("NEWSUM_EX1;");
  dump(exp1, max_iter);
}

void dump_newsum_ex2(double * exp2, int max_iter){
  printf("NEWSUM_EX2;");
  dump(exp2, max_iter);
}


void dump_newsum_scaler(double * scaler, int max_iter){
  printf("NEWSUM_SCALER;");
  dump(scaler, max_iter);
}

void dump_collection(double * coll, int max_iter,char * text)
{
  printf("%s\n", text);
  dump(coll,max_iter);
}

#endif
