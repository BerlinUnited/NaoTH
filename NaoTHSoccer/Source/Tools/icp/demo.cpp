/*
Copyright 2011. All rights reserved.
Institute of Measurement and Control Systems
Karlsruhe Institute of Technology, Germany

This file is part of libicp.
Authors: Andreas Geiger

libicp is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 3 of the License, or any later version.

libicp is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
libicp; if not, write to the Free Software Foundation, Inc., 51 Franklin
Street, Fifth Floor, Boston, MA 02110-1301, USA 
*/

// Demo program showing how libicp can be used

#include <iostream>
#include "icpPointToPlane.h"
#include <vector>

using namespace std;

int main (int argc, char** argv) {

  // define a 3 dim problem with 10000 model points
  // and 10000 template points:
  int32_t dim = 2;
  int32_t M_num = 0, T_num = 0;

  // allocate model and template memory
  std::vector<double> M;
  std::vector<double> T;

  // set model and template points
  cout << endl << "Creating model with 10000 points ..." << endl;
  cout << "Creating template by shifting model by (1,1,1) ..." << endl;

  for (double x=-2; x<2; x+=0.02) 
  {
    double y = 5*x*exp(-x*x);
    M.push_back(x);
    M.push_back(y);
    M_num++;
  }

  double a = 0.01;
  std::cout << cos(a) << "   --   " << sin(a) << std::endl;

  for (double x=-2; x<2; x+=0.04) 
  {
    double y = 5*x*exp(-x*x);

    double x0 = cos(a)*x - sin(a)*y - 1;
    double y0 = sin(a)*x + cos(a)*y - 1;

    T.push_back(x0);
    T.push_back(y0);
    T_num++;
  }

  // start with identity as initial transformation
  // in practice you might want to use some kind of prediction here
  Matrix R = Matrix::eye(dim);
  Matrix t(dim,1);

  // run point-to-plane ICP (-1 = no outlier threshold)
  cout << endl << "Running ICP (point-to-plane, no outliers)" << endl;
  IcpPointToPlane icp(M.data(),M_num,dim);
  icp.fit(T.data(),T_num,R,t,-1);

  // results
  cout << endl << "Transformation results:" << endl;
  cout << "R:" << endl << R << endl << endl;
  cout << "t:" << endl << t << endl << endl;

  // success
  return 0;
}

