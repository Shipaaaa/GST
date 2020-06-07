FROM centos:latest

RUN yum update -y && \
    yum group install "Development Tools" -y && \
    yum clean all

WORKDIR /openmp_with_tcp

ADD ./openmp_with_tcp.c  /openmp_with_tcp
ADD ./utils.h  /openmp_with_tcp
ADD ./utils.c  /openmp_with_tcp

RUN mkdir -p /openmp_with_tcp/results/openmp_with_tcp

RUN gcc -fopenmp openmp_with_tcp.c utils.c -o openmp_with_tcp.out
CMD $PWD/openmp_with_tcp.out $(getent hosts host.docker.internal | awk '{ print $1 }') ./results/openmp_with_tcp/500mb