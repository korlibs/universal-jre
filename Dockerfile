# docker build .
FROM ubuntu

WORKDIR /home/build

RUN apt-get update && apt-get install curl binutils -y

RUN curl -LO https://aka.ms/download-jdk/microsoft-jdk-21.0.3-linux-x64.tar.gz
RUN mkdir jdk-21
RUN tar --strip-components 1 -xf microsoft-jdk-21.0.3-linux-x64.tar.gz -C jdk-21

RUN /home/build/jdk-21/bin/jlink --add-modules java.base,java.compiler,java.datatransfer,java.desktop,java.instrument,java.logging,java.management,java.management.rmi,java.naming,java.net.http,java.prefs,java.rmi,java.scripting,java.se,java.security.jgss,java.security.sasl,java.smartcardio,java.sql,java.sql.rowset,java.transaction.xa,java.xml,java.xml.crypto,jdk.accessibility,jdk.charsets,jdk.crypto.cryptoki,jdk.crypto.ec,jdk.dynalink,jdk.httpserver,jdk.internal.vm.ci,jdk.internal.vm.compiler,jdk.internal.vm.compiler.management,jdk.jdwp.agent,jdk.jfr,jdk.jsobject,jdk.localedata,jdk.management,jdk.management.agent,jdk.management.jfr,jdk.naming.dns,jdk.naming.rmi,jdk.net,jdk.nio.mapmode,jdk.sctp,jdk.security.auth,jdk.security.jgss,jdk.unsupported,jdk.xml.dom,jdk.zipfs --output /home/build/jre-21 --strip-debug --no-man-pages --no-header-files --compress=0

RUN tar -cJf /home/build/jre-21.0.3+9.tar.xz /home/build/jre-21
