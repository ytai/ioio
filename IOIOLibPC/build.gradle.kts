plugins {
    id("java-library")
}

tasks.javadoc {
    source = fileTree("src/main/java")
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
    add("archives", sourcesJar)
    add("archives", javadocJar)
}

dependencies {
    implementation("com.sparetimelabs:purejavacomm:1.0.1")
    api(project(":IOIOLibCore"))
}

java {
    sourceCompatibility = JavaVersion.VERSION_1_8
    targetCompatibility = JavaVersion.VERSION_1_8
}
