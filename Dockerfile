FROM ubuntu:latest
MAINTAINER Shiroha "keqing.hu@icloud.com"

#RUN \
#  apt update && \
#  apt -y upgrade && \
#  apt -y install gcc-8 g++-8 gdb make dpkg-dev flex adoptopenjdk-8-hotspot && \
#  apt -y install python python-virtualenv python3 python3-virtualenv && \
#  mkdir -p /usr/local/cjudger/venv/ && \
#  virtualenv --no-setuptools --no-pip --no-wheel -p /usr/bin/python2 /usr/local/cjudger/venv/py2 && \
#  virtualenv --no-setuptools --no-pip --no-wheel -p /usr/bin/python3 /usr/local/cjudger/venv/py3 && \
#  ln -s /usr/local/cjudger/venv/py2/bin/python2 /usr/local/cjudger/py2 && \
#  ln -s /usr/local/cjudger/venv/py3/bin/python3 /usr/local/cjudger/py3 && \
#  rm -rf /var/lib/apt/lists/*

RUN \
  apt update && \
  apt -y upgrade && \
  apt -y libseccomp-dev && \
  apt -y install gcc g++

ARG BIN_DIR=/portable
COPY ./portable ${BIN_DIR}
WORKDIR ${BIN_DIR}

CMD ./main