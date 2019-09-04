#!/bin/bash

export CCACHE_DIR="`pwd`/ccache"
export CCACHE_BASEDIR="`rpm --eval %_builddir`/`ls build/SRPMS/*.src.rpm | awk -F '.' 'BEGIN{OFS=".";} {NF-=3;}1' | awk -F '/' '{print $NF}'`"
export CCACHE_SLOPPINESS=pch_defines
export CCACHE_NOHASHDIR=true
export CCACHE_MAXSIZE=4G

ccache -s
ccache -p
