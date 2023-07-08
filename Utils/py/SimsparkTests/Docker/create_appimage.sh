#!/bin/bash
set -e

DIR=`pwd`
DIR_GIT=`realpath ./Simspark.Git`
DIR_INSTALL=`realpath ./Simspark.Install`
DIR_APP=`realpath ./Simspark.AppDir`
DIR_DIST=`realpath ./Simspark.Dist`
CLEAR=true

main()
{
    echo "Enter a comand (eg. 'help'):"
    while true
    do
        read -p ">>> " CMD OPT

        case "$CMD" in
            "create"|"c")
                CLEAR=true
                if [ "$OPT" = "-n" ]; then
                    CLEAR=false
                fi
                
                create_appimage
            ;;
            "update"|"u")
                cd "$DIR_GIT" && git pull && cd - > /dev/null
            ;;
            "exit"|"e")
                exit 0
            ;;
            "help"|"h")
                print_help
            ;;
            *)
                echo "Unknown argument: $1"
        esac
    done
}

print_help()
{
    echo "Creates a SimSpark AppImage."
    echo "The following directories are used/required to build the AppImage."
    echo "  * $DIR_GIT"
    echo "  * $DIR_INSTALL"
    echo "  * $DIR_APP"
    echo "  * $DIR_DIST"
    echo ""
    echo "Commands:"
    echo -e "  c|create\tCreates the AppImage"
    echo -e "  c|create -n\tCreates the AppImage without cleaning the build directory (faster)"
    echo -e "  u|update\tUpdates the git repository"
    echo -e "  e|exit\tExits the application"
    echo -e "  h|help\tPrint this help message"
}

make_build() {
    if [ -d "build" ] && [ "$CLEAR" = true ]; then
        echo "clear existing build directory ..."
        rm -R build
    fi

    echo "create build directory ..."
    mkdir -p build
}

compile_and_install() {
    echo "make ..."
    #make -j2
    make -j$(nproc)

    echo "make install ..."
    make install
}

find_lib()
{
    RESULT=$(ldconfig -p | grep -Po -m 1 "(?<==> ).+$(sed 's/[&/\+]/\\&/g'<<<"$1")$" || true)
    if [[ -z "$RESULT" ]]; then
        echo "NOT FOUND: $1"
    else
        echo $RESULT
    fi
}

