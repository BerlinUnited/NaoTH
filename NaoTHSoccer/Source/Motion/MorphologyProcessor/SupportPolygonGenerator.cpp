/**
* @file SupportPolygonGenerator.cpp
*
* @author <a href="mailto:xu@informatik.hu-berlin.de">Xu, Yuan</a>
* Implentation of the class SupportPolygonGenerator
*/
#include <Tools/Math/ConvexHull.h>
#include <Tools/NaoInfo.h>
#include "SupportPolygonGenerator.h"
#include "PlatformInterface/Platform.h"

using namespace naoth;
using namespace std;

SupportPolygonGenerator::SupportPolygonGenerator()
{
  //TODO
  REGISTER_DEBUG_COMMAND("CalibrateFootTouchDetector", "Calibrate Foot Touch Detector", this);

  // HACK: do remove ;)
  theFSRData = getFSRData().force;
  theFSRPos = getFSRPositions().pos;
  theLink = getKinematicChainSensor().theLinks;


  string leftFootTouchDetectorCfg("1 1 1 1 -5");
  string rightFootTouchDetectorCfg("1 1 1 1 -5");
  const Configuration& cfg = Platform::getInstance().theConfiguration;
  string cfgname = "TouchDetector";
  if ( cfg.hasGroup(cfgname) )
  {
    leftFootTouchDetectorCfg = cfg.getString(cfgname, "leftFoot");
    rightFootTouchDetectorCfg = cfg.getString(cfgname, "rightFoot");
  }
  
  theLeftFootTouchDetector.init(theFSRData + FSRData::LFsrFL, leftFootTouchDetectorCfg);
  theRightFootTouchDetector.init(theFSRData + FSRData::RFsrFL, rightFootTouchDetectorCfg);
}

SupportPolygonGenerator::~SupportPolygonGenerator()
{
  
}

void SupportPolygonGenerator::execute()
{
  // TODO: make it better :)
  SupportPolygon& sp = getSupportPolygon();

    // supoort mode
    bool leftSupportable = isFootSupportable(true);
    bool rightSupportable = isFootSupportable(false);
    vector<Vector2<double> > vertex;
    if (leftSupportable){
        for(int i=FSRData::LFsrFL; i<FSRData::RFsrFL; i++) {
            const Vector3<double>& p = theFSRPos[i];
            vertex.push_back(Vector2<double>(p.x, p.y));
        }
    }
    if (rightSupportable){
        for(int i=FSRData::RFsrFL; i<FSRData::numOfFSR; i++) {
            const Vector3<double>& p = theFSRPos[i];
            vertex.push_back(Vector2<double>(p.x, p.y));
        }
    }

    sp.vertex = ConvexHull::convexHull(vertex);

    // force center
    sp.forceCenter = calcSupportForceCenter();

    if ( leftSupportable && !rightSupportable ){
        sp.mode = SupportPolygon::LEFT;
    } else if ( !leftSupportable && rightSupportable ){
        sp.mode = SupportPolygon::RIGHT;
    } else if ( leftSupportable && rightSupportable ){
        sp.mode = SupportPolygon::DOUBLE;
        bool leftonly = ( (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::LFsrFL], theFSRPos[FSRData::LFsrBL]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::LFsrBL], theFSRPos[FSRData::LFsrBR]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::LFsrBR], theFSRPos[FSRData::LFsrFR]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::LFsrFR], theFSRPos[FSRData::LFsrFL]) > 0) );
        bool rightonly = ( (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::RFsrFL], theFSRPos[FSRData::RFsrBL]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::RFsrBL], theFSRPos[FSRData::RFsrBR]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::RFsrBR], theFSRPos[FSRData::RFsrFR]) > 0)
                && (ConvexHull::cross(sp.forceCenter, theFSRPos[FSRData::RFsrFR], theFSRPos[FSRData::RFsrFL]) > 0) );

        if (leftonly && !rightonly)
            sp.mode = SupportPolygon::DOUBLE_LEFT;
        else if (!leftonly && rightonly)
            sp.mode = SupportPolygon::DOUBLE_RIGHT;
        else if (leftonly && rightonly )
            cerr<<"Unknown support mode!!"<<endl;// should not be in this case
    } else if (leftSupportable&&rightSupportable) {
        sp.mode = SupportPolygon::DOUBLE;
    } else {
        sp.mode = SupportPolygon::NONE;
    }

    // determine stand origin
    static bool lastLeftFoot = true;
    switch (sp.mode)
    {
        case SupportPolygon::LEFT :
        case SupportPolygon::DOUBLE_LEFT :
                    lastLeftFoot = true;
            break;
        case SupportPolygon::RIGHT :
        case SupportPolygon::DOUBLE_RIGHT :
                    lastLeftFoot = false;
            break;
        default:
            break;
    }
    if ( lastLeftFoot ){
        sp.standOrigin.rotation = theLink[KinematicChain::LFoot].R;
        sp.standOrigin.translation = theLink[KinematicChain::LFoot].p + sp.standOrigin.rotation * Vector3<double>(0, -NaoInfo::HipOffsetY, 0);
    }
    else{
        sp.standOrigin.rotation = theLink[KinematicChain::RFoot].R;
        sp.standOrigin.translation = theLink[KinematicChain::RFoot].p + sp.standOrigin.rotation * Vector3<double>(0, NaoInfo::HipOffsetY, 0);
    }
    sp.standOrigin.translation.z = 0;

}//end calcSupportPolygon

