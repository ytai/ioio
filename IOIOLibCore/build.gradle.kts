import java.util.Properties

plugins {
    id("java")
    id("maven-publish")
}

tasks.register("generateVersionFile", GenerateVersion::class) {
    version = versionString
    outputFile = layout.buildDirectory.file("classes/main/version.properties").get().asFile
}

tasks.named("classes") {
    dependsOn("generateVersionFile")
}

tasks.javadoc {
    source = fileTree("src/main/java") { include("ioio/lib/api/**") }
    isFailOnError = false
}

val javadocJar = tasks.register<Jar>("javadocJar") {
    archiveClassifier = "javadoc"
    from(tasks.javadoc)
}

val sourcesJar = tasks.register<Jar>("sourcesJar") {
    archiveClassifier = "sources"
    from(sourceSets.main.get().allSource)
}

artifacts {
    add("archives", javadocJar)
    add("archives", sourcesJar)
}

open class GenerateVersion : DefaultTask() {
    @Input
    lateinit var version: String

    @OutputFile
    lateinit var outputFile: File

    @TaskAction
    fun generate() {
        val properties = Properties()
        properties.setProperty("version", version)
        outputFile.writer().use { properties.store(it, null) }
    }
}

publishing {
    publications {
        create<MavenPublication>("release") {
            from(components["java"])
            artifact(javadocJar)
            artifact(sourcesJar)
        }
    }
}

java {
    sourceCompatibility = JavaVersion.VERSION_1_8
    targetCompatibility = JavaVersion.VERSION_1_8
}

val versionString: String
    get() = "IOIO${project.version}"
