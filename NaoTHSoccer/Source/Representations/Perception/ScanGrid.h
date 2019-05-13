#ifndef SCANGRID_H
#define SCANGRID_H

class ScanGrid
{
public:
  void reset() {
    vertical.clear();
    horizontal.clear();
    vScanPattern.clear();
    hScanPattern.clear();
  }

  class VScanLine
  {
  public:
    int x;
    size_t top;
    size_t bottom;
  };

  std::vector<int> vScanPattern;
  std::vector<VScanLine> vertical;

  class HScanLine
  {
  public:
    int y;
    size_t left;
    size_t right;
    int skip;
  };

  std::vector<int> hScanPattern;
  std::vector<HScanLine> horizontal;
};

class ScanGridTop : public ScanGrid
{
public:
  virtual ~ScanGridTop() {}
};


#endif // SCANGRID_H
