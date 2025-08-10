ARG DEBIAN_CODENAME=bookworm

FROM debian:${DEBIAN_CODENAME}

ARG CLANG_VERSION=19

RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
        ca-certificates \
        clang-${CLANG_VERSION} \
# TODO (https://github.com/Ruzovej/cxxet/issues/11) maybe fix exact version, so reformats doesn't happen "unexpectedly":
        clang-format-${CLANG_VERSION} \
        cmake \
        curl \
        gdb \
        git \
        jq \
        less \
        libasan8 \
        libclang-rt-${CLANG_VERSION}-dev \
        libldap-common \
        liblsan0 \
        libtsan2 \
        libubsan1 \
        lld-${CLANG_VERSION} \
        lldb-${CLANG_VERSION} \
        llvm-${CLANG_VERSION} \
        llvm-${CLANG_VERSION}-linker-tools \
        llvm-${CLANG_VERSION}-tools \
        make \
        ninja-build \
        netbase \
        patch \
        publicsuffix \
        ssh-client \
        strace \
        vim \
    && rm -rf /var/lib/apt/lists/*

RUN install_alternative() { update-alternatives --install "/usr/bin/$1" "$1" "/usr/bin/$1-$2" 100; } \
    && install_alternative clang ${CLANG_VERSION} \
    && install_alternative clang-format ${CLANG_VERSION} \
    && install_alternative clang++ ${CLANG_VERSION} \
    && install_alternative ld.lld ${CLANG_VERSION} \
    && install_alternative lldb ${CLANG_VERSION} \
    && install_alternative llvm-addr2line ${CLANG_VERSION} \
    && install_alternative llvm-ar ${CLANG_VERSION} \
    && install_alternative llvm-nm ${CLANG_VERSION} \
    && update-alternatives --install /usr/bin/nm nm "/usr/bin/llvm-nm-${CLANG_VERSION}" 100 \
    && install_alternative llvm-objcopy ${CLANG_VERSION} \
    && install_alternative llvm-objdump ${CLANG_VERSION} \
    && install_alternative llvm-ranlib ${CLANG_VERSION} \
    && install_alternative llvm-readelf ${CLANG_VERSION} \
    && install_alternative llvm-strip ${CLANG_VERSION}

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

# TODO (#56) later merge into the first such command ...
RUN apt update \
    && apt upgrade -y \
    && apt install -y --no-install-recommends \
        abigail-tools \
    && rm -rf /var/lib/apt/lists/*
