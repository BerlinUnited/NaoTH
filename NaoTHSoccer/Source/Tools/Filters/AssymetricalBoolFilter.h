
#ifndef _AssymetricalBoolFilter_h_
#define _AssymetricalBoolFilter_h_

class AssymetricalBoolFilter
{
private:
  double g0;
  double g1;
  double state;

public:
  AssymetricalBoolFilter(double g0, double g1) : g0(g0), g1(g1), state(0.0) 
  {}

  void setParameter(double g0, double g1) {
    this->g0 = g0; this->g1 = g1;
  }

  double update(bool v) {
    state += v ? g1*(1-state) : -g0*state;
    return state;
  }

  double value() const {
    return state;
  }

  void reset() {
    state = 0;
  }
};

class AssymetricalBoolHysteresisFilter
{
private:
  bool state;
  AssymetricalBoolFilter filter;

public:
  AssymetricalBoolHysteresisFilter(double g0, double g1) 
    : 
    state(false),
    filter(g0, g1)
  {
  }

  void setParameter(double g0, double g1) {
    filter.setParameter(g0, g1);
  }

  bool update(bool v, double low, double high) {
    filter.update(v);
    state = filter.value() > (state ? low : high);
    return state;
  }

  // TODO: name state
  bool value() const {
    return state;
  }

  double floatValue() const {
    return filter.value();
  }

  void reset() {
    filter.reset();
    state = false;
  }
};


#endif // _AssymetricalBoolFilter_h_
