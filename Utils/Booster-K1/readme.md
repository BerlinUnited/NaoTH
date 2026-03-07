

## Toolchain

Works for both: windows or linux toolchain repo

1. brahch "booster"
2. run `setup.sh` or `setup.bat`
  - needed to generate a new `projectconfig.user.lua`
  - no export or saving of variables is needed
3. copy `projectconfig.user.lua` to `NaoTHSoccer/Make`

### Verify

The file `projectconfig.user.lua` should contain such lines

```lua
...
-- LINE: 17

if PLATFORM == "Booster" then
  NAO_CTC = "<toolchain_repo>/toolchain_booster_aarch64"
end

...

-- LINE: 35

if PLATFORM == "Nao" or PLATFORM == "Booster" then
   _OPTIONS["crosscompiler"] = "clang" -- set "gcc" if needed
end
``` 

## Compile

1. `cd` to `<project>/NaoTHSoccer/Make`
2. run `compileGameBooster.sh`



## Deploy

1. on the Booster make a directory `~/naoth`
2. assume the ip is `10.0.4.41`


### Setup libs

1. copy libs to teh robot
```sh
cd <toolchain_repo>/toolchain_booster_aarch64/extern

scp -r lib booster@10.0.4.41:~/naoth/lib
```  

2. generate symlinks (the parameter `-n` is very important!)
```
ssh booster@10.0.4.41
cd ~/naoth/lib
ldconfig -n .
```

or 

```
ssh booster@10.0.4.41
ldconfig -n ~/naoth/lib
```

- NOTE:
  - `ldconfig -n <dir>` 
    - Only process directories specified on the command line.  
    - Don't build cache.


### Make directory for the bins

```
ssh booster@10.0.4.41
mkdir ~/naoth/bin
```

### Deploy the bridge

1. copy all python files (some of them are experimental)
```sh
cd <project dir>/Framework/Platforms/Source/Booster/BoosterRobot
scp *.py booster@10.0.4.41:~/naoth/bin/
```


### Deploy

1. Assume the current working directory is `<project>/NaoTHSoccer/Make`

2. Copy Binary
```sh
scp ../dist/Booster/naoth-booster booster@10.0.4.41:~/naoth/bin/
```

3. Copy Configs
```sh
scp -r ../Config booster@10.0.4.41:~/naoth/
```


## Run

All binaries must be executed in `~/naoth` on the Booster

1. start the bridge (in one shell)

```sh
ssh booster@10.0.4.41
cd ~/naoth
python image_bridge.py
```

12 start the binaty (in a second shell)

```sh
ssh booster@10.0.4.41
cd ~/naoth
./naoth-booster
```

## Test

Use RoboControl to receive images etc.
