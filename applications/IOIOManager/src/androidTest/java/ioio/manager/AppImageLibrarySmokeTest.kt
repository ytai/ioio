package ioio.manager

import androidx.test.core.graphics.writeToTestStorage
import androidx.test.espresso.Espresso
import androidx.test.espresso.assertion.ViewAssertions
import androidx.test.espresso.matcher.ViewMatchers
import androidx.test.espresso.matcher.ViewMatchers.isRoot
import androidx.test.espresso.screenshot.captureToBitmap
import androidx.test.ext.junit.rules.activityScenarioRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TestName
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class AppImageLibrarySmokeTest {

    @get:Rule
    val activityScenarioRule = activityScenarioRule<AppImageLibraryActivity>()

    var nameRule = TestName()

    @Test
    fun smokeTestSimplyStart() {
        Espresso.onView(ViewMatchers.withId(android.R.id.empty)).check(ViewAssertions.matches(ViewMatchers.isDisplayed()))
        Espresso.onView(isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}")
    }
}
