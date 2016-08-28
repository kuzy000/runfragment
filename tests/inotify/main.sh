#!/bin/sh

frag="../../build/runfragment"

cp Broken.glsl Image.glsl

frag -c config.ini &
pid=$!

sleep 0.5
echo '}' >> Image.glsl
