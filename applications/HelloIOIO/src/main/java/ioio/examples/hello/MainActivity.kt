package ioio.examples.hello

import android.content.Context
import android.os.Bundle
import android.widget.Toast
import android.widget.ToggleButton
import ioio.lib.api.DigitalOutput
import ioio.lib.api.IOIO
import ioio.lib.api.IOIO.VersionType
import ioio.lib.api.exception.ConnectionLostException
import ioio.lib.util.BaseIOIOLooper
import ioio.lib.util.IOIOLooper
import ioio.lib.util.android.IOIOActivity

/**
 * This is the main activity of the HelloIOIO example application.
 *
 *
 * It displays a toggle button on the screen, which enables control of the
 * on-board LED. This example shows a very simple usage of the IOIO, by using
 * the [IOIOActivity] class. For a more advanced use case, see the
 * HelloIOIOPower example.
 */
class MainActivity : IOIOActivity() {
    private var toggleButton: ToggleButton? = null
    private var numConnected = 0

    /**
     * Called when the activity is first created. Here we normally initialize
     * our GUI.
     */
    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.main)
        toggleButton = findViewById(R.id.button)
    }

    /**
     * A method to create our IOIO thread.
     *
     * @see ioio.lib.util.AbstractIOIOActivity.createIOIOThread
     */
    override fun createIOIOLooper(): IOIOLooper {
        return Looper()
    }

    private fun showVersions(ioio: IOIO, title: String) {
        toast(
            String.format(
                """
    %s
    IOIOLib: %s
    Application firmware: %s
    Bootloader firmware: %s
    Hardware: %s
    """.trimIndent(),
                title,
                ioio.getImplVersion(VersionType.IOIOLIB_VER),
                ioio.getImplVersion(VersionType.APP_FIRMWARE_VER),
                ioio.getImplVersion(VersionType.BOOTLOADER_VER),
                ioio.getImplVersion(VersionType.HARDWARE_VER)
            )
        )
    }

    private fun toast(message: String) {
        val context: Context = this
        runOnUiThread { Toast.makeText(context, message, Toast.LENGTH_LONG).show() }
    }

    private fun enableUi(enable: Boolean) {
        // This is slightly trickier than expected to support a multi-IOIO use-case.
        runOnUiThread {
            if (enable) {
                if (numConnected++ == 0) {
                    toggleButton!!.isEnabled = true
                }
            } else {
                if (--numConnected == 0) {
                    toggleButton!!.isEnabled = false
                }
            }
        }
    }

    /**
     * This is the thread on which all the IOIO activity happens. It will be run
     * every time the application is resumed and aborted when it is paused. The
     * method setup() will be called right after a connection with the IOIO has
     * been established (which might happen several times!). Then, loop() will
     * be called repetitively until the IOIO gets disconnected.
     */
    internal inner class Looper : BaseIOIOLooper() {
        /**
         * The on-board LED.
         */
        private var digitalOutput: DigitalOutput? = null

        /**
         * Called every time a connection with IOIO has been established.
         * Typically used to open pins.
         *
         * @throws ConnectionLostException When IOIO connection is lost.
         * @see ioio.lib.util.IOIOLooper.setup
         */
        @Throws(ConnectionLostException::class)
        override fun setup() {
            showVersions(ioio_, "IOIO connected!")
            digitalOutput = ioio_.openDigitalOutput(0, true)
            enableUi(true)
        }

        /**
         * Called repetitively while the IOIO is connected.
         *
         * @throws ConnectionLostException When IOIO connection is lost.
         * @throws InterruptedException    When the IOIO thread has been interrupted.
         * @see ioio.lib.util.IOIOLooper.loop
         */
        @Throws(ConnectionLostException::class, InterruptedException::class)
        override fun loop() {
            digitalOutput!!.write(!toggleButton!!.isChecked)
            Thread.sleep(100)
        }

        /**
         * Called when the IOIO is disconnected.
         *
         * @see ioio.lib.util.IOIOLooper.disconnected
         */
        override fun disconnected() {
            enableUi(false)
            toast("IOIO disconnected")
        }

        /**
         * Called when the IOIO is connected, but has an incompatible firmware version.
         *
         * @see ioio.lib.util.IOIOLooper.incompatible
         */
        @Deprecated("Deprecated in Java")
        override fun incompatible() {
            showVersions(ioio_, "Incompatible firmware version!")
        }
    }
}
