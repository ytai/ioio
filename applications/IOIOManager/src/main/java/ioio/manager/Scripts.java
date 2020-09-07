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
package ioio.manager;

import ioio.lib.api.IOIO;
import ioio.lib.api.IcspMaster;
import ioio.lib.api.exception.ConnectionLostException;

class Scripts {
	private static int NUM_ATTEMPTS = 20;
	
	private static void exitResetVector(IOIO ioio, IcspMaster icsp)
			throws ConnectionLostException {
		ioio.beginBatch();
		icsp.executeInstruction(0x040200); // GOTO 0x200
		icsp.executeInstruction(0x040200); // GOTO 0x200
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x040200); // GOTO 0x200
		icsp.executeInstruction(0x000000); // NOP
		ioio.endBatch();
	}

	public static int getDeviceId(IOIO ioio, IcspMaster icsp)
			throws ConnectionLostException, InterruptedException {
		ioio.beginBatch();
		exitResetVector(ioio, icsp);
		icsp.executeInstruction(0x200FF0); // MOV #0x00FF, W0
		icsp.executeInstruction(0x8802A0); // MOV W0, TBLPAG
		icsp.executeInstruction(0x200006); // MOV #0x0000, W6
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x200007); // MOV #0x0000, W7
		icsp.executeInstruction(0xBA0BB6); // TBLRDL [W6++], [W7]
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x883C20); // MOV W0, VISI
		icsp.executeInstruction(0x000000); // NOP
		ioio.endBatch();
		icsp.readVisi();
		icsp.executeInstruction(0x000000); // NOP
		return icsp.waitVisiResult();
	}

	public static void chipErase(IOIO ioio, IcspMaster icsp)
			throws ConnectionLostException, InterruptedException,
			TimeoutException {
		ioio.beginBatch();
		exitResetVector(ioio, icsp);
		icsp.executeInstruction(0x2404FA); // MOV #0x404F, W10
		icsp.executeInstruction(0x883B0A); // MOV W10, NVMCON

		icsp.executeInstruction(0x200000); // MOV #0x0000, W0
		icsp.executeInstruction(0x8802A0); // MOV W0, TBLPAG
		icsp.executeInstruction(0x200000); // MOV #0x0000, W0
		icsp.executeInstruction(0xBB0800); // TBLWTL W0,[W0]
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP

		initiateWrite(ioio, icsp);
		ioio.endBatch();
		waitWriteDone(ioio, icsp);
	}

	public static void readBlock(IOIO ioio, IcspMaster icsp, int addr, int numInst,
			int[] buf) throws ConnectionLostException, InterruptedException {
		ioio.beginBatch();
		exitResetVector(ioio, icsp);
		// Initialize the Write Pointer (W7) to point to the VISI register.
		icsp.executeInstruction(0x207847); // MOV #VISI, W7
		icsp.executeInstruction(0x000000); // NOP

		for (int i = 0; i < (numInst + 1) / 2; ++i) {
			// Initialize TBLPAG and the Read Pointer (W6) for TBLRD
			// instruction.
			icsp.executeInstruction(0x200000 | ((addr & 0xFF0000) >> 12)); // MOV
																			// #<SourceAddress23:16>,
																			// W0
			icsp.executeInstruction(0x8802A0); // MOV W0, TBLPAG
			icsp.executeInstruction(0x200006 | ((addr & 0x00FFFF) << 4)); // MOV
																			// #<SourceAddress15:0>,
																			// W6
			icsp.executeInstruction(0x000000); // NOP
			// Read and clock out the contents of the next two locations of code
			// memory, through the VISI register, using the REGOUT command.
			icsp.executeInstruction(0xBA0B96); // TBLRDL [W6], [W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			ioio.endBatch();
			icsp.readVisi();
			ioio.beginBatch();
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBADBB6); // TBLRDH.B [W6++], [W7++]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBAD3D6); // TBLRDH.B [++W6], [W7--]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			ioio.endBatch();
			icsp.readVisi();
			ioio.beginBatch();
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBA0BB6); // TBLRDL [W6++], [W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			ioio.endBatch();
			icsp.readVisi();
			ioio.beginBatch();
			icsp.executeInstruction(0x000000); // NOP

			// Reset device internal PC.
			icsp.executeInstruction(0x040200); // GOTO 0x200
			icsp.executeInstruction(0x000000); // NOP
			addr += 4;
		}
		ioio.endBatch();

		int mid = 0;
		for (int i = 0; i < numInst; ++i) {
			if ((i & 0x01) == 0) {
				int low = icsp.waitVisiResult();
				mid = icsp.waitVisiResult();
				buf[i] = low | ((mid & 0xFF) << 16);
			} else {
				buf[i] = icsp.waitVisiResult() | ((mid & 0xFF00) << 8);
			}
		}
	}

	public static void writeBlock(IOIO ioio, IcspMaster icsp, int addr, int[] buf)
			throws ConnectionLostException, InterruptedException,
			TimeoutException {
		assert (buf.length >= 64);
		ioio.beginBatch();
		exitResetVector(ioio, icsp);
		// Set the NVMCON to program 64 instruction words.
		icsp.executeInstruction(0x24001A); // MOV #0x4001, W10
		icsp.executeInstruction(0x883B0A); // MOV W10, NVMCON
		// Initialize the Write Pointer (W7) for TBLWT instruction.
		icsp.executeInstruction(0x200000 | ((addr & 0xFF0000) >> 12)); // MOV
																		// #<DestinationAddress23:16>,
																		// W0
		icsp.executeInstruction(0x8802A0); // MOV W0, TBLPAG
		icsp.executeInstruction(0x200007 | ((addr & 0x00FFFF) << 4)); // MOV
																		// #<DestinationAddress15:0>,
																		// W7
		int offset = 0;
		for (int i = 0; i < 16; ++i) {
			// Load W0:W5 with the next 4 instruction words to program.
			int i0 = buf[offset++];
			int i1 = buf[offset++];
			int i2 = buf[offset++];
			int i3 = buf[offset++];
			icsp.executeInstruction(0x200000 | ((i0 & 0x00FFFF) << 4)); // MOV
																		// #<LSW0>,
																		// W0
			icsp.executeInstruction(0x200001 | ((i0 & 0xFF0000) >> 12)
					| ((i1 & 0xFF0000) >> 4)); // MOV #<MSB1:MSB0>, W1
			icsp.executeInstruction(0x200002 | ((i1 & 0x00FFFF) << 4)); // MOV
																		// #<LSW1>,
																		// W2
			icsp.executeInstruction(0x200003 | ((i2 & 0x00FFFF) << 4)); // MOV
																		// #<LSW2>,
																		// W3
			icsp.executeInstruction(0x200004 | ((i2 & 0xFF0000) >> 12)
					| ((i3 & 0xFF0000) >> 4)); // MOV #<MSB3:MSB2>, W4
			icsp.executeInstruction(0x200005 | ((i3 & 0x00FFFF) << 4)); // MOV
																		// #<LSW3>,
																		// W5
			icsp.executeInstruction(0xEB0300); // CLR W6
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBB0BB6); // TBLWTL [W6++], [W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBBDBB6); // TBLWTH.B [W6++], [W7++]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBBEBB6); // TBLWTH.B [W6++], [++W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBB1BB6); // TBLWTL [W6++], [W7++]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBB0BB6); // TBLWTL [W6++], [W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBBDBB6); // TBLWTH.B [W6++], [W7++]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBBEBB6); // TBLWTH.B [W6++], [++W7]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0xBB1BB6); // TBLWTL [W6++], [W7++]
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x000000); // NOP
			// Set the Read Pointer (W6) and load the (next set of) write
			// latches.
		}

		initiateWrite(ioio, icsp);
		ioio.endBatch();
		waitWriteDone(ioio, icsp);
	}

	private static void initiateWrite(IOIO ioio, IcspMaster icsp)
			throws ConnectionLostException {
		ioio.beginBatch();
		icsp.executeInstruction(0xA8E761); // BSET NVMCON, #WR
		icsp.executeInstruction(0x000000); // NOP
		icsp.executeInstruction(0x000000); // NOP
		ioio.endBatch();
	}

	private static void waitWriteDone(IOIO ioio, IcspMaster icsp)
			throws ConnectionLostException, InterruptedException,
			TimeoutException {
		int attempts = NUM_ATTEMPTS;
		do {
			if (attempts-- == 0) {
				throw new TimeoutException();
			}
			ioio.beginBatch();
			icsp.executeInstruction(0x040200); // GOTO 0x200
			icsp.executeInstruction(0x000000); // NOP
			icsp.executeInstruction(0x803B02); // MOV NVMCON, W2
			icsp.executeInstruction(0x883C22); // MOV W2, VISI
			icsp.executeInstruction(0x000000); // NOP
			ioio.endBatch();
			icsp.readVisi();
			icsp.executeInstruction(0x000000); // NOP
		} while ((icsp.waitVisiResult() & (1 << 15)) != 0);
	}
}
