#include "Tools/Math/Polygon.h"
#include "iostream"

using namespace Math;

int main(){
      // square
      std::vector<Vector2d> points = {
        {-1, 1},
        { 1, 1},
        { 1,-1},
        {-1,-1}
      };

      Polygon<double> square(points);
      std::cout << "Area: " << square.getArea() << std::endl;
      std::cout << "Inside: " << square.isInside({-0.5,0.5}) << std::endl;
      std::cout << "Not inside: " << square.isInside({-5,5}) << std::endl;

      Polygon<double> empty;
      std::cout << "Area: " << empty.getArea() << std::endl;
      std::cout << "Not Inside: " << empty.isInside({-0.5,0.5}) << std::endl;
      std::cout << "Not inside: " << empty.isInside({-5,5}) << std::endl;

      Polygon<double> two({{-1,-1}, {1, 1}});
      std::cout << "Area: " << two.getArea() << std::endl;
      std::cout << "Not Inside: " << two.isInside({-0.5,0.5}) << std::endl;
      std::cout << "Not inside: " << two.isInside({-5,5}) << std::endl;
}
