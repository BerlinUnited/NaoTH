# Simspark Tests
Currently we only have one functioning setup of Simspark that can be used for tests. It's located on the instituts gruenau servers.

To run tests there do the following:
- Login to any gruenau server as naoth user
- `cd /vol/home-vol1/wissen/naoth/NaoTH/naoth-2020/Utils/py/SimsparkTests`
- `python3 main.py -t OpponentPushingFreekick -s /vol/home-vol1/wissen/naoth/NaoTH/Simspark_v0.7.2-naoth-6.AppImage`

To get a list of implemented tests run `python3 main.py -l`. The tests are implemented in `/vol/home-vol1/wissen/naoth/NaoTH/naoth-2020/Utils/py/SimsparkTests/Tests`. You can easily implement your own tests by using the existing ones as template.

The `-s` argument is the path to simspark. You have to use this. By default it find another simspark installed somewhere which throws errors.
If you try run this on another system run the Simspark AppImage first to see if it works. You can expect output like this:
```
(MonitorServer) WARNING: SimulationServer not found.
rcssserver3d (formerly simspark), a monolithic simulator 0.7.2-naoth-6
Copyright (C) 2004 Markus Rollmann, 
Universität Koblenz.
Copyright (C) 2004-2016, The RoboCup Soccer Server Maintenance Group.

Type '--help' for further information

(SimulationServer) SimControlNode 'AgentControl' registered
(AgentControl) Running in sync mode.
(spark.rb) recording Logfile as 'sparkmonitor.log'
(spark.rb) sparkSetupInput
(spark.rb) using InputSystem 'InputSystemSDL'
(InputServer) Init InputSystemSDL
(InputServer) CreateDevice Keyboard
(InputServer) CreateDevice Mouse
(spark.rb) sparkSetupTimer
(spark.rb) using TimerSystem 'TimerSystemBoost'
(SimulationServer) SimControlNode 'InputControl' registered
(SimulationServer) TimerSystem 'TimerSystemBoost' registered
(bindings.rb) setting up bindings
(spark.rb) sparkEnableLog logTarget=:cerr logType=eError
(Light) ERROR: OpenGLServer not found
(Light) ERROR: OpenGLServer not found
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(Material2DTexture) ERROR: cannot find TextureServer
(Material2DTexture) ERROR: OpenGLServer not found.
(InputControl) ERROR: no FPSController found at '/usr/scene/camera/physics/controller'

```



## Old Notes from Philipp
Do some Tests with:


Run a game (or more) with:

Do not forget to configure naoth!
Enable/Disable neccessary modules and set parameters.

Simspark can be run "headless", therefore modify rcsss...rb and set to false

Another Simspark configuration is the simulated joint heating which can be turned off via ....
