# Build Linux AppImage with static glfw3.3.10
#     docker build -t nsae:01 . --output=.
#
# ./nsae.AppImage [nsae|nsaectl|...] [...]

### Linux Build
### {{{
FROM almalinux:10 AS linux-build-glfw
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

FROM almalinux:10 AS linux-build-nsae
RUN dnf install -y epel-release
RUN dnf install -y clang cmake pkgconf make meson xxd \
        glew-devel portaudio-devel stb-devel zeromq-devel
WORKDIR /nsae
COPY . .
COPY --from=linux-build-glfw /installdir/glfw-root /glfw-root
RUN PKG_CONFIG_PATH=/glfw-root/lib64/pkgconfig \
            meson setup build --wipe --buildtype=release --prefix=/usr && \
    ninja -C build
RUN meson install -C build --destdir /installdir
### }}}

### Packaging 
### {{{
FROM almalinux:10 AS package-appimage
RUN dnf install -y wget file
WORKDIR /appimage 
RUN wget https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-x86_64.AppImage && \
    chmod +x appimagetool-x86_64.AppImage && \
    ./appimagetool-x86_64.AppImage --appimage-extract
COPY --from=linux-build-nsae /installdir /AppDir
COPY --from=linux-build-nsae /nsae/resources/AppRun /AppDir
RUN ln -fs usr/share/applications/nsae.desktop /AppDir/nsae.desktop
RUN ln -fs usr/share/icons/hicolor/128x128/apps/nsae.png /AppDir/nsae.png
RUN ls -R /appimage
RUN chmod 755 -R /AppDir
RUN ARCH=x86_64 /appimage/squashfs-root/AppRun /AppDir /appimage/nsae-linux-x86_64.AppImage

FROM scratch
COPY --from=package-appimage /appimage/nsae-linux-x86_64.AppImage .
### }}}

# vim: fdm=marker
