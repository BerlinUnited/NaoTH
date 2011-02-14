Introduction
============

This is the NaoTH repository 2011. See STATUS.txt for an overview what is done
yet.

Directory layout description

+ Platforms          : controller for the real Nao and different simulators
+ NaoTH-Tools        : common code like the platform interface or the module architecture
+ DebugCommunication : a simple and powerful debug communication framework based on glib
+ SimpleSoccerAgent  : some simple robot soccer agent using the NaoRunner
+ SoccerNaoTH        : The NaoTH 2011 soccer controller (not published)

Conventions: LUA premake project structure
------------------------------------------
- Basic Rule: no external pathes in projects
- projects contain local dependency definitions, i.e., includes to local projects
- all external dependencies are defined in solution (inlude and lib directories)
- links are defined in projects which compile to an executable or shared/dynamic library 
