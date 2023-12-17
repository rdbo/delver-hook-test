#!/bin/sh

docker build -t delverhook .
docker run -v "$(pwd):/app" -it delverhook
