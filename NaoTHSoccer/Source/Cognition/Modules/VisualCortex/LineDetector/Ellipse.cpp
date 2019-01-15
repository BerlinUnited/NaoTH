#include "Ellipse.h"

Ellipse::Ellipse()
{
  for(int i=0; i<6; i++) {
    params[i] = 0;
  }
}

void Ellipse::fitPoints(Eigen::VectorXd x,  Eigen::VectorXd y) {
  double mean_x = x.mean();
  double mean_y = y.mean();

  // quadratic part of the design matrix
  Eigen::MatrixXd d1(x.size(), 3);
  d1.col(0) = (x.array()-mean_x).square();
  d1.col(1) = (x.array()-mean_x) * (y.array()-mean_y);
  d1.col(2) = (y.array()-mean_y).square();

  // linear part of the design matrix
  Eigen::MatrixXd d2(x.size(), 3);
  d2.col(0) = (x.array()-mean_x);
  d2.col(1) = (y.array()-mean_y);
  d2.col(2) = Eigen::VectorXd::Constant(x.size(), 1.0);

  // quadratic part of the scatter matrix
  Eigen::Matrix3d s1 = d1.transpose() * d1;

  //combined part of the scatter matrix
  Eigen::Matrix3d s2 = d1.transpose() * d2;
  //linear part of the scatter matrix
  Eigen::Matrix3d s3 = d2.transpose() * d2;

  Eigen::Matrix3d t = -s3.inverse() * s2.transpose();


  Eigen::Matrix3d m_r = s1 + (s2 * t);

  Eigen::Matrix3d m;
  m.row(0) = m_r.col(2) * 0.5;
  m.row(1) = - m_r.col(1);
  m.row(2) = m_r.col(0) * 0.5;

  // solve eigensystem
  Eigen::EigenSolver<Eigen::MatrixXd> es(m);

  Eigen::MatrixXcd V = es.eigenvectors();


  Eigen::VectorXcd cond = (4*V.row(0).array() * V.row(2).array()) - (V.row(1).array().square());

  int max = 0;
  for(int i=1; i<3; i++) {
    if(cond(i).real() > cond(max).real()) {
      max = i;
    }
  }

  Eigen::VectorXcd a0 = V.col(max);

  Eigen::VectorXcd a(6);
  a << a0, t * a0;


  params[0] = a(0).real();
  params[1] = a(1).real();
  params[2] = a(2).real();
  params[3] = a(3).real() - 2*a(0).real()*mean_x - a(1).real()*mean_y;
  params[4] = a(4).real() - 2*a(2).real()*mean_y - a(1).real()*mean_x;
  params[5] = a(5).real()
    + a(0).real()*mean_x*mean_x
    + a(2).real()*mean_y*mean_y
    + a(1).real()*mean_x*mean_y
    - a(3).real()*mean_x
    - a(4).real()*mean_y;

  // normalize
  /*
  double norm = 0;
  for(int i=0; i<6; ++i) {
    norm += params[i] * params[i];
  }
  norm = sqrt(norm);
  for(int i=0; i<6; ++i) {
    params[i] /= norm;
  }
  */

  getCenter(center);
  axesLength(axes);

  double norm = (std::max(axes[0], axes[1]) /
      sqrt(
           std::fabs(center[0]*center[0]*params[0]
         + center[0]*center[1]*params[1]
         + center[1]*center[1]*params[2])));
  for(int i=0; i<6; ++i) {
    params[i] *= norm;
  }

  angle = rotationAngle();

  /*
  std::cout << "params =" << std::endl;
  for (int i = 0; i<6; i++)
    std::cout << params[i] << " ";
  std::cout << std::endl;
  */
}

double Ellipse::rotationAngle() {
  if (params[1]==0) {
    if (params[0]<params[2]) return 0;
    else return Math::pi_2;
  } else {
    if (params[0]<params[2]) return 0.5 * atan(params[1]/(params[0]-params[2]));
    else return Math::pi_2 + 0.5 * atan(params[1]/(params[0]-params[2]));
  }
}

void Ellipse::getCenter(double (&C)[2]) {
  double a = params[0];
  double b = params[1]/2;
  double c = params[2];
  double d = params[3]/2;
  double f = params[4]/2;

  double num = b*b - a*c;
  C[0] = (c*d-b*f) / num;
  C[1] = (a*f-b*d) / num;
}

void Ellipse::axesLength(double (&A)[2]) {
  double a = params[0];
  double b = params[1]/2;
  double c = params[2];
  double d = params[3]/2;
  double f = params[4]/2;
  double g = params[5];

  double num = 2*( a*f*f + c*d*d + g*b*b - 2*b*d*f - a*c*g );

  //sd = np.sqrt( np.power(A-C,2) + 4*np.power(B,2) )
  double sd = sqrt( (a-c)*(a-c) + 4*b*b );

  double den1 = (b*b - a*c) * (sd - (a+c));
  double den2 = (b*b - a*c) * (-sd - (a+c));

  A[0] = sqrt(num/den1);
  A[1] = sqrt(num/den2);
}
