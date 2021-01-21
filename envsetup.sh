# !/bin/sh

# setup build environment
function setup_env()
{
    # ipc scheme
    # IPC_SCHEME_NO=-1
    # echo
    # echo "ipc scheme："
    # echo 
    # echo 0:dbus
    # echo 
    # echo 1:rpc
    # echo 
    
    # until [ "$IPC_SCHEME_NO" = "0" ] || 
    #       [ "$IPC_SCHEME_NO" = "1" ]
          
    # do
    #     read -p "please select ipc scheme：" IPC_SCHEME_NO
    # done

    # case $IPC_SCHEME_NO in
    # 0)
    #     export IPC_SCHEME="dbus"
    #     ;;
    # 1)
    #     export IPC_SCHEME="rpc"
    #     ;; 
    # esac

    # platform
	PLATFORM_NO=-1
	echo
    echo "platform supported："
    echo 
    echo 0:x86_64-linux-gnu
    echo 
    echo 1:raspberry-pi
    echo 

    until [ "$PLATFORM_NO" = "0" ] || 
          [ "$PLATFORM_NO" = "1" ]
		  
    do
        read -p "select platform：" PLATFORM_NO
    done
    
    case $PLATFORM_NO in
    0)
        export PLATFORM="x86_64-linux-gnu"
        ;;
    1)
        export PLATFORM="raspberry-pi"
        export SYS_ROOT=/opt/rpi/root
        export CROSS_COMPILE=/opt/rpi/arm-rpi-4.9.3-linux-gnueabihf/bin/arm-linux-gnueabihf- 
        export CC=$CROSS_COMPILE"gcc"
        export CXX=$CROSS_COMPILE"g++"
        export AR=$CROSS_COMPILE"ar"
        ;; 
    esac

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
