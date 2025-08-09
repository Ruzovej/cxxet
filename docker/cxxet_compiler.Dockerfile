FROM debian:bookworm

RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
        ca-certificates \
        clang-19 \
        cmake \
        curl \
        git \
        less \
        libclang-19-dev \
        libclang-rt-19-dev \
        make \
        patch \
        ninja-build \
        vim \
    && rm -rf /var/lib/apt/lists/*

RUN ln -s /usr/bin/clang-19 /usr/local/bin/clang \
    && ln -s /usr/bin/clang++-19 /usr/local/bin/clang++

ENV LANG=C.UTF-8

ARG USER_ID
ARG GROUP_ID
ARG USER_NAME
ARG GROUP_NAME

RUN groupadd \
        --gid "${GROUP_ID:?}" \
        "${GROUP_NAME:?}" \
    && useradd \
        --uid "${USER_ID:?}" \
        --gid "${GROUP_ID:?}" \
        --no-create-home \
        --shell /bin/bash \
        "${USER_NAME:?}"

ENV CXXET_AVOID_DOCKER=1
