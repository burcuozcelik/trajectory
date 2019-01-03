#ifndef _GAGAN_DETECTORS_H
#define _GAGAN_DETECTORS_H

#include <stdio.h>
#include <queue>

#ifndef GLOBAL
#define GLOBAL
class Global {
 public:
  static int DMAX_ITER;
  static int windowSize;
  static queue<double> window;
  static double threshold;
  static double gTrend;
  static queue<double> diffs;
  static queue<double> trend;
};


int Global::DMAX_ITER = 0;                                                                                                            
int Global::windowSize = 0;                                                                                                          
queue<double> Global::window;                                                                                                        
double Global::threshold = 0;
double Global::gTrend = 0;                                                                                                          
queue<double> Global::diffs;                                                                                                         
queue<double> Global::trend;

#endif

void SDC_Gagan_Init(int iter, int winSize, double thres) {
  Global::DMAX_ITER = iter;
  Global::windowSize = winSize;
  Global::threshold = thres;
}

void SDC_Gagan_Finalize() {
  cout << "[GAGAN] windowSize: " << Global::windowSize << " maxIter: " << Global::DMAX_ITER << " threshold: " << Global::threshold << " queue size: " << Global::window.size() << " gTrend: " << Global::gTrend << endl;
}

bool SDC_Gagan_Snapshot(double curRes){
  //printf("GOKCEN-Snapshot:%0.12le\n",curRes);   
  int size = Global::windowSize;
  double& gTrend = Global::gTrend;
  queue<double>& res = Global::window;
  queue<double>& diffs = Global::diffs;
  queue<double>& trend = Global::trend;
  double thres = Global::threshold;

  if (res.size() == 0) {
    return false;
  } else if (res.size() < size) {
    return false;
  } else {
    if (gTrend < 0) {
      // residual is increasing                                                                                                     
      return true;
    }
    double prevRes = res.back();
    double curDiff = prevRes - curRes;
    double curTrend = curDiff / prevRes;
    return curTrend < 0 && -curTrend > gTrend + thres;
  }
  return false;
}

void SDC_Gagan_SetMark(double curRes) {
  //printf("GOKCEN-SetMark:%0.12le\n",curRes);
  int size = Global::windowSize;
  double& gTrend = Global::gTrend;
  queue<double>& res = Global::window;
  queue<double>& diffs = Global::diffs;
  queue<double>& trend = Global::trend;
  /*
    std::cout << "res: " << res.size() << '\n';
    std::cout << "diffs: " << diffs.size() <<  '\n';
    std::cout << "trend: " << trend.size() << '\n';
  */
  if (res.size() == 0) {
    res.push(curRes);
  } else if (res.size() < size) {
    double prevRes = res.back();
    res.push(curRes);
    double curDiff = prevRes - curRes;
    double curTrend = curDiff / prevRes;
    trend.push(curTrend);
    gTrend = (gTrend * (trend.size() - 1) + curTrend) / trend.size();
    diffs.push(curDiff);
  } else {
    assert(res.size() > 0);
    res.pop();
    
    assert(diffs.size() > 0);
    diffs.pop();
    
    assert(trend.size() > 0);
    trend.pop();
    
    double prevRes = res.back();

    res.push(curRes);
    
    double curDiff = prevRes - curRes;
    double dumpTrend = trend.front();
    double curTrend = curDiff / prevRes;
    trend.push(curTrend);
    gTrend = (gTrend * trend.size() - dumpTrend + curTrend) / trend.size();
    diffs.push(curDiff);
  }
}

#endif
