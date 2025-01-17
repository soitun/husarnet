FROM ubuntu:22.04 AS app_builder
ARG TARGETPLATFORM
RUN echo $TARGETPLATFORM

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends git ca-certificates python3 linux-headers-generic ninja-build cmake build-essential g++-mingw-w64

# This is unfortunate but... (see https://stackoverflow.com/a/38751292 )
RUN ln -sf /usr/include/asm-generic /usr/include/asm

WORKDIR /husarnet
# note: will assume context as . (repo root)
COPY . .
RUN ./util/build-cmake.sh $TARGETPLATFORM

# stage 2
FROM ubuntu:22.04
RUN apt-get update -y && apt-get install -y iptables procps iproute2
RUN update-alternatives --set ip6tables /usr/sbin/ip6tables-nft

COPY --from=app_builder /husarnet/build/release/husarnet-daemon /usr/bin/husarnet-daemon
COPY ./platforms/docker/husarnet-docker.sh /usr/bin/husarnet-docker

CMD husarnet-docker