create_appimage()
{
    if [ ! -f "./appimagetool-x86_64.AppImage" ]; then
        echo "Missing AppImage-Tool (appimagetool-x86_64.AppImage)! Download from:"
        echo "https://github.com/AppImage/AppImageKit/releases"
        exit 1
    fi

    if [ ! -d "$DIR_GIT" ]; then
        echo "Missing simspark git clone!"
        git clone https://github.com/BerlinUnited/SimSpark-SPL.git "$DIR_GIT"
    fi

    mkdir -p "$DIR_INSTALL"
    mkdir -p "$DIR_DIST"

    cd $DIR_GIT/spark

    make_build

    echo "configure ..."
    cd build
    cmake -DCMAKE_INSTALL_PREFIX:STRING="$DIR_INSTALL" \
          -DCMAKE_CXX_FLAGS=-w \
          ..

    compile_and_install

    #
    # Compile the RCSSServer part
    #

    cd $DIR_GIT/rcssserver3d

    make_build

    echo "configure ..."
    cd build
    cmake -DCARBON_DATA_DIR:STRING="$DIR_INSTALL/share" \
          -DCARBON_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DCARBON_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/libcarbon.so" \
          -DKEROSIN_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DKEROSIN_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/libkerosin.so" \
          -DOXYGEN_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DOXYGEN_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/liboxygen.so" \
          -DRCSSNET3D_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DRCSSNET3D_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/librcssnet3D.so" \
          -DSALT_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DSALT_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/libsalt.so" \
          -DSPARK_INCLUDE_DIR:STRING="$DIR_INSTALL/include/simspark" \
          -DSPARK_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/libspark.so" \
          -DZEITGEIST_LIBRARY:STRING="$DIR_INSTALL/lib/simspark/libzeitgeist.so" \
          -DCMAKE_INSTALL_PREFIX:STRING="$DIR_INSTALL" \
          -DCMAKE_CXX_FLAGS=-w \
          ..

    compile_and_install

    #
    # 
    #

    echo "Removing old application directory ..."
    if [ -d "$DIR_APP" ]; then
        rm -r $DIR_APP
    fi

    echo "Creating application directory ..."
    mkdir -p $DIR_APP
    chmod a+rw $DIR_APP

    echo "Copying installation files ..."
    cp -R ${DIR_INSTALL}/* "${DIR_APP}/"

    cd $DIR_APP

    echo "Deleting unnecessary files ..."

    # delete unnecessary stuff
    rm -rf ./include
    rm -rf ./share/doc

    # delete broken sym links
    find . -xtype l -delete

    echo "Copying libs and binaries ..."

    # copy probably missing libs
    mkdir -p ./lib/
    cp "$(find_lib libboost_system.so.1.65.1)" ./lib/
    cp "$(find_lib libpng16.so)" ./lib/
    cp "$(find_lib libtiff.so)" ./lib/
    #cp "$(find_lib libjbig.so)" ./lib/
    #cp "$(find_lib libmng.so)" ./lib/
    cp "$(find_lib libjpeg.so)" ./lib/
    cp "$(find_lib libicui18n.so)" ./lib/
    cp "$(find_lib ibicuuc.so)" ./lib/
    cp "$(find_lib libicudata.so)" ./lib/
    cp "$(find_lib libcaca.so)" ./lib/
    cp "$(find_lib libasound.so)" ./lib/
    cp "$(find_lib libslang.so)" ./lib/
    #cp "$(find_lib libncursesw.so)" ./lib/
    #cp "$(find_lib libtinfo.so)" ./lib/
    #cp "$(find_lib libnss_nis.so)" ./lib/
    cp "$(find_lib libruby-2.5.so.2.5)" ./lib/
    cp "$(find_lib libboost_thread.so.1.65.1)" ./lib/
    cp "$(find_lib libboost_regex.so)" ./lib/
    cp "$(find_lib libode.so.6)" ./lib/
    cp "$(find_lib libIL.so.1)" ./lib/
    cp "$(find_lib libSDL-1.2.so.0)" ./lib/
    #cp "$(find_lib libstdc++.so.6)" ./lib/
    cp "$(find_lib libfreetype.so)" ./lib/
    #cp "$(find_lib libGLU.so)" ./lib/
    #cp "$(find_lib libGL.so)" ./lib/libGL.so.1

    #cp "$(find_lib libboost_thread.so.1.65.1)" ./lib/
    #cp "$(find_lib libIL.so.1)" ./lib/
    cp "$(find_lib libtiff.so.5)" ./lib/
    cp "$(find_lib libjbig.so.0)" ./lib/
    #cp "$(find_lib libasound.so.2)" ./lib/
    #cp "$(find_lib libpulse-simple.so.0)" ./lib/
    #cp "$(find_lib libpulse.so.0)" ./lib/
    #cp "$(find_lib libXext.so.6)" ./lib/
    #cp "$(find_lib libcaca.so.0)" ./lib/
    #cp "$(find_lib libXcursor.so.1)" ./lib/
    #cp "$(find_lib libXrender.so.1)" ./lib/
    #cp "$(find_lib libXfixes.so.3)" ./lib/
    #cp "$(find_lib libGLX_mesa.so.0)" ./lib/
    #cp "$(find_lib libglapi.so.0)" ./lib/
    #cp "$(find_lib libdrm.so.2)" ./lib/
    #cp "$(find_lib libxcb-glx.so.0)" ./lib/
    #cp "$(find_lib libxcb-dri2.so.0)" ./lib/

    #cp "$(find_lib libgcc_s.so.1)" ./lib/
    #cp "$(find_lib libc.so.6)" ./lib/
    #cp "$(find_lib libpthread.so.0)" ./lib/
    #cp "$(find_lib libm.so.6)" ./lib/
    #cp "$(find_lib libfreetype.so.6)" ./lib/
    #cp "$(find_lib libGLU.so.1)" ./lib/
    #cp "$(find_lib libgmp.so.10)" ./lib/
    #cp "$(find_lib libdl.so.2)" ./lib/
    #cp "$(find_lib libcrypt.so.1)" ./lib/
    #cp "$(find_lib libz.so.1)" ./lib/
    #cp "$(find_lib libbz2.so.1.0)" ./lib/
    #cp "$(find_lib libpng16.so.16)" ./lib/
    #cp "$(find_lib libharfbuzz.so.0)" ./lib/
    #cp "$(find_lib libbrotlidec.so.1)" ./lib/
    #cp "$(find_lib liblcms2.so.2)" ./lib/
    #cp "$(find_lib libjpeg.so.8)" ./lib/
    #cp "$(find_lib libGLX.so.0)" ./lib/
    #cp "$(find_lib libGLdispatch.so.0)" ./lib/
    #cp "$(find_lib libOpenGL.so.0)" ./lib/
    #cp "$(find_lib librt.so.1)" ./lib/
    #cp "$(find_lib libglib-2.0.so.0)" ./lib/
    #cp "$(find_lib libgraphite2.so.3)" ./lib/
    #cp "$(find_lib libbrotlicommon.so.1)" ./lib/
    #cp "$(find_lib liblzma.so.5)" ./lib/
    #cp "$(find_lib libxcb.so.1)" ./lib/
    #cp "$(find_lib libXau.so.6)" ./lib/
    #cp "$(find_lib libXdmcp.so.6)" ./lib/
    #cp "$(find_lib libgcc_s.so.1)" ./lib/
    #cp "$(find_lib libX11.so.6)" ./lib/
    #cp "$(find_lib libbsd.so.0)" ./lib/

    # some more libs, these should be generally available
    #cp /usr/lib/x86_64-linux-gnu/libgmp.so.10 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/liblcms2.so.2 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb-dri3.so.0 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb-present.so.0 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb-sync.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxshmfence.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libglapi.so.0 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXext.so.6 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXdamage.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXfixes.so.3 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libX11-xcb.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libX11.so.6 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb-glx.so.0 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb-dri2.so.0 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libxcb.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXxf86vm.so.1 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libdrm.so.2 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXau.so.6 ./lib/
    #cp /usr/lib/x86_64-linux-gnu/libXdmcp.so.6 ./lib/
    #cp /lib/x86_64-linux-gnu/libdl.so.2 ./lib/

    mkdir -p ./lib/kerosin/
    mkdir -p ./lib/kerosin/inputserver/
    mkdir -p ./lib/oxygen/
    mkdir -p ./lib/zeitgeist/
    mkdir -p ./lib/spark/

    cp $DIR_GIT/spark/test/inputtest/german.scan.rb ./lib/kerosin/inputserver/
    cp $DIR_GIT/spark/lib/oxygen/oxygen.rb ./lib/oxygen/
    cp $DIR_GIT/spark/lib/zeitgeist/zeitgeist.rb ./lib/zeitgeist/
    cp $DIR_GIT/spark/lib/kerosin/kerosin.rb ./lib/kerosin/
    cp $DIR_GIT/spark/spark/spark.rb  ./lib/spark/

    # move some stuff
    cp -r $DIR_APP/share/rcssserver3d/* $DIR_APP/share/simspark
    rm -r $DIR_APP/share/rcssserver3d/

    #cd lib/
    #ln -s simspark/odeimps.so odeimps.so
    #ln -s libode.so libode.so.1
    #ln -s rcssserver3d/soccer.so soccer.so
    #cd ..

    echo "Creating start script ..."

    cat > AppRun <<EOF
#!/bin/sh

# adds a help message to the appimage, we don't need to execute the stuff below for showing this!
if ( [ \$# != 0  ] && [ "\$1" = "--help" ] ) || ( [ \$# != 0 ] && [ "\$1" = "-h" ] ); then
  echo "AppImage usage:
 --help              print this message.
 --appimage-mount        mounts the internal directory structure to a tmp directory
 --headless          disables the internal simspark monitor

usage: rcssserver3d [options] [script]
options:
 --script-path PATH      set the script path (rcssserver3d.rb path).
 --init-script-prefix PATH   path prefix for init scripts (spark.rb, oxygen.rb, etc.).
 --agent-port PORTNUM        port for agents to connect to.
 --server-port PORTNUM       port for monitors to connect to."
  exit 0
fi

# var definition
SELF_DIR="\$(dirname "\$(readlink -f "\$0")")"
LIBS_PATH="\$SELF_DIR/lib:\$SELF_DIR/lib/simspark:\$SELF_DIR/lib/rcssserver3d"
SIMSPARK_ARGS="rcssserverspl.rb"

# copy the script file to the simspark home dir to make sure the file exists for later modification
if [ ! -f \$HOME/.simspark/rcssserverspl.rb ]; then
  cp \$SELF_DIR/share/simspark/rcssserverspl.rb \$HOME/.simspark/
fi

# add "virtual" argument - disable monitor (GUI)
if [ \$# != 0 -a "\$1" = "--headless" ]; then
  # if starting headless, use a backup file which is modified and used for simspark
  cp \$HOME/.simspark/rcssserverspl.rb \$HOME/.simspark/rcssserverspl-appimage.rb
  sed -i 's/\$enableInternalMonitor = true/\$enableInternalMonitor = false/g' \$HOME/.simspark/rcssserverspl-appimage.rb
  # extend argument list
  set -- "$@" "--script-path" "rcssserverspl-appimage.rb"
  shift 1
fi

# set library path
if [ -z "\$LD_LIBRARY_PATH" ]; then
  LD_LIBRARY_PATH="\$LIBS_PATH"
else
  LD_LIBRARY_PATH="\$LIBS_PATH:\$LD_LIBRARY_PATH"
fi
export LD_LIBRARY_PATH

# set the environment vars
export SPARK_DIR="\$SELF_DIR"
export RCSS_BUNDLE_PATH="\$SELF_DIR"

# change working directory and exceute application
cd \$SELF_DIR
exec "\$SELF_DIR/bin/simspark" "\$@"
EOF
    chmod a+x AppRun

    cat > simspark.desktop <<EOF
[Desktop Entry]
Name=Simspark
Comment=RoboCup Soccer Simulation Server 3D
Icon=simspark
Type=Application
Categories=Education;Science;Robotics;
EOF

    cp $DIR_GIT/rcssserver3d/data/logos/simspark.png .

    cd $DIR

    echo "Generate AppImage ..."

    VERSION=$(awk '/^#define RCSS_VERSION "(.+)"/{print $3}' $DIR_INSTALL/include/rcssserver3d/rcssserver3d_config.h | tr -d '"')

    ARCH=x86_64 ./appimagetool-x86_64.AppImage --appimage-extract-and-run $DIR_APP "$DIR_DIST/Simspark_v$VERSION.AppImage"
}

# run the actual script
main
