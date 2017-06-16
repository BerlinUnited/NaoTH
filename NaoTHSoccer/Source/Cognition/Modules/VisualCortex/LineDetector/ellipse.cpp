#include "ellipse.h"

Ellipse::Ellipse()
{
  std::cout << "Something works :D\n";

  for(int i=0; i<5; i++) {
    params[i] = 0;
  }
}

void Ellipse::fitPoints(Eigen::VectorXd x,  Eigen::VectorXd y) {
  std::cout << "x =" << std::endl << x << std::endl;
  std::cout << "y =" << std::endl << y << std::endl;

  double mean_x = x.mean();
  double mean_y = y.mean();

  // quadratic part of the design matrix
  Eigen::MatrixXd d1(x.size(), 3);
  d1.col(0) = (x.array()-mean_x).square();
  d1.col(1) = (x.array()-mean_x) * (y.array()-mean_y);
  d1.col(2) = (y.array()-mean_y).square();
  std::cout << "D1 =" << std::endl << d1 << '\n';

  // linear part of the design matrix
  Eigen::MatrixXd d2(x.size(), 3);
  d2.col(0) = (x.array()-mean_x);
  d2.col(1) = (y.array()-mean_y);
  d2.col(2) = Eigen::VectorXd::Constant(x.size(), 1.0);
  std::cout << "D2 =" << std::endl << d2 << '\n';

  // quadratic part of the scatter matrix
  Eigen::MatrixXd s1 = d1.transpose() * d1;
  std::cout << "S1 =" << std::endl << s1 << '\n';
  //combined part of the scatter matrix
  Eigen::MatrixXd s2 = d1.transpose() * d2;
  std::cout << "S2 =" << std::endl << s2 << '\n';
  //linear part of the scatter matrix
  Eigen::MatrixXd s3 = d2.transpose() * d2;
  std::cout << "S3 =" << std::endl << s3 << '\n';

  std::cout << "invS3 =" << std::endl << s3.inverse() << '\n';

  Eigen::MatrixXd t = -s3.inverse() * s2.transpose();
  std::cout << "T =" << std::endl << t << '\n';

  Eigen::MatrixXd m_r = s1 + (s2 * t);
  std::cout << "reduced M =" << std::endl << m_r << '\n';

  Eigen::MatrixXd m(3, 3);
  m.row(0) = m_r.col(2) * 0.5;
  m.row(1) = - m_r.col(1);
  m.row(2) = m_r.col(0) * 0.5;
  std::cout << "M =" << std::endl << m << '\n';

  // solve eigensystem
  Eigen::EigenSolver<Eigen::MatrixXd> es(m);
  std::cout << "The eigenvalues of A are:" << std::endl << es.eigenvalues() << std::endl;
  std::cout << "The matrix of eigenvectors, V, is:" << std::endl << es.eigenvectors() << std::endl << std::endl;

  Eigen::MatrixXcd V = es.eigenvectors();

  Eigen::VectorXcd cond = (4*V.row(0).array() * V.row(2).array()) - (V.row(1).array().square());
  std::cout << "cond =" << std::endl << cond << '\n';

  int max = 0;
  for(int i=1; i<3; i++) {
    if(cond(i).real() > cond(max).real()) {
      max = i;
    }
  }
  std::cout << "index =" << std::endl << max << '\n';

  Eigen::VectorXcd a0 = V.col(max);
  std::cout << "A0 =" << std::endl << a0 << '\n';

  Eigen::VectorXcd a(6);
  a << a0, t * a0;

  std::cout << "A =" << std::endl << a << '\n';

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

  std::cout << "params =" << std::endl;
  for (int i = 0; i<6; i++)
    std::cout << params[i] << " ";
  std::cout << std::endl;
}
