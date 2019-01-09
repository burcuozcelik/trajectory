#ifndef __ONLINE_ML_H
#define __ONLINE_ML_H

#define MAX_NUM_ITERATION 35000

//#include </share/apps/python/2.7.2/include/python2.7/Python.h>
#include <Python/Python.h>
#include <fstream>
#include <sstream>


extern double err_margin;
extern int ivecid;
extern long iters;
extern long iiternum;
extern long ivpos;
extern int inumbfs;
extern int istatnum;
extern std::string bfstr; 
extern char* sol;
extern char dsname[2048];
extern std::string pkl_name;
extern ifstream base_f;
std::string fname="";
#define isclose(a, b) std::abs(a-b) <= std::max(1e-30 * std::max(std::abs(a), std::abs(b)), 0.0)

template <class Vector>
void backup_vector(Vector src, Vector dest){
  printf("here! \n");
  printf("%d\n",src.dim());
  for(int i = 0; i < src.dim(); i++){
    dest[i] = src[i];
  }
  printf("%d\n",dest.dim());
  printf("end \n");
}

template <class Vector>
void print_vector_to_file(FILE* fp, Vector v,char* vec_start, int iterpos){
  fprintf(fp,"%s;%d;",vec_start,iterpos);
  for(int i = 0; i < v.dim(); i++){
    fprintf(fp,"%0.12le;",v[i]);
  }
  fprintf(fp,"\n");
}

template <class Vector>
void print_data(Vector x5,Vector r5,Vector p5,Vector x10,Vector r10,Vector p10,Vector x20,Vector r20,Vector p20){
  fname = "/Users/mutl832/Desktop/CG_Versions/CG_versions/results/mlpredict/data/";
  fname+= std::string(sol) + '/' + std::string(dsname) + '/' + sol + '_' + std::string(dsname) + '_' + std::to_string(istatnum) + "_" + std::to_string(ivecid) + "_" + std::to_string(iiternum)+ "_" + std::to_string(ivpos) + "_" + std::to_string(inumbfs) + bfstr + ".dat";
  printf("%s\n",fname.c_str());
  FILE* fp=fopen(fname.c_str(), "w");
  if(fp == NULL){
    printf("File problem\n");
  }
  else{
    fprintf(fp,"%d\n", ivecid);
    fprintf(fp,"%d\n",iiternum);
    fprintf(fp,"%d\n",iters);
    fprintf(fp,"%0.12le\n",err_margin);
    print_vector_to_file(fp,x5,"x_vector",iiternum+5);
    print_vector_to_file(fp,r5,"r_vector",iiternum+5);
    print_vector_to_file(fp,p5,"p_vector",iiternum+5);
    print_vector_to_file(fp,x10,"x_vector",iiternum+10);
    print_vector_to_file(fp,r10,"r_vector",iiternum+10);
    print_vector_to_file(fp,p10,"p_vector",iiternum+10);
    print_vector_to_file(fp,x20,"x_vector",iiternum+20);
    print_vector_to_file(fp,r20,"r_vector",iiternum+20);
    print_vector_to_file(fp,p20,"p_vector",iiternum+20);
    fclose(fp);
  }
}

template <class Vector>
float calculate_marginal_difference(Vector avec, Vector bvec){
  float diff = 0.0;
  for(int i=0; i<avec.dim(); i++){
    if(isclose(avec[i],bvec[i]))
      continue;
    else if(isclose(avec[i],0.0)){
      if(isclose(bvec[i],0.0))
	continue;
      else
	diff += std::abs((bvec[i]-avec[i])/bvec[i]);
    }
    else
      diff += std::abs((avec[i]-bvec[i])/avec[i]);
  }
  return diff;
}

