function [py] = generateZMPNaoDevils(phase, t)

polygonLeft  = [ 0.02, 0, 0, -0.025];
polygonRight = [-0.02, 0, 0,  0.025];

foot_spacing = -0.02; % rf.y-lf.y Annahme symmetrisch, lf positiv, rf negativ
% ending = 0;
% kickPhase = 1;

switch phase
    case 1%'toFirstSingleSupport'
      py=FourPointBezier1D(polygonLeft, t);%frameInPhase/singleSupportLen);
%       if ((*_footList)->kickPhase == freeLegNA)
%         p.x=-pxss+pc*(float)theControllerParams.dt*vxss;
%       else
%         p.x = -0.02f;
%       toConvert=true;    
%       break;

    case 2%'toFirstDoubleSupport'
%       if (kickPhase == ending)
%       
%         py = (rf.y - lf.y) / 2;
%       
%       else
        plgn = [polygonLeft(4), foot_spacing/2, foot_spacing/2, foot_spacing + polygonRight(1)];
        py= FourPointBezier1D(plgn, t);%frameInPhase/doubleSupportLen);
%       end
%       if ((*_footList)->kickPhase == freeLegNA)
%         p.x=+pxss+pc*(float)theControllerParams.dt*vxds;
%       else
%         p.x = -0.02f;
%       break;

    case 3%'toFecondSingleSupport'
      py=FourPointBezier1D(polygonRight, t);%frameInPhase/singleSupportLen);
%       if ((*_footList)->kickPhase == freeLegNA)
%         p.x=-pxss+pc*(float)theControllerParams.dt*vxss;
%       else
%         p.x = -0.02f;
%       toConvert=true;
%       break;

    case 4%'toFecondDoubleSupport'
%       if (kickPhase == ending)
%         py = (lf.y - rf.y) / 2;
%       else
        plgn = [polygonRight(4), -foot_spacing/2, -foot_spacing/2, -foot_spacing + polygonLeft(1)];
        py = FourPointBezier1D(plgn, t);
%       end
%       if ((*_footList)->kickPhase == freeLegNA)
%         p.x=+pxss+pc*(float)theControllerParams.dt*vxds;
%       else
%         p.x = -0.02f;
%       break;

    otherwise
%       p.x = -pxss;
      py = -0.05;
%       toConvert = true;
%       break;
end