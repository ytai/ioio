plugins {
    id("java")
}

val distJar = tasks.register<Jar>("distJar") {
    dependsOn(configurations.getByName("runtimeClasspath"))
    archiveClassifier = "standalone"
    archiveVersion = ""
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    manifest {
        attributes["Main-Class"] = "ioio.examples.servo_swing.IOIOServoSwing"
    }
    from(sourceSets.main.get().output)
    from({
        configurations.getByName("runtimeClasspath").filter { it.exists() }.map { if (it.isDirectory) it else zipTree(it) }
    })
}

tasks.named("build") {
    dependsOn(distJar)
}

dependencies {
    implementation(project(":IOIOLibPC"))
}
