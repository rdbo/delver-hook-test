FROM debian:oldoldstable

WORKDIR /app

RUN apt-get -y update
RUN apt-get -y install gcc g++ gdb git cmake make python3 bash pkg-config vim
RUN apt-get -y install default-jdk
RUN ln -s /usr/lib/jvm/default-java/include/jni.h /usr/include/
RUN ln -s /usr/lib/jvm/default-java/include/linux/jni_md.h /usr/include/
RUN ln -s /usr/lib/jvm/default-java/include/jvmti.h /usr/include

CMD /bin/bash
