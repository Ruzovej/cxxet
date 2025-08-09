FROM debian:bookworm

RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
        ca-certificates \
        clang-19 \
# TODO maybe fix exact version, so reformats doesn't happen "unexpectedly":
        clang-format-19 \
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
    && ln -s /usr/bin/clang++-19 /usr/local/bin/clang++ \
    && ln -s /usr/bin/clang-format-19 /usr/local/bin/clang-format

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
