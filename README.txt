Introduction
============

This is the NaoTH repository 2011. See STATUS.txt for an overview what is done
yet.

Directory layout description

+ NaoRunner       : controller for the real Nao and different simulators
   + Interface    : a generic interface to interact with either a real or 
                  simulated robot
   + DCM          : real robot implementation
   + Webots       : webots simulator implementation
   + SimSpark     : SimSpark simulator implementation
   + LogSimulator : simulator based on logfiles
   + Webcam       : use your webcam as a "simulator"
+ ModuleFramework : a simple and powerful module framework
+ TestBehavior    : some simple robot control using the NaoRunner
+ SoccerNaoTH     : The NaoTH 2011 soccer controller (not published)

