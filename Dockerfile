FROM lockblox/orthodox:latest

RUN cd ${VCPKG_ROOT} \
 && git pull \
 && ./vcpkg update \
 && ./vcpkg install boost-iterator \
 && ./vcpkg install boost-tokenizer \
 && ./vcpkg install boost-range

COPY . /root/src