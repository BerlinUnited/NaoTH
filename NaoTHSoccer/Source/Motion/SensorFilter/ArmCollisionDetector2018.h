#ifndef _ArmCollisionDetector2018_H_
#define _ArmCollisionDetector2018_H_

#include <ModuleFramework/Module.h>

//Representations
#include <Representations/Infrastructure/FrameInfo.h>
#include <Representations/Infrastructure/JointData.h>
#include <Representations/Motion/MotionStatus.h>
#include <Representations/Motion/Request/MotionRequest.h>
#include <Representations/Motion/CollisionPercept.h>

//Tools
#include <Tools/Math/ConvexHull.h>
#include <Tools/DataStructures/RingBufferWithSum.h>
#include <vector>
#include <string>
#include <fstream>

//Debug and Plot
#include <Tools/Debug/DebugRequest.h>
#include <Tools/Debug/DebugPlot.h>
#include <Tools/Debug/DebugModify.h>
#include <Tools/Debug/DebugParameterList.h>


BEGIN_DECLARE_MODULE(ArmCollisionDetector2018)
  PROVIDE(DebugRequest)
  PROVIDE(DebugPlot)
  PROVIDE(DebugModify)
  PROVIDE(DebugParameterList)

  REQUIRE(FrameInfo)
  REQUIRE(MotorJointData)
  REQUIRE(SensorJointData)
  REQUIRE(MotionStatus)
  REQUIRE(MotionRequest)

  PROVIDE(CollisionPercept)
END_DECLARE_MODULE(ArmCollisionDetector2018)

class ArmCollisionDetector2018 : private ArmCollisionDetector2018Base
{
public:
	ArmCollisionDetector2018();
	~ArmCollisionDetector2018();
	void execute();

public:
	class Parameter : public ParameterList
	{
	public:
		Parameter() : ParameterList("ArmCollisionDetector2018")
		{
			//Entweder direkt als Point vektor
			//PARAMETER_REGISTER(ReferenceHull) = vector<Point>;
			//Oder als Pfad zur txt was vermutlich einfacher ist
			PARAMETER_REGISTER(point_configLeft) = "reference_points_cd18Left.txt";
			PARAMETER_REGISTER(point_configRight) = "reference_points_cd18Right.txt";
			PARAMETER_REGISTER(collect) = 32;
			syncWithConfig();

		}
		std::string point_configLeft;
		std::string point_configRight;
		size_t collect;
	} params;

private:
	//Private variablen wie zb ringbuffer zur MJD und SJD synchronisation
	RingBuffer<double, 4> jointDataBufferLeft;
	RingBuffer<double, 4> jointDataBufferRight;

  struct Greater {
    bool operator()(const Vector2d& A, const Vector2d& B) const {   
        return A.x < B.x || (A.x == B.x && A.y < B.y);
    }   
  };

  template<typename T>
  struct Less {
    bool operator()(const T& A, const T& B) const {   
        return A.y < B.y || (A.y == B.y && A.x < B.x);
    }   
  };

	double cross(const Vector2d &O, const Vector2d &A, const Vector2d &B){
		return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
	}

	// Implementation of Andrew's monotone chain 2D convex hull algorithm.
	std::vector<Vector2d> convex_hull(std::vector<Vector2d> P)
  {
		//Initialization and Trivial case check
		int n = P.size(), k = 0;

		if (n <= 3) { 
      return P; 
    };

		std::vector<Vector2d> H(2 * n);
		sort(P.begin(), P.end(), Less<Vector2d>());

		// Build lower hull
		for (int i = 0; i < n; i++) {
			while (k >= 2 && cross(H[k - 2], H[k - 1], P[i]) <= 0) k--;
			H[k++] = P[i];
		}

    std::cout << "k1 = " << k << std::endl;

		// Build upper hull
		for (int i = n - 1, t = k + 1; i > 0; i--){
			while (k >= t && cross(H[k - 2], H[k - 1], P[i - 1]) <= 0) k--;
			H[k++] = P[i - 1];
		}

    std::cout << "k1 = " << k << std::endl;

		H.resize(k);
		return H;
	}
};

#endif