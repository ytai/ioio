plugins {
    id("java")
}

val distJar = tasks.register<Jar>("distJar") {
    dependsOn(configurations.getByName("runtimeClasspath"))
    archiveClassifier = "standalone"
    archiveVersion = ""
    duplicatesStrategy = DuplicatesStrategy.EXCLUDE
    manifest {
        attributes["Main-Class"] = "ioio.connection_tester_pc.IOIOConnectionTesterMain"
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
    implementation(project(":IOIOConnectionTester"))
    implementation("com.google.guava:guava:32.1.3-jre")
}
