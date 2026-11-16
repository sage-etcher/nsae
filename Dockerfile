# Build Linux AppImage with static glfw3.3.10
#     docker build -t nsae:01 . --output=.
#
# ./nsae.AppImage [nsae|nsaectl|...] [...]

### Linux Build
### {{{
FROM almalinux:8 AS linux-build-glfw
RUN dnf install -y epel-release
RUN dnf install -y clang cmake pkgconf make wget unzip \
            wayland-devel libxkbcommon-devel libXcursor-devel libXi-devel \
            libXinerama-devel libXrandr-devel
WORKDIR /glfw3.3.10
RUN wget https://github.com/glfw/glfw/releases/download/3.3.10/glfw-3.3.10.zip && \
    unzip glfw-3.3.10.zip
RUN cmake -S glfw-3.3.10 -B build -DCMAKE_BUILD_TYPE=Release \
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
#RUN LD_LIBRARY_PATH=/installdir/usr/lib64 ldd /installdir/usr/bin/nsae |tee deps.txt
### }}}

### Packaging 
### {{{
FROM almalinux:8 AS package-appimage
RUN dnf install -y wget file
WORKDIR /appimage 
RUN wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage && \
    chmod +x appimagetool-x86_64.AppImage && \
    ./appimagetool-x86_64.AppImage --appimage-extract
COPY --from=linux-build-nsae /installdir                /AppDir
COPY --from=linux-build-nsae /nsae/resources/AppRun     /AppDir
COPY --from=linux-build-nsae /lib64/libGLEW.so.2.2    /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libGLU.so.1       /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libportaudio.so.2 /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libzmq.so.5       /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libunwind.so.8    /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libsodium.so.23   /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libpgm-5.2.so.0   /AppDir/usr/lib64
COPY --from=linux-build-nsae /lib64/libpcre2-8.so.0   /AppDir/usr/lib64
RUN ln -fs usr/share/applications/nsae.desktop          /AppDir/nsae.desktop
RUN ln -fs usr/share/icons/hicolor/128x128/apps/nsae.png /AppDir/nsae.png
RUN chmod 755 -R /AppDir
RUN ARCH=x86_64 /appimage/squashfs-root/AppRun /AppDir /appimage/nsae-linux-x86_64.AppImage

FROM scratch
COPY --from=package-appimage /appimage/nsae-linux-x86_64.AppImage .
#COPY --from=linux-build-nsae /nsae/deps.txt .
### }}}

# vim: fdm=marker
