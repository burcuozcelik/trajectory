#ifndef __ONLINE_ML_H
#define __ONLINE_ML_H

#define MAX_NUM_ITERATION 35000

//#include </share/apps/python/2.7.2/include/python2.7/Python.h>
#include <Python/Python.h>

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
std::string fname="";

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
int ml_predict(Vector x5,Vector r5,Vector p5,Vector x10,Vector r10,Vector p10,Vector x20,Vector r20,Vector p20){
  print_data(x5,r5,p5,x10,r10,p10,x20,r20,p20);
  //instead of print data call calculate data 
  
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
      //PyTuple_SetItem(pArgs, 2, PyFloat_FromDouble(x5m));
      //PyTuple_SetItem(pArgs, 3, PyFloat_FromDouble(p5m));
      //PyTuple_SetItem(pArgs, 4, PyFloat_FromDouble(r5m));
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
