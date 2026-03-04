#!/bin/sh

set -e

echo "[*] Building Dockerfile..."
docker build -t delverhook .

echo "[*] Starting Docker environment..."

prog="${@:-bash}"
docker run -v "$(pwd):/app" --workdir /app -it delverhook $prog
