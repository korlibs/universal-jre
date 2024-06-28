#!/bin/bash

#wget https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21%2B35/OpenJDK21U-jre_aarch64_mac_hotspot_21_35.tar.gz
#wget https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21%2B35/OpenJDK21U-jre_x64_mac_hotspot_21_35.tar.gz
#wget https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21%2B35/OpenJDK21U-jdk_aarch64_mac_hotspot_21_35.tar.gz

die() {
    echo "${0##*/}: $@" >&2
    exit 1
}


if [ ! -f "microsoft-jdk-21.0.3-macos-aarch64.tar.gz" ]; then
    wget https://aka.ms/download-jdk/microsoft-jdk-21.0.3-macos-aarch64.tar.gz
fi

if [ ! -f "microsoft-jdk-21.0.3-macos-x64.tar.gz" ]; then
    wget https://aka.ms/download-jdk/microsoft-jdk-21.0.3-macos-x64.tar.gz
fi

#rm -rf aarch64; rm -rf x64

if [ ! -d "aarch64/jdk-21" ]; then
    mkdir -p aarch64/jdk-21 > /dev/null 2>&1
    tar --strip-components 4 -xzf microsoft-jdk-21.0.3-macos-aarch64.tar.gz -C aarch64/jdk-21
fi

if [ ! -d "x64/jdk-21" ]; then
    mkdir -p x64/jdk-21 > /dev/null 2>&1
    tar --strip-components 4 -xzf microsoft-jdk-21.0.3-macos-x64.tar.gz -C x64/jdk-21
fi

if [ ! -f "./aarch64/jre-21/bin/java" ]; then
    ./aarch64/jdk-21/bin/jlink --add-modules java.base,java.compiler,java.datatransfer,java.desktop,java.instrument,java.logging,java.management,java.management.rmi,java.naming,java.net.http,java.prefs,java.rmi,java.scripting,java.se,java.security.jgss,java.security.sasl,java.smartcardio,java.sql,java.sql.rowset,java.transaction.xa,java.xml,java.xml.crypto,jdk.accessibility,jdk.charsets,jdk.crypto.cryptoki,jdk.crypto.ec,jdk.dynalink,jdk.httpserver,jdk.internal.vm.ci,jdk.internal.vm.compiler,jdk.internal.vm.compiler.management,jdk.jdwp.agent,jdk.jfr,jdk.jsobject,jdk.localedata,jdk.management,jdk.management.agent,jdk.management.jfr,jdk.naming.dns,jdk.naming.rmi,jdk.net,jdk.nio.mapmode,jdk.sctp,jdk.security.auth,jdk.security.jgss,jdk.unsupported,jdk.xml.dom,jdk.zipfs --output ./aarch64/jre-21 --strip-debug --no-man-pages --no-header-files --compress=0
fi

if [ ! -f "./x64/jre-21/bin/java" ]; then
    ./x64/jdk-21/bin/jlink --add-modules java.base,java.compiler,java.datatransfer,java.desktop,java.instrument,java.logging,java.management,java.management.rmi,java.naming,java.net.http,java.prefs,java.rmi,java.scripting,java.se,java.security.jgss,java.security.sasl,java.smartcardio,java.sql,java.sql.rowset,java.transaction.xa,java.xml,java.xml.crypto,jdk.accessibility,jdk.charsets,jdk.crypto.cryptoki,jdk.crypto.ec,jdk.dynalink,jdk.httpserver,jdk.internal.vm.ci,jdk.internal.vm.compiler,jdk.internal.vm.compiler.management,jdk.jdwp.agent,jdk.jfr,jdk.jsobject,jdk.localedata,jdk.management,jdk.management.agent,jdk.management.jfr,jdk.naming.dns,jdk.naming.rmi,jdk.net,jdk.nio.mapmode,jdk.sctp,jdk.security.auth,jdk.security.jgss,jdk.unsupported,jdk.xml.dom,jdk.zipfs --output ./x64/jre-21 --strip-debug --no-man-pages --no-header-files --compress=0
fi

if [ ! -f "./universal/jre-21/bin/java" ]; then
    echo "Creating universal JRE..."
    rm -rf universal
    mkdir universal
    find aarch64 -type f | while read arm_file ; do
        noarch_file=${arm_file#aarch64/}
        mkdir -p universal/${noarch_file%/*}
        if file $arm_file | grep "Mach-O.\+arm64" ; then
            # Create universal binary from both x86_64 and arm64
            lipo -create -output universal/$noarch_file x64/$noarch_file $arm_file
            if file $arm_file | grep executable ; then
                chmod 755 universal/$noarch_file
            fi
        else
            # Not a file with binary code, copy it as it is
            cp $arm_file universal/$noarch_file
        fi
    done
fi

if [ ! -f "./microsoft-jre-21.0.3-mac-universal.tar.xz" ]; then
    tar -cJf microsoft-jre-21.0.3-mac-universal.tar.xz -C universal jre-21
fi