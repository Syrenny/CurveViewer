FROM ubuntu:22.04

RUN apt-get update && \
    apt-get install -y qt6-base-dev cmake g++ git && \
    apt-get clean

WORKDIR /app
COPY . /app

RUN cmake -Bbuild -H. && cmake --build build

CMD ["./build/CurveViewer"]
