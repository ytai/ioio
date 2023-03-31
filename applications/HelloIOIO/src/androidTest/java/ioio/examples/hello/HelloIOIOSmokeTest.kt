package ioio.examples.hello

import androidx.test.core.graphics.writeToTestStorage
import androidx.test.espresso.Espresso.onView
import androidx.test.espresso.assertion.ViewAssertions.matches
import androidx.test.espresso.matcher.ViewMatchers
import androidx.test.espresso.matcher.ViewMatchers.isDisplayed
import androidx.test.espresso.matcher.ViewMatchers.withId
import androidx.test.espresso.screenshot.captureToBitmap
import androidx.test.ext.junit.rules.activityScenarioRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import org.junit.Rule
import org.junit.Test
import org.junit.rules.TestName
import org.junit.runner.RunWith

@RunWith(AndroidJUnit4::class)
class HelloIOIOSmokeTest {

    @get:Rule
    val activityScenarioRule = activityScenarioRule<MainActivity>()

    var nameRule = TestName()

    @Test
    fun smokeTestSimplyStart() {
        onView(withId(R.id.button)).check(matches(isDisplayed()))
        onView(ViewMatchers.isRoot())
            .captureToBitmap()
            .writeToTestStorage("${javaClass.simpleName}_${nameRule.methodName}")
    }
}
