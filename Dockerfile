FROM centos:latest

RUN yum -y install vim
RUN mkdir -p /src

# COPY Pine /src
# COPY  /src
# COPY server.conf /src
COPY . /src

WORKDIR /src

EXPOSE 9996