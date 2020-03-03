#!/bin/tcsh
# 
# File:   GraffitiSH.tcsh
# Author: Orbis Terrae
#
# Created on Jan 30, 2018, 9:54:50 PM
#

echo "$0 $1"
set called=($_)

set script_dir=`dirname $0`

set argdir="./*"

if ( "$*" == "" ) then
    echo "no arguments, processing all files in `pwd`"
    set argdir="./*"

    foreach file ($argdir)
        echo "---------------------------------------------------------------------------"
        echo "---> $script_dir/GraffitiCL $file"
        $script_dir/GraffitiCL $file
    end
else
#    set argdir=$1
#    set argdir="${1:q}*"
    echo "---> argument detected, processing all files in $1"

    foreach file ("$1*")
        echo "---------------------------------------------------------------------------"
        echo "---> $script_dir/GraffitiCL $file"
        $script_dir/GraffitiCL $file
    end
endif

#echo "argdir= $argdir"
#ls -alF `echo "$argdir"`
#foreach file ($argdir)
#    echo "---------------------------------------------------------------------------"
#    echo "---> $script_dir/GraffitiCL $file"
#        $script_dir/GraffitiCL $file
#end
