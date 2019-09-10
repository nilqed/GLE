#!/bin/sh

bbox=`gs -sDEVICE=bbox -dBATCH -dNOPAUSE $@ 2>&1 >/dev/null | grep '%%BoundingBox' | sed 's/\n//'`
sed -i "s@^%%BoundingBox.*@${bbox}@" $@
