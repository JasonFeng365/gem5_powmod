#include "arch/x86/insts/m4_microop.hh" // class declarations for M4 micro-ops

#include "arch/x86/memhelpers.hh"   // initiateMemRead / initiateMemWrite helpers
#include "arch/x86/regs/int.hh"     // int_reg::MicroBegin for dep reg selection
#include "base/logging.hh"          // panic(), DPRINTF (if you add debug)
#include "cpu/thread_context.hh"    // ThreadContext accessors
#include "enums/MemoryMode.hh"      // enums::timing vs enums::atomic check
#include "sim/system.hh"            // system->getMemoryMode()


#include "arch/x86/insts/microop.hh"
#include "arch/x86/insts/microop_args.hh"
#include "arch/x86/ldstflags.hh"
#include "mem/packet.hh"
#include "mem/request.hh"
#include "sim/faults.hh"
#include "cpu/exec_context.hh"
#include <cstdint>


// This is the competitive programmer way
#include <bits/stdc++.h>

#include "debug/PowmodAccel.hh"  // Generated from DebugFlag declaration


// struct Int32PowmodState {
// 	int32_t n;
// 	int32_t k;
// 	int32_t m;
// };

// Int32PowmodState state;

// Preconditions
// k, m >= 0
// n*n <= (1<<31) - 1
int32_t int32_compute(int32_t n, int32_t k, int32_t m) {
	int32_t res = 1;
	int32_t base = n;

	while (k) {
		if (k&1) res = (res*base) % m;

		base = (base*base) % m;
		k>>=1;
	}

	return res;
}



namespace gem5 {
namespace X86ISA {

Int32PowmodMicroop::Int32PowmodMicroop(ExtMachInst machInst,
        const char *inst_mnem, uint64_t setFlags,
        Request::FlagsType mem_flags) :
    X86MicroopBase(machInst, "int32_powmod", inst_mnem, setFlags, MemReadOp),
    memFlags(mem_flags)
{
    const uint8_t rm = machInst.modRM.rm | (machInst.rex.b << 3);
    const RegId ptr_reg(intRegClass, rm);

    // Hidden micro-register for dependency tracking between M4 ops
    constexpr int DepRegIdx = int_reg::MicroBegin + 2;
    const RegId dep_reg(intRegClass, DepRegIdx);

    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&Int32PowmodMicroop::m4SrcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&Int32PowmodMicroop::m4DestRegIdx));

		

    // Now we read TWO registers and write ONE
    _numSrcRegs = 2;
    _numDestRegs = 1;
    _numTypedDestRegs[IntRegClass] = 1;

    m4SrcRegIdx[0] = dep_reg;   // Read dependency register (wait for previous M4 op)
    m4SrcRegIdx[1] = ptr_reg;   // Read pointer register
    m4DestRegIdx[0] = dep_reg;  // Write dependency register (next M4 op waits for us)

    flags[IsLoad] = 1;            // Load in data
	flags[IsStore] = 1;           // Store result of computation
    flags[IsNonSpeculative] = 1;  // Don't execute speculatively
    flags[IsReadBarrier] = 1;     // Order with respect to earlier memory ops
    flags[IsWriteBarrier] = 1;
}


Fault
Int32PowmodMicroop::execute(ExecContext *xc, trace::InstRecord *traceData) const
{
	// Atomic path (SimpleCPU) - do everything here
	const Addr addr = xc->getRegOperand(this, 1);
	std::array<uint8_t, 64> buf;
	std::vector<bool> byte_enable(64, true);
	Fault fault = xc->readMem(addr, buf.data(), 64, memFlags, byte_enable);
	if (fault != NoFault)
		return fault;

	// TODO: Copy buf into accelerator state, trigger computation if ready
	// std::array<float, 16> floats;
	// #pragma unroll  
	// for (size_t i = 0; i < 16; ++i) {
	// 	memcpy(&floats[i], &buf[4*i], 4);
	// 	DPRINTF(PowmodAccel, "%d = %f, from %d %d %d %d\n", i, floats[i], buf[4*i], buf[4*i+1], buf[4*i+2], buf[4*i+3]);
	// }

	// DPRINTF(PowmodAccel, "ATOMIC Finished loading floats to A\n");

	// state.aQueue.push_back(std::move(floats));


	// Write dep_reg to create RAW dependency edge (value doesn't matter)
	xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));
	return NoFault;
}

Fault
Int32PowmodMicroop::initiateAcc(ExecContext *xc, trace::InstRecord *traceData) const
{
	// Timing path - just start the memory request
	const Addr addr = xc->getRegOperand(this, 1);
	return initiateMemRead(xc, traceData, addr, 64, memFlags);
}

Fault
Int32PowmodMicroop::completeAcc(PacketPtr pkt, ExecContext *xc,
		trace::InstRecord *traceData) const
{
	// Timing path - memory response arrived
	const uint8_t *data = pkt->getConstPtr<uint8_t>();

	// TODO: Copy buf into accelerator state, trigger computation if ready
	// std::array<float, 16> floats;
	// #pragma unroll
	// for (size_t i = 0; i < 16; ++i) {
	// 	memcpy(&floats[i], &data[4*i], 4);
	// }


	// state.aQueue.push_back(std::move(floats));


	
	// In your completeAcc:

	// Write dep_reg to create RAW dependency edge (value doesn't matter)
	xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));
	
	return NoFault;
}

}
}