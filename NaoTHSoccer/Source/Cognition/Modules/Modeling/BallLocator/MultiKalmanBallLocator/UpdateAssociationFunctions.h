#ifndef UPDATEASSOCIATIONFUNCTIONS
#define UPDATEASSOCIATIONFUNCTIONS

//#include "MeasurementFunctions.h"
#include "BallHypothesis.h"
#include <Tools/Math/Common.h>

struct UpdateAssociationFunction{
    protected:
        std::vector<BallHypothesis>::iterator bestPredictor;
        double score;
        double threshold;

    public:
        std::vector<BallHypothesis>::iterator getBestPredictor() const
        {
            return bestPredictor;
        }

        double getScore() const {
            return score;
        }

        void setThreshold(double new_threshold) {
            threshold = new_threshold;
        }

        double getThreshold() const {
          return threshold;
        }

        virtual void determineBestPredictor(std::vector<BallHypothesis>& filter, const Eigen::Vector2d& z, const Measurement_Function_H& h){
            score = initialValue();
            bestPredictor = filter.begin();

            for(std::vector<BallHypothesis>::iterator iter = filter.begin(); iter != filter.end(); ++iter)
            {
                double temp = associationScore(*iter,z,h);

                if(better(temp,score)) {
                    score = temp;
                    bestPredictor = iter;
                }
            }
        }

        virtual double associationScore(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z, const Measurement_Function_H& h) const = 0;

        virtual bool better(double value1 ,double value2) const
        {
            return value1 < value2;
        }

        virtual double initialValue() const
        {
            return std::numeric_limits<double>::infinity();
        }

        virtual bool inRange() const {
            return score < threshold;
        }
};

struct EuclideanUAF : UpdateAssociationFunction
{
    double associationScore(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z, const Measurement_Function_H& h) const
    {
        Eigen::Vector2d diff = z-filter.getStateInMeasurementSpace(h);
        return std::sqrt((diff.transpose() * Eigen::Matrix2d::Identity() * diff)(0,0));
    }
};

struct MahalanobisUAF : UpdateAssociationFunction
{
    double associationScore(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z, const Measurement_Function_H& h) const
    {
        Eigen::Vector2d diff = z-filter.getStateInMeasurementSpace(h);

        Eigen::Matrix2d covariance = filter.getStateCovarianceInMeasurementSpace(h);

        return std::sqrt((diff.transpose() * covariance.inverse() * diff)(0,0));
    }
};

struct LikelihoodUAF : UpdateAssociationFunction
{
    double associationScore(const ExtendedKalmanFilter4d& filter, const Eigen::Vector2d& z, const Measurement_Function_H& h) const
    {
        Eigen::Vector2d diff = z-filter.getStateInMeasurementSpace(h);

        Eigen::Matrix2d covariance = filter.getStateCovarianceInMeasurementSpace(h);

        double exponent = -0.5*(diff.transpose() * covariance.inverse() * diff)(0,0);
        double factor   = 1.0 / (Math::pi2 * std::sqrt(covariance.determinant()));

        return factor * std::exp(exponent);
    }

    bool better(double value1, double value2) const
    {
        return value1 > value2;
    }

    double initialValue() const
    {
        return 0;
    }

    bool inRange() const {
        return score > threshold;
    }
};

#endif // UPDATEASSOCIATIONFUNCTIONS

