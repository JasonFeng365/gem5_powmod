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

inline double double_pow(double n, uint64_t k) {
	double res = 1;
	double base = n;

	while (k) {
		if (k&1) res*=base;

		base *= base;
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

UInt64PowmodMicroop::UInt64PowmodMicroop(ExtMachInst machInst,
        const char *inst_mnem, uint64_t setFlags,
        Request::FlagsType mem_flags) :
    X86MicroopBase(machInst, "uint64_powmod", inst_mnem, setFlags, MemReadOp),
    memFlags(mem_flags)
{
    // const uint8_t rm = machInst.modRM.rm | (machInst.rex.b << 3);
    // const RegId ptr_reg(intRegClass, rm);

	const RegId n_reg(intRegClass, 0 | (machInst.rex.b << 3));
	const RegId k_reg(intRegClass, 1 | (machInst.rex.b << 3));
	const RegId m_reg(intRegClass, 2 | (machInst.rex.b << 3));
	const RegId res_reg(intRegClass, 3 | (machInst.rex.b << 3));

	// DPRINTF(PowmodAccel, "UInt64PowmodMicroop %d, %d\n", machInst.modRM.rm, rm);

    // Hidden micro-register for dependency tracking between M4 ops
    // constexpr int DepRegIdx = int_reg::MicroBegin + 2;
    // const RegId dep_reg(intRegClass, DepRegIdx);

    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&UInt64PowmodMicroop::m4SrcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&UInt64PowmodMicroop::m4DestRegIdx));

    _numSrcRegs = 3;
    _numDestRegs = 1;
    _numTypedDestRegs[IntRegClass] = 1;

	m4SrcRegIdx[0] = n_reg;
	m4SrcRegIdx[1] = k_reg;
	m4SrcRegIdx[2] = m_reg;
	m4DestRegIdx[0] = res_reg;
}



// Note: getRegOperand returns uint64_t
Fault
UInt64PowmodMicroop::execute(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "ATOMIC INT64 Entered execute\n");
	DPRINTF(PowmodAccel, "ATOMIC INT64 Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "ATOMIC INT64 Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));
	DPRINTF(PowmodAccel, "ATOMIC INT64 Value in register 2: %" PRIu64 "\n", xc->getRegOperand(this, 2));

	uint64_t n = xc->getRegOperand(this, 0);
	uint64_t k = xc->getRegOperand(this, 1);
	uint64_t m = xc->getRegOperand(this, 2);

	uint64_t res = uint64_powmod(n, k, m);

	DPRINTF(PowmodAccel, "ATOMIC INT64 Res: %" PRIu64 "\n", res);


	xc->setRegOperand(this, 0, res);

	return NoFault;
}

Fault
UInt64PowmodMicroop::initiateAcc(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING INT64 Entered execute\n");
	DPRINTF(PowmodAccel, "TIMING INT64 Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "TIMING INT64 Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));
	DPRINTF(PowmodAccel, "TIMING INT64 Value in register 2: %" PRIu64 "\n", xc->getRegOperand(this, 2));

	uint64_t n = xc->getRegOperand(this, 0);
	uint64_t k = xc->getRegOperand(this, 1);
	uint64_t m = xc->getRegOperand(this, 2);

	uint64_t res = uint64_powmod(n, k, m);

	DPRINTF(PowmodAccel, "TIMING INT64 Res: %" PRIu64 "\n", res);


	xc->setRegOperand(this, 0, res);

	return NoFault;
}

Fault
UInt64PowmodMicroop::completeAcc(PacketPtr pkt, ExecContext *xc,
		trace::InstRecord *traceData) const
{
	return NoFault;
}





DoublePowMicroop::DoublePowMicroop(ExtMachInst machInst,
        const char *inst_mnem, uint64_t setFlags,
        Request::FlagsType mem_flags) :
    X86MicroopBase(machInst, "double_pow", inst_mnem, setFlags, MemReadOp),
    memFlags(mem_flags)
{

	const RegId n_reg(intRegClass, 0 | (machInst.rex.b << 3));
	const RegId k_reg(intRegClass, 1 | (machInst.rex.b << 3));
	const RegId res_reg(intRegClass, 3 | (machInst.rex.b << 3));\

    setRegIdxArrays(
        reinterpret_cast<RegIdArrayPtr>(&DoublePowMicroop::m4SrcRegIdx),
        reinterpret_cast<RegIdArrayPtr>(&DoublePowMicroop::m4DestRegIdx));

    _numSrcRegs = 2;
    _numDestRegs = 1;
    _numTypedDestRegs[IntRegClass] = 1;

	m4SrcRegIdx[0] = n_reg;
	m4SrcRegIdx[1] = k_reg;
	m4DestRegIdx[0] = res_reg;
}


Fault
DoublePowMicroop::execute(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "ATOMIC DOUBLE Entered execute\n");
	DPRINTF(PowmodAccel, "ATOMIC DOUBLE Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "ATOMIC DOUBLE Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));

	uint64_t n_long = xc->getRegOperand(this, 0);
	double n;
	memcpy(&n, &n_long, sizeof(double));
	uint64_t k = xc->getRegOperand(this, 1);

	double res_double = double_pow(n, k);
	uint64_t res_long;
	memcpy(&res_long, &res_double, sizeof(double));

	DPRINTF(PowmodAccel, "ATOMIC DOUBLE Res: %lf\n", res_double);


	xc->setRegOperand(this, 0, res_long);

	return NoFault;
}

Fault
DoublePowMicroop::initiateAcc(ExecContext *xc, trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING DOUBLE Entered execute\n");
	DPRINTF(PowmodAccel, "TIMING DOUBLE Value in register 0: %" PRIu64 "\n", xc->getRegOperand(this, 0));
	DPRINTF(PowmodAccel, "TIMING DOUBLE Value in register 1: %" PRIu64 "\n", xc->getRegOperand(this, 1));

	uint64_t n_long = xc->getRegOperand(this, 0);
	double n;
	memcpy(&n, &n_long, sizeof(double));
	uint64_t k = xc->getRegOperand(this, 1);

	double res_double = double_pow(n, k);
	uint64_t res_long;
	memcpy(&res_long, &res_double, sizeof(double));

	DPRINTF(PowmodAccel, "TIMING DOUBLE Res: %lf\n", res_double);


	xc->setRegOperand(this, 0, res_long);

	return NoFault;
}

Fault
DoublePowMicroop::completeAcc(PacketPtr pkt, ExecContext *xc,
		trace::InstRecord *traceData) const
{
	DPRINTF(PowmodAccel, "TIMING DOUBLE completeAcc\n");

	return NoFault;
}

}
}
