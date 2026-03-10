#include "arch/x86/insts/powmod_microop.hh" // class declarations for M4 micro-ops

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
#include <inttypes.h>


// This is the competitive programmer way
#include <bits/stdc++.h>

#include "debug/PowmodAccel.hh"  // Generated from DebugFlag declaration

#define range(i, start, end, iter) for (int i=start; i<end; i+=iter)



std::deque<int32_t> int32_results;

// Preconditions
// k, m >= 0
// n*n <= (1<<31) - 1
inline int32_t int32_compute(int32_t n, int32_t k, int32_t m) {
	int32_t res = 1;
	int32_t base = n;

	while (k) {
		if (k&1) res = (res*base) % m;

		base = (base*base) % m;
		k>>=1;
	}

	return res;
}

inline uint64_t uint64_powmod(uint64_t n, uint64_t k, uint64_t m) {
	uint64_t res = 1;
	uint64_t base = n;

	while (k) {
		if (k&1) res = (res*base) % m;

		base = (base*base) % m;
		k>>=1;
	}

	return res;
}

inline uint64_t uint64_fibmod(uint64_t n, uint64_t k, uint64_t m) {
	uint64_t res = 1;
	uint64_t base = n;

	while (k) {
		if (k&1) res = (res*base) % m;

		base = (base*base) % m;
		k>>=1;
	}

	return res;
}



namespace gem5 {
namespace X86ISA {

StartInt32PowmodMicroop::StartInt32PowmodMicroop(ExtMachInst machInst,
        const char *inst_mnem, uint64_t setFlags,
        Request::FlagsType mem_flags) :
    X86MicroopBase(machInst, "start_int32_powmod", inst_mnem, setFlags, MemReadOp),
    memFlags(mem_flags)
{
    // const uint8_t rm = machInst.modRM.rm | (machInst.rex.b << 3);
    // const RegId ptr_reg(intRegClass, rm);

	const RegId n_reg(intRegClass, 0 | (machInst.rex.b << 3));
	const RegId k_reg(intRegClass, 1 | (machInst.rex.b << 3));
	const RegId m_reg(intRegClass, 2 | (machInst.rex.b << 3));
	const RegId res_reg(intRegClass, 3 | (machInst.rex.b << 3));

	// DPRINTF(PowmodAccel, "StartInt32PowmodMicroop %d, %d\n", machInst.modRM.rm, rm);

    // Hidden micro-register for dependency tracking between M4 ops
    // constexpr int DepRegIdx = int_reg::MicroBegin + 2;
    // const RegId dep_reg(intRegClass, DepRegIdx);

    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&StartInt32PowmodMicroop::m4SrcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&StartInt32PowmodMicroop::m4DestRegIdx));

    // Now we read TWO registers and write ONE
    _numSrcRegs = 3;
    _numDestRegs = 1;
    _numTypedDestRegs[IntRegClass] = 1;

	m4SrcRegIdx[0] = n_reg;
	m4SrcRegIdx[1] = k_reg;
	m4SrcRegIdx[2] = m_reg;
	m4DestRegIdx[0] = res_reg;

    // m4SrcRegIdx[0] = dep_reg;   // Read dependency register (wait for previous M4 op)
    // m4SrcRegIdx[1] = ptr_reg;   // Read pointer register
    // m4DestRegIdx[0] = dep_reg;  // Write dependency register (next M4 op waits for us)

    // flags[IsLoad] = 1;            // Load in data
	// flags[IsStore] = 1;           // Store result of computation
    // flags[IsNonSpeculative] = 1;  // Don't execute speculatively
    // flags[IsReadBarrier] = 1;     // Order with respect to earlier memory ops
    // flags[IsWriteBarrier] = 1;
}



