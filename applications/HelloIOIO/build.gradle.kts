plugins {
    id("com.android.application")
}

android {
    namespace = "ioio.examples.hello"

    defaultConfig {
        minSdk = 21
        compileSdk = 34
        targetSdk = 34

        versionCode = getGitCommitCount()
        versionName = getTag()

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        testInstrumentationRunnerArguments["useTestStorageService"] = "true"
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    packaging {
        jniLibs {
            useLegacyPackaging = true
        }
    }
}

dependencies {
    val versionLib = "6.5"
    implementation("com.github.ytai.ioio:IOIOLibAndroidBluetooth:${versionLib}")
    implementation("com.github.ytai.ioio:IOIOLibAndroidAccessory:${versionLib}")
    implementation("com.github.ytai.ioio:IOIOLibAndroidDevice:${versionLib}")
    implementation("com.github.ytai.ioio:IOIOLibAndroid:${versionLib}")
    implementation("com.github.ytai.ioio:IOIOLibCore:${versionLib}")

    implementation("androidx.appcompat:appcompat:1.6.1")

    androidTestImplementation("androidx.test.ext:junit-ktx:1.1.5")
    androidTestUtil("androidx.test.services:test-services:1.4.2")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}
