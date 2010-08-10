-- NaoTH controller for the logfile based "simulator"
project "LogSimulator"
  kind "StaticLib"
  language "C++"
        
  files{"LogSimulatorSource/**.cpp"}    

  links {"Interface"}

  targetname "logsimulator"
  targetdir "dist/"

  -- END LogSimulator
