docker build -t build_linux_jre_x64 -f Dockerfile.linux.x64 .
container_id=$(docker create "build_linux_jre_x64")
docker cp $container_id:/home/build/microsoft-jre-21.0.3-linux-x64.tar.xz microsoft-jre-21.0.3-linux-x64.tar.xz
