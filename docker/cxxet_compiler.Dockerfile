FROM debian:bookworm

RUN apt update \
    && apt install -y  \
        clang-19 \
        cmake \
        git \
        make \
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
