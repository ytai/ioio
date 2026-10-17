plugins {
    id("java")
}

val distJar = tasks.register<Jar>("distJar") {
    dependsOn(configurations.getByName("runtimeClasspath"))
    archiveClassifier = "standalone"
    archiveVersion = ""
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    manifest {
        attributes["Main-Class"] = "ioio.dude.IOIODudeMain"
    }
    from(sourceSets.main.get().output)
    from({
        configurations.getByName("runtimeClasspath").filter { it.exists() }.map { if (it.isDirectory) it else zipTree(it) }
    })
}

tasks.register<Zip>("zip") {
    dependsOn(distJar)
    archiveBaseName = "IOIODude"
    archiveVersion = "0103"

    from(distJar)
    from("ioiodude", "ioiodude.bat")
    rename("IOIODude-standalone.jar", "ioiodude.jar")
}

tasks.named("build") {
    dependsOn("zip")
}

dependencies {
    implementation("com.sparetimelabs:purejavacomm:1.0.1")
}
