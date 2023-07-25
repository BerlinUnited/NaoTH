#
# docker build -t simspark .
# docker run --rm -p 3200:3200 -p 3100:3100 simspark
#

FROM ubuntu:18.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libfreetype6 libode6 libsdl1.2debian ruby libdevil1c2 qt4-default nano \
    libboost-regex1.65.1 libboost-system1.65.1 libboost-thread1.65.1 \
    g++ cmake git libfreetype6-dev libode-dev libsdl-dev ruby-dev libdevil-dev libboost-dev libboost-thread-dev libboost-regex-dev libboost-system-dev libqt4-opengl-dev \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

RUN git clone \
      --depth 1 https://github.com/BerlinUnited/SimSpark-SPL.git /tmp/SimSpark \
    && mkdir -p /tmp/SimSpark/spark/build \
    && cd /tmp/SimSpark/spark/build \
    && cmake -DRVDRAW=OFF -DCMAKE_CXX_FLAGS=-w .. \
    && make -j$(nproc) \
    && make install \
    && ldconfig \
    && mkdir -p /tmp/SimSpark/rcssserver3d/build \
    && cd /tmp/SimSpark/rcssserver3d/build \
    && cmake -DRVDRAW=OFF -DCMAKE_CXX_FLAGS=-w .. \
    && make -j$(nproc) \
    && make install && ldconfig \
    && rm -fr /tmp/SimSpark \
    && apt-get purge -y --auto-remove $buildDeps \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/* \
    && sed -i 's/\$enableInternalMonitor = true/\$enableInternalMonitor = false/g' /usr/local/share/rcssserver3d/rcssserverspl.rb

ENV LD_LIBRARY_PATH=/usr/local/lib/simspark:/usr/local/lib/rcssserver3d

ENTRYPOINT ["rcssserver3d"]

EXPOSE 3100
EXPOSE 3200
