#include <ares/ares.hpp>
#include "m68000.hpp"

namespace ares {

enum : u32  { Byte, Word, Long };
enum : bool { Reverse = 1 };

#include "registers.cpp"
#include "memory.cpp"
#include "effective-address.cpp"
#include "traits.cpp"
#include "conditions.cpp"
#include "algorithms.cpp"
#include "instructions.cpp"
#include "disassembler.cpp"
#include "instruction.cpp"
#include "serialization.cpp"

auto M68000::power() -> void {
  for(auto& dr : r.d) dr = 0;
  for(auto& ar : r.a) ar = 0;
  r.sp = 0;
  r.pc = 0;

  r.c = 0;
  r.v = 0;
  r.z = 0;
  r.n = 0;
  r.x = 0;
  r.i = 7;
  r.s = 1;
  r.t = 0;

  r.irc = 0x4e71;  //nop
  r.ir  = 0x4e71;  //nop
  r.ird = 0x4e71;  //nop

  r.stop  = false;
  r.reset = false;
}

auto M68000::supervisor() -> bool {
  if(r.s) return true;

  r.pc -= 4;
  exception(Exception::Unprivileged, Vector::Unprivileged);
  return false;
}

auto M68000::exception(u32 exception, u32 vector, u32 priority) -> void {
  r.stop  = false;
  idle(10);  //todo: not accurate

  auto pc = r.pc;
  auto sr = readSR();

  if(!r.s) swap(r.a[7], r.sp);
  if(priority) r.i = priority;
  r.s = 1;
  r.t = 0;

  push<Long>(pc - 4);
  push<Word>(sr);

  r.pc = read<Long>(vector << 2);
  prefetch();
  prefetch();
}

auto M68000::interrupt(u32 vector, u32 priority) -> void {
  return exception(Exception::Interrupt, vector, priority);
}

}
