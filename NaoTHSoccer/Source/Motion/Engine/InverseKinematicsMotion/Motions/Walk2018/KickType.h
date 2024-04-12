#pragma once
#include "Tools/DataStructures/Spline.h"
#include "Tools/Math/CubicSpline.h"
class KickType 
{
  private:
    tk::spline x_trajectory;
    tk::spline y_trajectory;
    tk::spline z_trajectory;
    
  public:
    KickType(
        const std::vector<double>& x_t, const std::vector<double>& x_f,
        const std::vector<double>& y_t, const std::vector<double>& y_f,
        const std::vector<double>& z_t, const std::vector<double>& z_f) 
    {
      x_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      x_trajectory.set_points(x_t, x_f);
      
      y_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      y_trajectory.set_points(y_t, y_f);
      
      z_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      z_trajectory.set_points(z_t, z_f);
    }

    KickType(const std::vector<double>& x_t, const std::vector<double>& x_f,
             const std::vector<double>& z_t, const std::vector<double>& z_f)
    {
      x_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      x_trajectory.set_points(x_t, x_f);
      
      y_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      y_trajectory.set_points({0.0, 0.5, 1.0}, {0.0, 0.0, 0.0});
      
      z_trajectory.set_boundary(tk::spline::first_deriv, 0.0, tk::spline::first_deriv, 0.0, false);
      z_trajectory.set_points(z_t, z_f);
    }

    KickType() = default;

    double getX(double v) const { return x_trajectory(v); }
    double getY(double v) const { return y_trajectory(v); }
    double getZ(double v) const { return z_trajectory(v); }
};
