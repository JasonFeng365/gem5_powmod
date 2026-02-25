#ifndef __ARCH_X86_INSTS_M4_MACROOP_HH__
#define __ARCH_X86_INSTS_M4_MACROOP_HH__

#include "arch/x86/memhelpers.hh"   // initiateMemRead / initiateMemWrite helpers
#include "arch/x86/regs/int.hh"     // int_reg::MicroBegin for dep reg selection
#include "base/logging.hh"          // panic(), DPRINTF (if you add debug)
#include "cpu/thread_context.hh"    // ThreadContext accessors
#include "enums/MemoryMode.hh"      // enums::timing vs enums::atomic check
#include "sim/system.hh"            // system->getMemoryMode()
#include "arch/x86/insts/macroop.hh"
#include "arch/x86/insts/microop.hh"


#include "arch/x86/insts/microop.hh"
#include "arch/x86/insts/microop_args.hh"
#include "arch/x86/ldstflags.hh"
#include "mem/packet.hh"
#include "mem/request.hh"
#include "sim/faults.hh"
#include "cpu/exec_context.hh"
#include "arch/x86/insts/microop_args.hh"


namespace gem5 {
namespace X86ISA {

class Int32PowmodMicroop : public X86MicroopBase
{
  private:
    static constexpr int NumSrcRegs = 2;
    static constexpr int NumDestRegs = 1;

    RegId m4SrcRegIdx[NumSrcRegs];
    RegId m4DestRegIdx[NumDestRegs];

    Request::FlagsType memFlags;

  public:
    Int32PowmodMicroop(ExtMachInst machInst, const char *inst_mnem,
            uint64_t setFlags, Request::FlagsType mem_flags);

    Fault execute(ExecContext *xc, trace::InstRecord *traceData) const override;
    Fault initiateAcc(ExecContext *xc,
            trace::InstRecord *traceData) const override;
    Fault completeAcc(PacketPtr pkt, ExecContext *xc,
            trace::InstRecord *traceData) const override;
};

} // namespace X86ISA
} // namespace gem5

#endif