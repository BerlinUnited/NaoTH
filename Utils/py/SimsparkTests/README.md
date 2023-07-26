```text
NOTE: currently the whole setup is only tested on linux and runs with python 3.6!
```

# Simspark Game

With the `game.py` script it is possible to run complete games.  
To do this, the following is required:
* Python 3.6
  * with the `naoth` package installed
* SimSpark SPL
  * the `simspark` executable must be available (e.g. locally compiled or as symlink to an AppImage)
  * simspark is started automatically by the script
  * if you want to use a containerised Simspark instance, you must start it manually
* compiled `naoth-simspark`

To get an overview of the available options, simply use the help ("-h") option:
```shell
Utils/py/SimsparkTests/game.py -h
```

The easiest way to start a game is as follows:
```shell
# cd into the default naoth directory
cd NaoTHSoccer/
# ... and start a game with the same config and executable for both teams
../Utils/py/SimsparkTests/game.py
# press Ctrl+C to cancel the game before it is finished. 
```

In order to configure different options, the best way is to let the script create a config file and then adjust the options to your needs:
```shell
Utils/py/SimsparkTests/game.py --write-config "my-config.cfg"
```
This allows us to set:
* different naoth configs
* use different executables
* set the used ports (if multiple instances should run at the same time)
* play multiple games in sequence 
* ...

To start a game with the modified config, simply use:
```shell
Utils/py/SimsparkTests/game.py --config "my-config.cfg"
```

# Simspark Tests

It is also possible to test certain game situations or game setups, just like playing a full game.
Therefore, the same setup is required:
* Python 3.6
* SimSpark SPL
* compiled `naoth-simspark`

The following commands give an overview of the available options and test cases:
```shell
# print help message
main.py -h

# view available test cases
main.py -l
```
To run a test, simply execute:
```shell
# cd into the default naoth directory
cd NaoTHSoccer/
# ... and start a test (or multiple by naming them)
../Utils/py/SimsparkTests/main.py -t PenaltyKicker
```

New tests can be easily implemented by using an existing test as a template.

# Simspark

The easiest way to run simspark is to download a precompiled AppImage from our [Github repository](https://github.com/BerlinUnited/SimSpark-SPL/releases) or use a Docker container (see below).

## Docker (Ubuntu 18.04)
* the simspark application can be build and run in a docker container
* in order to build simspark run the following command with one of the Dockerfiles (see below)
  * `docker build -t simspark .`
* afterwards, the container can be started with
  * `docker run --rm -p 3200:3200 -p 3100:3100 simspark`
    * the container is deleted (`--rm`)!
* if another port for the monitor and/or agents should be used, the simspark arguments can be set (`--agent-port PORTNUM`, `--server-port PORTNUM`)
  * the container forwarded ports must be set accordingly

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
