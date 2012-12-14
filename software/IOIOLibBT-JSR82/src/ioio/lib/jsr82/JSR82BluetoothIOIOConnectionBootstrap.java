/*
 * Copyright 2011 Ytai Ben-Tsvi. All rights reserved.
 *  
 * 
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 * 
 *    1. Redistributions of source code must retain the above copyright notice, this list of
 *       conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright notice, this list
 *       of conditions and the following disclaimer in the documentation and/or other materials
 *       provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ARSHAN POURSOHI OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied.
 */

package ioio.lib.jsr82;

import ioio.lib.api.IOIOConnection;
import ioio.lib.spi.IOIOConnectionBootstrap;
import ioio.lib.spi.IOIOConnectionFactory;
import ioio.lib.spi.NoRuntimeSupportException;

import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.atomic.AtomicInteger;

import javax.bluetooth.DeviceClass;
import javax.bluetooth.DiscoveryAgent;
import javax.bluetooth.DiscoveryListener;
import javax.bluetooth.LocalDevice;
import javax.bluetooth.RemoteDevice;
import javax.bluetooth.ServiceRecord;
import javax.bluetooth.UUID;

import android.util.Log;

public class JSR82BluetoothIOIOConnectionBootstrap implements
IOIOConnectionBootstrap {

  public static final UUID IOIO_UUID =
      new UUID("0000110100001000800000805F9B34FB", false);
  private static final String TAG = 
      "JSR82BluetoothIOIOConnectionDiscovery";
  private final LocalDevice adapter_;

  public JSR82BluetoothIOIOConnectionBootstrap() throws NoRuntimeSupportException {
    try {
      adapter_ = LocalDevice.getLocalDevice();
      return;
    } catch (Throwable e) {
      e.printStackTrace();
    }
    throw new NoRuntimeSupportException(
        "Bluetooth is not supported on this device.");
  }

  @Override
  public void getFactories(final Collection<IOIOConnectionFactory> result) {
    try {
      final DiscoveryAgent agent = adapter_.getDiscoveryAgent();
      final AtomicInteger numRunning = new AtomicInteger(1);
      Log.i(TAG, "Enumeration started. Takes a while...");
      agent.startInquiry(
          DiscoveryAgent.GIAC, new DiscoveryListener() {
            @Override
            public synchronized void deviceDiscovered(RemoteDevice btDevice, DeviceClass cod) {
              try {
                agent.searchServices(null, new UUID[] {IOIO_UUID}, btDevice, this);
                numRunning.incrementAndGet();
              } catch (Throwable t) {
                Log.w(TAG, "Could not get services from "+ btDevice.getBluetoothAddress(), t);
              }
            }
            @Override
            public synchronized void servicesDiscovered(int transID, final ServiceRecord[] servRecords) {
              Log.i(TAG, "Found IOIO module: " + servRecords[0].getHostDevice().getBluetoothAddress());
              result.add(new IOIOConnectionFactory() {
                @Override
                public String getType() {
                  return JSR82BluetoothIOIOConnection.class.getCanonicalName();
                }

                @Override
                public Object getExtra() {
                  return servRecords[0].getHostDevice().getBluetoothAddress();
                }

                @Override
                public IOIOConnection createConnection() {
                  return new JSR82BluetoothIOIOConnection(servRecords[0]);
                }
              });
            }

            @Override
            public synchronized void serviceSearchCompleted(int transID, int respCode) {
              synchronized (numRunning) {
                numRunning.decrementAndGet();            
                numRunning.notify(); 
              }
            }

            @Override
            public void inquiryCompleted(int discType) {
              synchronized (numRunning) {
                numRunning.decrementAndGet();            
                numRunning.notify(); 
              }
            }

          });
      synchronized (numRunning) {
        while (numRunning.get() != 0) {
          numRunning.wait();
        }
      }
      Log.i(TAG, "Enumeration Complete");
    } catch (Throwable t) {
      Log.w(TAG, "Enumeration failed", t);
    }
  }


  public static void main(String[] args) {
    new JSR82BluetoothIOIOConnectionBootstrap().getFactories(new ArrayList<IOIOConnectionFactory>());
  }
}
