plugins {
    id("com.android.application")
}

android {
    namespace = "ioio.connection_tester"
    defaultConfig {
        minSdk = 14
        compileSdk = 34

        versionCode = getGitCommitCount()
        versionName = getTag()
    }
}

dependencies {
    implementation("com.google.guava:guava:32.1.3-jre")
    implementation(project(":IOIOConnectionTester"))

    implementation(project(":IOIOLibAndroidBluetooth"))
    implementation(project(":IOIOLibAndroidAccessory"))

    implementation("androidx.appcompat:appcompat:1.6.1")
}