Vector3<double> SupportPolygonGenerator::calcSupportForceCenter()
{
    Vector3<double> c(0,0,0);
    double fs=0;
    for(int i=0;i<FSRData::numOfFSR;i++){
        fs += theFSRData[i];
        c += (theFSRPos[i] * theFSRData[i]);
    }
    if (fs > 0){
        c /= fs;
    }
    return c;
}

bool SupportPolygonGenerator::isFootSupportable(bool isLeft) const
{
    if (isLeft){
        return theLeftFootTouchDetector.isTouch();
    }
    else{
        return theRightFootTouchDetector.isTouch();
    }
}

//TODO

void SupportPolygonGenerator::executeDebugCommand(
        const std::string& /*command*/,
        const std::map<std::string, std::string>& /*arguments*/,
        std::ostream & /*outstream*/)
{
/*    if ("CalibrateFootTouchDetector" == command)
    {
        bool touch = false;
        if ( arguments.find("touch") != arguments.end() ){
            if ( "true" == arguments.find("touch")->second ){
                touch = true;
            }
        }
        bool isLeftFoot = true;
        if ( arguments.find("foot") != arguments.end() )
        {
            if ( "Right" == arguments.find("foot")->second ){
                isLeftFoot = false;
            }
        }
        bool isInit = false;
        if ( arguments.find("init") != arguments.end() )
        {
            if ( "true" == arguments.find("init")->second ){
                isInit = true;
            }
        }
        bool isSave = false;
        if ( arguments.find("save") != arguments.end() )
        {
            if ( "true" == arguments.find("save")->second ){
                isSave = true;
            }
        }
        if (isLeftFoot){
            if (isInit){
                theLeftFootTouchDetector.init(theFSRData + FSRData::LFsrFL, leftFootTouchDetectorCfg);
            }
            else if (isSave){
                theLeftFootTouchDetector.save(leftFootTouchDetectorCfg);
            }
            else{
                theLeftFootTouchDetector.calibrate(touch);
            }
        }else{
            if (isInit){
                theRightFootTouchDetector.init(theFSRData + FSRData::RFsrFL, rightFootTouchDetectorCfg);
            }
            else if (isSave){
                theRightFootTouchDetector.save(rightFootTouchDetectorCfg);
            }
            else {
                theRightFootTouchDetector.calibrate(touch);
            }
        }
    }
    */
}//end executeDebugCommand

