import org.gradle.api.initialization.resolve.RepositoriesMode

rootProject.name = "IOIO"

// PC applications.
val pcApplications = listOf(
    "HelloIOIOSwing",
    "HelloIOIOConsole",
    "HelloSequencer",
    "IOIOServoSwing",
    "SeeedTestbed",
    "IOIOConnectionTesterPC"
)

// Android applications
val androidApplications = listOf(
    "HelloIOIO",
    "HelloIOIOService",
    "HolidayIOIO",
    "IOIOManager",
    "IOIOSimpleApp",
    "IOIOTestBed",
    "IOIOTortureTest",
    "ShoeBot",
    "IOIOConnectionTesterAndroid"
)

// Application support libraries.
val appSupportLibraries = listOf(
    "IOIOConnectionTester"
)

// Include statements and projectDir settings.
androidApplications.forEach { app ->
    include(app)
    project(":$app").projectDir = file("applications/$app")
}

pcApplications.forEach { app ->
    include(app)
    project(":$app").projectDir = file("applications/pc/$app")
}

appSupportLibraries.forEach { app ->
    include(app)
    project(":$app").projectDir = file("applications/$app")
}

// Core library.
include("IOIOLibCore")

// PC library.
include("IOIOLibPC")

// Android libraries.
include("IOIOLibAndroid", "IOIOLibAndroidBluetooth", "IOIOLibAndroidAccessory", "IOIOLibAndroidDevice")

// PC tools.
include("IOIODude", "IOIOBridge")

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
        maven {
            url = uri("http://www.sparetimelabs.com/maven2")
            isAllowInsecureProtocol = true
        }
        maven { url = uri("https://jitpack.io") }
    }
}
