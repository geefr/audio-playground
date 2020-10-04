FROM ubuntu:latest
LABEL project="OpenGL Visualiser"
LABEL MAINTAINER Gareth Francis (gfrancis.dev@gmail.com)
# Deal with tzdata asking the user for info
ENV TZ=Europe/London
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y && apt-get install -y cmake gcc g++ git
RUN apt-get install -y libglfw3-dev libglm-dev

