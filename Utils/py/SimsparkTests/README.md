```text
NOTE: currently the whole setup is only tested on linux and runs with python 3.6!
```

# Simspark Game
* 

# Simspark Tests
Currently, we only have one functioning setup of SimSpark that can be used for tests. It's located on the instituts gruenau servers.

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

# Simspark

Um SimSpark zum laufen zu bekommen ist es am einfachsten eines der vorkomililierten AppImages von XYZ zu verwenden oder via Dockerimage.

## Docker (Ubuntu 18.04)
* the simspark application can be build and run in a docker container
* in order to build simspark run the following command with one of the Dockerfiles (see below)
  * `docker build -t simspark .`
* afterwards, the container can be started with
  * `docker run --rm -p 3200:3200 -p 3100:3100 simspark`
    * the container is deleted (`--rm`)!
* if another port for the monitor and/or agents should be used, the simspark arguments can be set (`--agent-port PORTNUM`, `--server-port PORTNUM`)
  * the container forwarded ports must be set accordingly

Dockerfile:
```docker
FROM ubuntu:18.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libfreetype6 libode6 libsdl1.2debian ruby libdevil1c2 qt4-default nano \
    libboost-regex1.65.1 libboost-system1.65.1 libboost-thread1.65.1 \
    g++ cmake git libfreetype6-dev libode-dev libsdl-dev ruby-dev libdevil-dev libboost-dev libboost-thread-dev libboost-regex-dev libboost-system-dev libqt4-opengl-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN git clone \
      --depth 1 https://github.com/BerlinUnited/SimSpark-SPL.git /tmp/SimSpark \
    && mkdir -p /tmp/SimSpark/spark/build \
    && cd /tmp/SimSpark/spark/build \
    && cmake -DRVDRAW=OFF -DCMAKE_CXX_FLAGS=-w .. \
    && make -j$(nproc) \
    && make install \
    && ldconfig \
    && mkdir -p /tmp/SimSpark/rcssserver3d/build \
    && cd /tmp/SimSpark/rcssserver3d/build \
    && cmake -DRVDRAW=OFF -DCMAKE_CXX_FLAGS=-w .. \
    && make -j$(nproc) \
    && make install && ldconfig \
    && rm -fr /tmp/SimSpark \
    && apt-get purge -y --auto-remove $buildDeps \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && sed -i 's/\$enableInternalMonitor = true/\$enableInternalMonitor = false/g' /usr/local/share/rcssserver3d/rcssserverspl.rb

ENV LD_LIBRARY_PATH=/usr/local/lib/simspark:/usr/local/lib/rcssserver3d

ENTRYPOINT ["rcssserver3d"]

EXPOSE 3100
EXPOSE 3200
```

## Docker (AppImage)
* the simspark application can be build and run as [AppImage](https://github.com/AppImage) (self-contained application)
* the AppImage is build with a docker container
* use the following command with the Dockerfile to build the container
  * `docker build -t simspark-appimage -f Dockerfile_AppImage .`
    * the `create_appimage.sh` script is required to build the container (see `Docker` directory)
* afterward, the AppImage can be created running the container 
  * `docker run -it -v "/path/to/save/result/:/root/Simspark.Dist" --name simspark-appimage simspark-appimage`
    * make sure to set the result path, to get the created AppImage
    * the container has a simple command line interface
      * `help`, `create`, `create -n`, `exit`
* the container can be used multiple times to create a new AppImage
  * `docker start -ia simspark-appimage`

Dockerfile:
```docker
FROM ubuntu:18.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libfreetype6 libode6 libsdl1.2debian ruby libdevil1c2 qt4-default nano file \
    libboost-regex1.65.1 libboost-system1.65.1 libboost-thread1.65.1 \
    g++ cmake git libfreetype6-dev libode-dev libsdl-dev ruby-dev libdevil-dev libboost-dev libboost-thread-dev libboost-regex-dev libboost-system-dev libqt4-opengl-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

ADD https://github.com/AppImage/AppImageKit/releases/download/13/appimagetool-x86_64.AppImage /root
RUN chmod +x /root/appimagetool-x86_64.AppImage

RUN git clone https://github.com/BerlinUnited/SimSpark-SPL.git /root/Simspark.Git

COPY create_appimage.sh /root/

WORKDIR /root
ENTRYPOINT ["/root/create_appimage.sh"]
```

## Old Notes from Philipp
Do some Tests with:


Run a game (or more) with:

Do not forget to configure naoth!
Enable/Disable neccessary modules and set parameters.

Simspark can be run "headless", therefore modify rcsss...rb and set to false

Another Simspark configuration is the simulated joint heating which can be turned off via ....
