#ifndef PATTERN_H
#define PATTERN_H

#include <Tools/Math/Vector2.h>
#include <vector>
#include <set>
#include <Tools/Math/Common.h>

using namespace std;

class Pattern
{
public:
    vector<Vector2d> pattern;

//    Pattern(unsigned int numberOfPoints,unsigned int w, unsigned int h){
//        Vector2< double> point;
//        set<Vector2<double> > points;

//        srand((unsigned int)time(0));

//        while(pattern.size()<numberOfPoints){
//            point.x = w * (double)rand()/(double)RAND_MAX-w/2.0;
//            point.y = h * (double)rand()/(double)RAND_MAX-h/2.0;
//            points.insert(point);
//        }

//        for(set<Vector2<double> >::iterator it=points.begin(); it!=points.end();it++){
//            pattern.push_back(*it);
//        }

//    }

    Pattern(unsigned int vertical,unsigned int horizontal,double w, double h, bool combs)
    {
        Vector2d point(-w/2.0,h/2.0);
        Vector2d temp;
        // vertical & horizontal !=1
        double ygap=h/(vertical-1);
        double xgap=w/(horizontal-1);


        double verticalDriftFactor = 10;
        double horizontalDriftFactor = 10;

        double verticalDrift=verticalDriftFactor/(horizontal-1);
        double horizontalDrift=horizontalDriftFactor/(vertical-1);

        if(!combs){
            for(unsigned int i=0;i<vertical;i++){
                temp=point;

                for(unsigned int j=0;j<horizontal;j++){
                    pattern.push_back(temp);
                    temp.x+=xgap;
                }//end for

                point.y-=ygap;
            }//end for
        } else {
            bool evenLine=false;

            for(unsigned int i=0;i<vertical;i++){
                temp=point;

                if(evenLine){
                    temp.x+=xgap/2.0;
                    for(unsigned int j=0;j<horizontal-1;j++){
                        pattern.push_back(temp);
                        temp.x+=xgap;
                        temp.y-=verticalDrift;
                    }
                    evenLine=false;
                } else {
                    for(unsigned int j=0;j<horizontal;j++){//2;j++){
                        pattern.push_back(temp);
                        temp.x+=xgap;
                        temp.y-=verticalDrift;
                    }//end for
                    evenLine=true;
                }//end else

                point.x+=horizontalDrift;
                point.y-=ygap;
            }//end for
        }
    }

    Pattern(double alpha)
    {
        pattern.push_back(Vector2d(0,0));

        double alphaInRad=Math::fromDegrees(alpha);

        double numberOfRotations = 360/alpha;

        Vector2d shift(250,0);
        // in 22.5° Schritten einmal rum, halber Einflussradius
        for(int i = 0; i<numberOfRotations;i++){
            pattern.push_back(shift);
            shift.rotate(alphaInRad);
        }
    }

    void rotate(double angle)
    {
      for(vector<Vector2d>::iterator it=pattern.begin();it!=pattern.end();it++){
        (*it).rotate(angle);
      }
    }

    struct CompareSites : public std::binary_function<Vector2d, Vector2d, bool>
    {
      bool operator()(const Vector2d& l, const Vector2d& r) const 
      {
        if (l.y < r.y){
          return true;
        }

        if ( (l.y == r.y) && (l.x < r.x)){
          return true;
        }

        return false; 
      }
    };
};


#endif // PATTERN_H
