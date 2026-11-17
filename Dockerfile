# Build Linux AppImage with static glfw3.3.10
#     docker build -t nsae:01 . --output=.
#
# ./nsae.AppImage [nsae|nsaectl|...] [...]

### Linux Build
### {{{
FROM almalinux:8 AS linux-build-glfw
RUN dnf install -y epel-release
RUN dnf install -y clang cmake git pkgconf make wget unzip \
            wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel \
            libXinerama-devel libXrandr-devel
WORKDIR /glfw
RUN wget 'https://github.com/glfw/glfw/releases/download/3.3.10/glfw-3.3.10.zip' && \
    unzip 'glfw-3.3.10.zip'
RUN cmake -S 'glfw-3.3.10' -B build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/glfw-root && \
    make -C build && \
    make -C build install DESTDIR=/installdir

FROM almalinux:8 AS linux-build-nsae
RUN dnf install -y 'https://pkgs.sysadmins.ws/el8/base/x86_64/raven-release.el8.noarch.rpm'
RUN dnf install -y epel-release
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y clang cmake git pkgconf make meson vim-common \
        glew-devel portaudio-devel stb-devel zeromq-devel
WORKDIR /nsae
COPY . .
COPY --from=linux-build-glfw /installdir/glfw-root /glfw-root
RUN rm -rf build && \
    PKG_CONFIG_PATH=/glfw-root/lib64/pkgconfig \
            meson setup build --buildtype=release --prefix=/usr && \
    ninja -C build
RUN meson install -C build --destdir /installdir

FROM almalinux:8 AS package-appimage
RUN dnf install -y wget file
WORKDIR /appimage 
RUN wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage && \
    chmod +x appimagetool-x86_64.AppImage && \
    ./appimagetool-x86_64.AppImage --appimage-extract
COPY --from=linux-build-nsae /installdir                 /AppDir
COPY --from=linux-build-nsae /nsae/resources/AppRun      /AppDir
COPY --from=linux-build-nsae /lib64/libGLEW.so.2.2       /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libGLU.so.1          /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libportaudio.so.2    /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libzmq.so.5          /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libunwind.so.8       /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libsodium.so.23      /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libpgm-5.2.so.0      /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libpcre2-8.so.0      /AppDir/usr/lib64
RUN ln -fs usr/share/applications/nsae.desktop           /AppDir/nsae.desktop
RUN ln -fs usr/share/icons/hicolor/128x128/apps/nsae.png /AppDir/nsae.png
RUN chmod 755 -R /AppDir
RUN ARCH=x86_64 /appimage/squashfs-root/AppRun /AppDir /appimage/nsae-linux-x86_64.AppImage
### }}}

### Windows Build
### {{{
FROM almalinux:8 AS win-build-glfw
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y cmake wget unzip mingw32-gcc
WORKDIR /glfw
RUN wget 'https://github.com/glfw/glfw/releases/download/3.3.10/glfw-3.3.10.zip' && \
    unzip 'glfw-3.3.10.zip'
RUN cmake -S 'glfw-3.3.10' -B build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/dep-root -DGLFW_BUILD_WIN32=True \
            -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake
RUN mingw32-make -C build
RUN mingw32-make -C build install

FROM almalinux:8 AS win-build-glew
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y wget unzip make mingw32-gcc mingw32-crt mingw32-binutils
WORKDIR /glew
RUN wget 'https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1.zip' && \
    unzip 'glew-2.3.1.zip' && \
    sed -i 's,$(HOST)/lib,/$(HOST)/sys-root/mingw/lib,' glew-2.3.1/config/Makefile.linux-mingw32
RUN make -C glew-2.3.1 SYSTEM=linux-mingw32 
RUN make -C glew-2.3.1 install SYSTEM=linux-mingw32 GLEW_DEST=/dep-root
RUN sed -i 's/Requires.*//' /dep-root/lib/pkgconfig/glew.pc

FROM almalinux:8 AS win-build-portaudio
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y wget tar gzip cmake mingw32-gcc mingw32-crt mingw32-binutils \
        mingw32-gcc-c++
WORKDIR /portaudio
RUN wget 'https://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz' && \
    tar -xf pa_stable_v190700_20210406.tgz
RUN cmake -S 'portaudio' -B build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/dep-root \
            -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake
RUN mingw32-make -C build
RUN mingw32-make -C build install

FROM almalinux:8 AS win-build-zeromq
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y wget zip cmake mingw32-gcc mingw32-crt mingw32-binutils \
        mingw32-gcc-c++
WORKDIR /zeromq
RUN wget 'https://github.com/zeromq/libzmq/releases/download/v4.3.5/zeromq-4.3.5.zip' && \
    unzip 'zeromq-4.3.5.zip'
RUN cmake -S 'zeromq-4.3.5' -B build -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/dep-root \
            -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw/toolchain-mingw32.cmake \
            -DCMAKE_SYSTEM_NAME=WindowsStore -DCMAKE_SYSTEM_VERSION=10.0 \
            -DENABLE_CURVE=OFF -DZMQ_BUILD_TESTS=OFF
RUN mingw32-make -C build
RUN mingw32-make -C build install

FROM almalinux:8 AS win-build-nsae
RUN dnf install -y 'https://pkgs.sysadmins.ws/el8/base/x86_64/raven-release.el8.noarch.rpm'
RUN dnf install -y epel-release
RUN dnf config-manager --set-enabled powertools
RUN dnf install -y vim-common meson git cmake mingw32-gcc mingw32-crt \
        mingw32-binutils mingw32-pkg-config stb-devel
WORKDIR /nsae
COPY . .
COPY --from=win-build-glfw /dep-root /dep-root
COPY --from=win-build-glew /dep-root /dep-root
COPY --from=win-build-zeromq /dep-root /dep-root
COPY --from=win-build-portaudio /dep-root /dep-root
RUN cp -R /usr/include/stb/* /usr/i686-w64-mingw32/sys-root/mingw/include/
RUN rm -rf build && \
    PKG_CONFIG_PATH=/dep-root/lib/pkgconfig:/usr/lib64/pkgconfig \
    meson setup build --buildtype=release --prefix=/ \
            --cross-file /usr/share/mingw/toolchain-mingw32.meson && \
    ninja -C build
RUN meson install -C build --destdir /installdir

FROM almalinux:8 AS package-win
RUN dnf install -y zip
WORKDIR /archive
#COPY --from=win-build-glfw /dep-root ./nsae
COPY --from=win-build-glew /dep-root/lib/glew32.dll ./nsae/
#COPY --from=win-build-portaudio /dep-root ./nsae
#COPY --from=win-build-zeromq /dep-root ./nsae
COPY --from=win-build-nsae /installdir/bin ./nsae
RUN zip -r -9 nsae-portable-win32-i686.zip nsae/
### }}}

### Packaging 
FROM scratch
#COPY --from=package-appimage /appimage/nsae-linux-x86_64.AppImage .
COPY --from=package-win /archive/nsae-portable-win32-i686.zip .

# vim: fdm=marker