// Note: getRegOperand returns uint64_t
Fault
StartInt32PowmodMicroop::execute(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "ATOMIC INT32 Entered execute\n");
	DPRINTF(PowmodAccel, "ATOMIC INT32 Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "ATOMIC INT32 Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));
	DPRINTF(PowmodAccel, "ATOMIC INT32 Value in register 2: %" PRIu64 "\n", xc->getRegOperand(this, 2));

	uint64_t n = xc->getRegOperand(this, 0);
	uint64_t k = xc->getRegOperand(this, 1);
	uint64_t m = xc->getRegOperand(this, 2);

	uint64_t res = uint64_powmod(n, k, m);

	DPRINTF(PowmodAccel, "ATOMIC INT32 Res: %" PRIu64 "\n", res);


	xc->setRegOperand(this, 0, res);

	return NoFault;

	// // Atomic path (SimpleCPU) - do everything here
	// const Addr addr = xc->getRegOperand(this, 1);
	// std::array<uint8_t, 12> buf;
	// std::vector<bool> byte_enable(12, true);
	// Fault fault = xc->readMem(addr, buf.data(), 12, memFlags, byte_enable);
	// if (fault != NoFault)
	// 	return fault;

	// // Copy data from buf into a struct int32_input
	// struct int32_input state;
	// memcpy(&state, &buf, sizeof(int32_t)*3);


	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[0]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[1]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[2]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[3]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[4]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[5]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[6]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[7]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[8]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[9]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[10]);
	// DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[11]);

	// DPRINTF(PowmodAccel, "ATOMIC INT32 Finished loading state: %d, %d, %d\n", state.n, state.k, state.m);

	// int32_t res = int32_compute(state.n, state.k, state.m);
	// int32_results.push_back(res);


	// DPRINTF(PowmodAccel, "ATOMIC INT32 %d^%d %% %d = %d\n", state.n, state.k, state.m, res);

	// // uint8_t *outbuf = (uint8_t *)malloc(12);
	// // memcpy(&outbuf[0], &state.n, 4);
	// // memcpy(&outbuf[4], &state.k, 4);
	// // memcpy(&outbuf[8], &state.m, 4);
	// // memcpy(&outbuf[12], &res, 4);

	// // fault = xc->writeMem(outbuf, 16, addr, memFlags, nullptr, byte_enable);
	// // if (fault != NoFault)
	// // 	return fault;

	// // Write dep_reg to create RAW dependency edge (value doesn't matter)
	// xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));
	// return NoFault;
}

Fault
StartInt32PowmodMicroop::initiateAcc(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING INT32 Entered execute\n");
	DPRINTF(PowmodAccel, "ATIMINGTOMIC INT32 Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "TIMING INT32 Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));
	DPRINTF(PowmodAccel, "TIMING INT32 Value in register 2: %" PRIu64 "\n", xc->getRegOperand(this, 2));

	uint64_t n = xc->getRegOperand(this, 0);
	uint64_t k = xc->getRegOperand(this, 1);
	uint64_t m = xc->getRegOperand(this, 2);

	uint64_t res = uint64_powmod(n, k, m);

	DPRINTF(PowmodAccel, "TIMING INT32 Res: %" PRIu64 "\n", res);


	xc->setRegOperand(this, 0, res);

	return NoFault;
	// // Timing path - just start the memory request
	// const Addr addr = xc->getRegOperand(this, 1);
	// return initiateMemRead(xc, traceData, addr, 12, memFlags);
}

Fault
StartInt32PowmodMicroop::completeAcc(PacketPtr pkt, ExecContext *xc,
		trace::InstRecord *traceData) const
{
	// DPRINTF(PowmodAccel, "TIMING INT32 Entered execute\n");
	// const uint8_t *data = pkt->getConstPtr<uint8_t>();

	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[0]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[1]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[2]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[3]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[4]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[5]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[6]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[7]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[8]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[9]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[10]);
	// DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", data[11]);

	// // Copy data from buf into a struct int32_input
	// struct int32_input state;
	// memcpy(&state, data, sizeof(int32_t)*3);

	// DPRINTF(PowmodAccel, "TIMING INT32 Finished loading state: %d, %d, %d\n", state.n, state.k, state.m);

	// int32_t res = int32_compute(state.n, state.k, state.m);
	// int32_results.push_back(res);

	// DPRINTF(PowmodAccel, "TIMING INT32 %d^%d %% %d = %d\n", state.n, state.k, state.m, res);

	// // uint8_t *outbuf = (uint8_t *)malloc(16);
	// // memcpy(&outbuf[0], &state.n, 4);
	// // memcpy(&outbuf[4], &state.k, 4);
	// // memcpy(&outbuf[8], &state.m, 4);
	// // memcpy(&outbuf[12], &res, 4);


	// // How do we write this back, without triggering another call to completeAcc?
	// // Maybe just an internal "mode" flag that switches back and forth


	// // DPRINTF(PowmodAccel, "TIMING start writeMem\n");
	// // std::vector<bool> byte_enable(16, true);
	// // Fault fault = xc->writeMem(outbuf, 16, addr, memFlags, nullptr, byte_enable);
	// // if (fault != NoFault)
	// // 	return fault;

	// // DPRINTF(PowmodAccel, "TIMING after writeMem\n");

	// // Write dep_reg to create RAW dependency edge (value doesn't matter)
	// xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));

	return NoFault;
}





