FROM debian:latest

WORKDIR /app

RUN apt-get -y update
RUN apt-get -y install gcc g++ gdb git cmake make python3 bash pkg-config vim just
RUN apt-get -y install default-jdk
ENV JAVA_HOME=/usr/lib/jvm/default-java
