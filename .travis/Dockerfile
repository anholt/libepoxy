FROM debian:stretch-slim
MAINTAINER Emmanuele Bassi <ebassi@gmail.com>

RUN apt-get update -qq && \
    apt-get install --no-install-recommends -qq -y \
        ca-certificates \
        clang \
        gcc \
        libgl1-mesa-dev \
        libegl1-mesa-dev \
        libgles1-mesa-dev \
        libgles2-mesa-dev \
        libgl1-mesa-dri \
        locales \
        ninja-build \
        pkg-config \
        python3 \
        python3-pip \
        python3-setuptools \
        python3-wheel \
        xvfb && \
        rm -rf /usr/share/doc/* /usr/share/man/*

RUN locale-gen C.UTF-8 && /usr/sbin/update-locale LANG=C.UTF-8
ENV LANG=C.UTF-8 LANGUAGE=C.UTF-8 LC_ALL=C.UTF-8

RUN pip3 install meson

WORKDIR /root
