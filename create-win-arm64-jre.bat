@echo off

SETLOCAL

IF NOT EXIST microsoft-jdk-21.0.3-windows-aarch64.zip (
    curl -LO https://aka.ms/download-jdk/microsoft-jdk-21.0.3-windows-aarch64.zip
)

IF NOT EXIST jdk-21.0.3+9 (
    tar -vxf microsoft-jdk-21.0.3-windows-aarch64.zip
)

IF NOT EXIST jre-21.0.3+9 (
    jdk-21.0.3+9\bin\jlink --add-modules java.base,java.compiler,java.datatransfer,java.desktop,java.instrument,java.logging,java.management,java.management.rmi,java.naming,java.net.http,java.prefs,java.rmi,java.scripting,java.se,java.security.jgss,java.security.sasl,java.smartcardio,java.sql,java.sql.rowset,java.transaction.xa,java.xml,java.xml.crypto,jdk.accessibility,jdk.charsets,jdk.crypto.cryptoki,jdk.crypto.ec,jdk.dynalink,jdk.httpserver,jdk.internal.vm.ci,jdk.internal.vm.compiler,jdk.internal.vm.compiler.management,jdk.jdwp.agent,jdk.jfr,jdk.jsobject,jdk.localedata,jdk.management,jdk.management.agent,jdk.management.jfr,jdk.naming.dns,jdk.naming.rmi,jdk.net,jdk.nio.mapmode,jdk.sctp,jdk.security.auth,jdk.security.jgss,jdk.unsupported,jdk.xml.dom,jdk.zipfs --output jre-21.0.3+9 --strip-debug --no-man-pages --no-header-files --compress=0
)

IF NOT EXIST jre-21.0.3+9.tar.xz (
    tar -cJf jre-21.0.3+9.tar.xz jre-21.0.3+9
)
