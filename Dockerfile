FROM 998244353/portable-judge-base:latest
MAINTAINER Shiroha "keqing.hu@icloud.com"

ARG BIN_DIR=/portable
COPY ./build/main ${BIN_DIR}/main
COPY ./env ${BIN_DIR}/env
COPY ./run.sh ${BIN_DIR}/run.sh
WORKDIR ${BIN_DIR}

CMD ./main
