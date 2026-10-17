plugins {
    id("java")
}

val distJar = tasks.register<Jar>("distJar") {
    dependsOn(configurations.getByName("runtimeClasspath"))
    archiveClassifier = "standalone"
    archiveVersion = ""
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    manifest {
        attributes["Main-Class"] = "ioio.bridge.Main"
    }
    from(sourceSets.main.get().output)
    from({
        configurations.getByName("runtimeClasspath").filter { it.exists() }.map { if (it.isDirectory) it else zipTree(it) }
    })
}

tasks.register<Zip>("zip") {
    dependsOn(distJar)
    archiveBaseName = "IOIOBridge"
    archiveVersion = "0103"

    from(distJar)
    from("ioiobridge", "ioiobridge.bat")
    rename("IOIOBridge-standalone.jar", "ioiobridge.jar")
}

tasks.named("build") {
    dependsOn("zip")
}

dependencies {
    implementation("com.sparetimelabs:purejavacomm:1.0.1")
}
