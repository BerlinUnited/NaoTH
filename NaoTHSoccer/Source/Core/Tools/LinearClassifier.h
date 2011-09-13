/**
* @file LinearClassifier.h
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implentation of the Linear Classifier
*/

#ifndef _LINEARCLASSIFIER_H
#define  _LINEARCLASSIFIER_H

#include <iostream>
#include <vector>
#include <Tools/Math/Vector_n.h>

template<size_t SIZE>
class LinearClassifier {
public:
    LinearClassifier(){
        p = 1;
        for (size_t i = 0; i < SIZE+1; i++) {
            w[i] = 1;
        }
    }

    double classify(const Vector_n<double,SIZE>& x) const {
        double ret = w[SIZE];
        for (size_t i = 0; i < SIZE; i++) {
            ret += (w[i] * x[i]);
        }
        return ret;
    }

    /* train the classifier by perception algorithm
     * @param positiveTrainSet: the positive train set
     * @param negativeTrainSet: the negative train set
     */
    void perceptionTrain(const std::vector<Vector_n<double,SIZE> >& positiveTrainSet,
            const std::vector<Vector_n<double,SIZE> >& negativeTrainSet){
        int max_iteration = 100;
        for(int k=0; k<max_iteration; k++){
            bool ok = true;
            // positive
            for (typename std::vector<Vector_n<double,SIZE> >::const_iterator iter=positiveTrainSet.begin();
            iter!=positiveTrainSet.end(); ++iter){
                double v = classify(*iter);
                if ( v <= 0 ){
                    ok = false;
                    for (size_t i=0; i<SIZE; i++){
                        w[i] += p*((*iter)[i]);
                    }
                    w[SIZE] += p;
                }
            }
            //negative
            for (typename std::vector<Vector_n<double,SIZE> >::const_iterator iter=negativeTrainSet.begin();
            iter!=negativeTrainSet.end(); ++iter){
                double v = classify(*iter);
                if ( v >= 0 ){
                    ok = false;
                    for (size_t i=0; i<SIZE; i++){
                        w[i] -= p*((*iter)[i]);
                    }
                    w[SIZE] -= p;
                }
            }

            if (ok) break;
        }
    }

    /* train the classifier by LMS(Least Sum of Error) algorithm
     * @param positiveTrainSet: the positive train set
     * @param negativeTrainSet: the negative train set
     */
    void LMSTrain(const std::vector<Vector_n<double,SIZE> >& positiveTrainSet, double positiveValue,
            const std::vector<Vector_n<double,SIZE> >& negativeTrainSet, double negativeValue, double rate, int iterationNum){
        double err = 1;
        
        double b1 = 1;
        double b2 = -1;

        for(int k=1; k<=iterationNum; k++){
            double pk = rate/k;
            err = 0;
            // positive
            for (typename std::vector<Vector_n<double,SIZE> >::const_iterator iter=positiveTrainSet.begin();
            iter!=positiveTrainSet.end(); ++iter){
                double e = b1 - classify(*iter);
                double pke = pk*e;
                for (size_t i=0; i<SIZE; i++){
                    err += fabs(e);
                    w[i] += pke*((*iter)[i]);
                }
                w[SIZE] += pke;
            }
            //negative
            for (typename std::vector<Vector_n<double,SIZE> >::const_iterator iter=negativeTrainSet.begin();
            iter!=negativeTrainSet.end(); ++iter){
                double e = b2 - classify(*iter);
                double pke = pk*e;
                for (size_t i=0; i<SIZE; i++){
                    err += fabs(e);
                    w[i] += pke*((*iter)[i]);
                }
                err += fabs(pke);
                w[SIZE] += pke;
            }
            std::cout<<"k="<<k<<"\terr="<<err<<std::endl;
        }
    }

    void fromStream(std::istream& ist) {
        for (size_t i = 0; i < SIZE + 1; i++) {
            ist >> w[i];
        }
        ist >> p;
    }

    void toStream(std::ostream& ost) const {
        for (size_t i = 0; i < SIZE + 1; i++) {
            ost << w[i] << ' ';
        }
        ost << p;
    }

private:
    double w[SIZE+1];
    double p;
};

template<size_t SIZE>
std::istream & operator >>(std::istream& ist, LinearClassifier<SIZE>& lc) {
    lc.fromStream(ist);
    return ist;
}

template<size_t SIZE>
std::ostream & operator <<(std::ostream& ost, const LinearClassifier<SIZE>& lc) {
    lc.toStream(ost);
    return ost;
}

#endif  /* _LINEARCLASSIFIER_H */