template <class Vector>
Vector calculate_data(Vector x5,Vector r5,Vector p5,Vector x10,Vector r10,Vector p10,Vector x20,Vector r20,Vector p20){  
  printf("calculate_data\n");
  Vector diffs(9, 1e15);// 0..2 x, 3..5 p, 6..8 r , 5-10-20
  int inds[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
  Vector tmp_base(x5.dim());
  std::string s,item;
  Vector *v20,*v10,*v5;
  //x -> p -> r -> x -> p ...
  for (int i=0; i<168; i++){
    std::getline(base_f, s);
    if(base_f.eof()){
      break;
    }
    if(s.find("vector") == std::string::npos){
      break;
    }  
    std::istringstream iss(s);
    std::getline(iss, item, ';');
    string vecname = item;
    std::getline(iss, item, ';');
    int curitr = std::stoi(item);
 
    //get curitr base vector 
    int vecindex = 0;
    while (std::getline(iss, item, ';')){
      tmp_base[vecindex] = std::stof(item);
      vecindex++;
    }
    int diffindex=-1;
    if(vecname == "x_vector"){
      v20 = &x20;
      v10 = &x10;
      v5 = &x5;
      diffindex = 0;
    }
    else if (vecname == "p_vector"){
      v20 = &p20;
      v10 = &p10;
      v5 = &p5;
      diffindex = 3;
    }
    else if (vecname == "r_vector"){
      v20 = &r20;
      v10 = &r10;
      v5 = &r5;
      diffindex = 6;
    }
    //    printf("%1.16f\n",tmp_base[0]);
    
    float cur_diff;
    if(curitr >= iiternum && curitr <= iiternum + 40){
      //(xpr)20
      //      printf("20 %s did:%d curitr:%i ", vecname.c_str(), diffindex, curitr);
      cur_diff = calculate_marginal_difference(*v20,tmp_base);
      //printf("curdiff returned:%1.16f, overall diff: %1.16f\n", cur_diff, diffs[diffindex+2]);
      if( cur_diff < diffs[diffindex+2] ){
	inds[diffindex+2] = curitr;
	diffs[diffindex+2] = cur_diff;
	//	printf("inds[%d]=%d is it? %f\n",diffindex+2,curitr,inds[diffindex+2]);
      }
      //printf("overall diff: %1.16f \n", diffs[diffindex+2]);
    } 
    if(curitr >= iiternum-10 && curitr <= iiternum + 30){
      //(xpr)10
      cur_diff = calculate_marginal_difference(*v10,tmp_base);
      if(cur_diff < diffs[diffindex+1]){
	inds[diffindex+1] = curitr;
	diffs[diffindex+1] = cur_diff;
      }
    }
    if(curitr >= iiternum-5 && curitr <= iiternum + 25){
      //(xpr)x5
      cur_diff = calculate_marginal_difference(*v5,tmp_base);
      if(cur_diff < diffs[diffindex]){
	inds[diffindex] = curitr;
	diffs[diffindex] = cur_diff;
      }
    }
    if(curitr > iiternum + 40)
      break;    
  }
  //  printf("%d,%d,%d,%d,%d,%d,%d,%d,%d\n", inds[0],inds[1],inds[2],inds[3],inds[4],inds[5],inds[6],inds[7],inds[8]);
  //printf("%1.16f,%1.16f,%1.16f,%1.16f,%1.16f,%1.16f,%1.16f,%1.16f,%1.16f\n",diffs[0],diffs[1],diffs[2],diffs[3],diffs[4],diffs[5],diffs[6],diffs[7],diffs[8]);
  return diffs;
}


template <class Vector>
int ml_predict(Vector x5,Vector r5,Vector p5,Vector x10,Vector r10,Vector p10,Vector x20,Vector r20,Vector p20){
  print_data(x5,r5,p5,x10,r10,p10,x20,r20,p20);
  //instead of print data call calculate data
  Vector diffs = calculate_data(x5,r5,p5,x10,r10,p10,x20,r20,p20);
  
  PyObject *pName, *pModule, *pDict, *pFunc, *pValue, *pArgs;
  // Initialize the Python Interpreter
  Py_Initialize();
  PyRun_SimpleString("import sys\n"
		     "sys.path.insert(0, '/Users/mutl832/Desktop/CG_Versions/CG_versions/inc')");
  // Build the name object
  pName = PyString_FromString((char*)"ml_predict");

  // Load the module object
  pModule = PyImport_Import(pName);

  if (pModule != NULL) {
    pFunc = PyObject_GetAttrString(pModule, "predict");    
  }
  else{
    PyErr_Print();
    fprintf(stderr, "Failed to load \"%s\"\n", "mlpredict");
    return 1;
  }

  // pDict is a borrowed reference 
  pDict = PyModule_GetDict(pModule);

  // pFunc is also a borrowed reference 
  pFunc = PyDict_GetItemString(pDict, "predict");
  
  struct timeval pt0,pt1;
  if (PyCallable_Check(pFunc)) 
    {
      pArgs = PyTuple_New(2);
      PyTuple_SetItem(pArgs, 0, PyString_FromString(fname.c_str())); 
      PyTuple_SetItem(pArgs, 1, PyString_FromString(pkl_name.c_str()));
      //send the values calculated 
      //PyTuple_SetItem(pArgs, 2, PyFloat_FromDouble(diffs[0]));
      //PyTuple_SetItem(pArgs, 3, PyFloat_FromDouble(diffs[1]));
      //PyTuple_SetItem(pArgs, 4, PyFloat_FromDouble(diffs[2]));
      //PyTuple_SetItem(pArgs, 5, PyFloat_FromDouble(x10m));
      //PyTuple_SetItem(pArgs, 6, PyFloat_FromDouble(p10m));
      //PyTuple_SetItem(pArgs, 7, PyFloat_FromDouble(r10m));
      //PyTuple_SetItem(pArgs, 8, PyFloat_FromDouble(x20m));
      //PyTuple_SetItem(pArgs, 9, PyFloat_FromDouble(p20m));
      //PyTuple_SetItem(pArgs, 10, PyFloat_FromDouble(r20m));      
      //PyTuple_SetItem(pArgs, 11, PyFloat_FromDouble(err_margin));
      //PyTuple_SetItem(pArgs, 12, PyFloat_FromDouble(itr_per));

      gettimeofday(&pt0, 0);
      pValue = PyObject_CallObject(pFunc, pArgs);
      //PyObject_CallObject(pFunc, pArgs);
      gettimeofday(&pt1, 0);
      long elapsedpython  = (pt1.tv_sec-pt0.tv_sec) * 1e6 + pt1.tv_usec - pt0.tv_usec;
      printf("Python time: %lld\n",elapsedpython);
      printf("Prediction: %ld\n", PyInt_AsLong(pValue));
    } else 
    {
      PyErr_Print();
    }

  // Clean up
  Py_DECREF(pModule);
  Py_DECREF(pName);

  // Finish the Python Interpreter
  Py_Finalize();
  return 0;
}

#endif 
