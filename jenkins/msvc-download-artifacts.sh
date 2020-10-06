#!/usr/bin/env bash
set -euo pipefail

tp=service-docker:8081/artifactory/generic-local/thirdparty/

curl -o glew.zip "${tp}/glew-2.1.0.zip"
curl -o glfw.zip "${tp}/glfw-3.3.2.zip"
curl -o glm.zip "${tp}/glm-0.9.9.8.zip"

unzip -o glew.zip -d glew
unzip -o glfw.zip -d glfw
unzip -o glm.zip -d glm

