#ifndef SCANGRID_H
#define SCANGRID_H

class ScanGrid
{
public:
  void reset() {
    vScanPattern.clear();
    hScanPattern.clear();
    longverticals.clear();
    // vertical is resized in ScanGridProvider
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

  // for field detection
  std::vector<int> longverticals;
};

class ScanGridTop : public ScanGrid
{
public:
  virtual ~ScanGridTop() {}
};


#endif // SCANGRID_H
