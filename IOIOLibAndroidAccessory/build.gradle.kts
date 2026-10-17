plugins {
    id("com.android.library")
    id("maven-publish")
}

android {
    namespace = "ioio.lib.accessory"
    defaultConfig {
        minSdk = 14
        compileSdk = 34
    }
    publishing {
        singleVariant("release") {
            withSourcesJar()
        }
    }
}

dependencies {
    api(project(":IOIOLibAndroid"))
}

publishing {
    publications {
        create<MavenPublication>("release") {
            afterEvaluate {
                from(components.findByName("release"))
            }
        }
    }
}
