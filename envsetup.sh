#!/bin/bash

#设置编译环境变量
function setup_env()
{
	PLATFORM=-1
	
	echo
    echo "platform supported："
    echo 
    echo 0:x86_64-linux-gnu
    echo 
    echo 1:raspberry pi
    echo 
    
    until [ "$PLATFORM" = "0" ] || 
          [ "$PLATFORM" = "1" ]
		  
    do
        read -p "select platform：" PLATFORM
    done
    
    case $PLATFORM in
    0)
        ;;
    1)
        export SYS_ROOT=/opt/rpi/root
        export CROSS_COMPILE=/opt/rpi/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf- 
        export CC=$CROSS_COMPILE"gcc"
        export CXX=$CROSS_COMPILE"g++"
        export AR=$CROSS_COMPILE"ar"
        ;; 
    esac

    export PLATFORM

    IPC_SCHEME=-1
    
    echo
    echo "ipc scheme："
    echo 
    echo 0:dbus
    echo 
    echo 1:rpclib
    echo 
    
    until [ "$IPC_SCHEME" = "0" ] || 
          [ "$IPC_SCHEME" = "1" ]
          
    do
        read -p "please select ipc scheme：" IPC_SCHEME
    done

    export IPC_SCHEME

    pwd
	
    echo "platform: " $PLATFORM
    echo "ipc scheme: " $IPC_SCHEME
    echo "system root: " $SYS_ROOT
    echo "cross complie: " $CROSS_COMPILE
    echo "C complie: " $CC
    echo "CXX complie: " $CXX
    echo "AR complie: " $AR
}

setup_env
