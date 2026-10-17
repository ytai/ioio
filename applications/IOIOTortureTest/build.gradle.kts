plugins {
    id("com.android.application")
}

android {
    namespace = "ioio.tests.torture"
    defaultConfig {
        minSdk = 14
        compileSdk = 34

        versionCode = getGitCommitCount()
        versionName = getTag()
    }
}

dependencies {
    implementation(project(":IOIOLibAndroidBluetooth"))
    implementation(project(":IOIOLibAndroidAccessory"))
    implementation(project(":IOIOLibAndroidDevice"))

    implementation("androidx.appcompat:appcompat:1.6.1")
}
