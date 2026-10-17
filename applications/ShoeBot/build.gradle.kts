plugins {
    id("com.android.application")
}

android {
    namespace = "ioio.applications.shoebot"
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

    implementation("androidx.appcompat:appcompat:1.6.1")
}
