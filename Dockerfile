FROM ubuntu:latest AS builder
RUN apt-get update
RUN apt-get install -y git gcc make wget automake libtool
RUN cd /root && wget https://github.com/libsdl-org/SDL/releases/download/release-2.28.5/SDL2-2.28.5.tar.gz
RUN cd /root && wget https://dist.libuv.org/dist/v1.47.0/libuv-v1.47.0.tar.gz
RUN cd /root && tar xzf SDL2-2.28.5.tar.gz && cd SDL2-2.28.5 && ./configure && make all install
RUN cd /root && tar xzf libuv-v1.47.0.tar.gz && cd libuv-v1.47.0 && ./autogen.sh && ./configure && make all install
RUN cd /root &&  git clone https://github.com/lispware/minilisp.git -b libuv2

FROM ubuntu:latest
COPY --from=builder /usr /usr
COPY --from=builder /etc /etc
RUN cd /root &&  git clone https://github.com/lispware/minilisp.git -b libuv2

# docker build -t minipicolisp .
