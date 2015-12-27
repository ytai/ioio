@adb forward tcp:4545 tcp:4545 && java -Done-jar.silent=true  -jar %~dp0\ioiobridge.jar %*
