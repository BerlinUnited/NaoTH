#include "Minimizer.h"

GaussNewtonMinimizer::GaussNewtonMinimizer()
{

}

Vector2d GaussNewtonMinimizer::determineJacobian(ErrorFunction* errorFunction, double epsilon){
    // approximate the derivative Dg(x)=(dg1, dg2) of g at point x=(y,p)
    double dg11 = (*errorFunction)( epsilon, 0);
    double dg12 = (*errorFunction)(-epsilon, 0);

    double dg21 = (*errorFunction)(0,  epsilon);
    double dg22 = (*errorFunction)(0, -epsilon);

    double dg1  = (dg11 - dg12) / (2 * epsilon);
    double dg2  = (dg21 - dg22) / (2 * epsilon);

    return Vector2d(dg1,dg2);
}

Vector2d GaussNewtonMinimizer::minimizeOneStep(ErrorFunction* errorFunction, double epsilon){

    Vector2d offset;

    // Dg(x)^T*Dg(x)
    Vector2d dg = determineJacobian(errorFunction, epsilon);

    // g(x) - target
    double w = (*errorFunction)(offset.x, offset.y);

    //Vector2<double> z_GN = (-((Dg.transpose()*Dg).invert()*Dg.transpose()*w));
    Vector2d z_GN = dg * (-w / (dg * dg));
    offset += z_GN;

    double lambda = 0.005;
    //MODIFY("CameraMatrixCorrectorV2:lambda", lambda);
    if (offset.abs() > lambda) {
        offset.normalize(lambda);
    }

    return offset;
}