SaveInt32PowmodMicroop::SaveInt32PowmodMicroop(ExtMachInst machInst,
        const char *inst_mnem, uint64_t setFlags,
        Request::FlagsType mem_flags) :
    X86MicroopBase(machInst, "save_int32_powmod", inst_mnem, setFlags, MemReadOp),
    memFlags(mem_flags)
{
    const uint8_t rm = machInst.modRM.rm | (machInst.rex.b << 3);
    const RegId ptr_reg(intRegClass, rm);

    // Hidden micro-register for dependency tracking between M4 ops
    constexpr int DepRegIdx = int_reg::MicroBegin + 2;
    const RegId dep_reg(intRegClass, DepRegIdx);

    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&SaveInt32PowmodMicroop::m4SrcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&SaveInt32PowmodMicroop::m4DestRegIdx));

    // Now we read TWO registers and write ONE
    _numSrcRegs = 2;
    _numDestRegs = 1;
    _numTypedDestRegs[IntRegClass] = 1;

    m4SrcRegIdx[0] = dep_reg;   // Read dependency register (wait for previous M4 op)
    m4SrcRegIdx[1] = ptr_reg;   // Read pointer register
    m4DestRegIdx[0] = dep_reg;  // Write dependency register (next M4 op waits for us)

    // flags[IsLoad] = 1;            // Load in data
	flags[IsStore] = 1;           // Store result of computation
    // flags[IsNonSpeculative] = 1;  // Don't execute speculatively
    // flags[IsReadBarrier] = 1;     // Order with respect to earlier memory ops
    // flags[IsWriteBarrier] = 1;
}


Fault
SaveInt32PowmodMicroop::execute(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "ATOMIC INT32 Entered save\n");
		// Atomic path (SimpleCPU) - do everything here
	const Addr addr = xc->getRegOperand(this, 1);
	std::vector<bool> byte_enable(4, true);

	int32_t res = int32_results.front();
	int32_results.pop_front();

	DPRINTF(PowmodAccel, "ATOMIC INT32 res = %d\n", res);

	uint8_t *buf = (uint8_t *)malloc(4);
	memcpy(buf, &res, 4);

	DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[0]);
	DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[1]);
	DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[2]);
	DPRINTF(PowmodAccel, "ATOMIC DATA LOADED: %d\n", buf[3]);


	Fault fault = xc->writeMem(buf, 4, addr, memFlags, nullptr, byte_enable);
	if (fault != NoFault)
		return fault;

	// Write dep_reg to create RAW dependency edge (value doesn't matter)
	xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));
	return NoFault;
}

Fault
SaveInt32PowmodMicroop::initiateAcc(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING INT32 Entered save\n");
	const Addr addr = xc->getRegOperand(this, 1);
	std::vector<bool> byte_enable(4, true);

	int32_t res = int32_results.front();
	int32_results.pop_front();

	DPRINTF(PowmodAccel, "TIMING INT32 res = %d\n", res);

	uint8_t *buf = (uint8_t *)malloc(4);
	memcpy(buf, &res, 4);

	DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", buf[0]);
	DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", buf[1]);
	DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", buf[2]);
	DPRINTF(PowmodAccel, "TIMING DATA LOADED: %d\n", buf[3]);


	DPRINTF(PowmodAccel, "TIMING INT32 Before writeMem\n");
	Fault fault = xc->writeMem(buf, 4, addr, memFlags, nullptr, byte_enable);
	xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "TIMING INT32 After writeMem\n");
	return fault;
}

Fault
SaveInt32PowmodMicroop::completeAcc(PacketPtr pkt, ExecContext *xc,
		trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING INT32 completeAcc\n");

	// Write dep_reg to create RAW dependency edge (value doesn't matter)
	xc->setRegOperand(this, 0, xc->getRegOperand(this, 0));

	return NoFault;
}

}
}
