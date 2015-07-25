/* gen-i-table.pl Sat Jul 25 09:52:00 2015 */

Mnemonic Instruction::m6502_mnemonics[] = {
  ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, 
  BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, 
  CLV, CMP, CPX, CPY, DEC, DEX, DEY, EOR, 
  INC, INX, INY, JMP, JSR, LDA, LDX, LDY, 
  LSR, NOP, ORA, PHA, PHP, PLA, PLP, ROL, 
  ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, 
  STX, STY, TAX, TAY, TSX, TXA, TXS, TYA, 
};

Mnemonic Instruction::m65c02_mnemonics[] = {
  ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, 
  BNE, BPL, BRA, BRK, BVC, BVS, CLC, CLD, 
  CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, 
  EOR, INC, INX, INY, JMP, JSR, LDA, LDX, 
  LDY, LSR, NOP, ORA, PHA, PHP, PHX, PHY, 
  PLA, PLP, PLX, PLY, ROL, ROR, RTI, RTS, 
  SBC, SEC, SED, SEI, STA, STP, STX, STY, 
  STZ, TAX, TAY, TRB, TSB, TSX, TXA, TXS, 
  TYA, WAI, 
};

Mnemonic Instruction::mw65c02_mnemonics[] = {
  ADC, AND, ASL, BBR0, BBR1, BBR2, BBR3, BBR4, 
  BBR5, BBR6, BBR7, BBS0, BBS1, BBS2, BBS3, BBS4, 
  BBS5, BBS6, BBS7, BCC, BCS, BEQ, BIT, BMI, 
  BNE, BPL, BRA, BRK, BVC, BVS, CLC, CLD, 
  CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, 
  EOR, INC, INX, INY, JMP, JSR, LDA, LDX, 
  LDY, LSR, NOP, ORA, PHA, PHP, PHX, PHY, 
  PLA, PLP, PLX, PLY, RMB0, RMB1, RMB2, RMB3, 
  RMB4, RMB5, RMB6, RMB7, ROL, ROR, RTI, RTS, 
  SBC, SEC, SED, SEI, SMB0, SMB1, SMB2, SMB3, 
  SMB4, SMB5, SMB6, SMB7, STA, STP, STX, STY, 
  STZ, TAX, TAY, TRB, TSB, TSX, TXA, TXS, 
  TYA, WAI, 
};

Mnemonic Instruction::mr65c02_mnemonics[] = {
  ADC, AND, ASL, BBR0, BBR1, BBR2, BBR3, BBR4, 
  BBR5, BBR6, BBR7, BBS0, BBS1, BBS2, BBS3, BBS4, 
  BBS5, BBS6, BBS7, BCC, BCS, BEQ, BIT, BMI, 
  BNE, BPL, BRA, BRK, BVC, BVS, CLC, CLD, 
  CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, 
  EOR, INC, INX, INY, JMP, JSR, LDA, LDX, 
  LDY, LSR, NOP, ORA, PHA, PHP, PHX, PHY, 
  PLA, PLP, PLX, PLY, RMB0, RMB1, RMB2, RMB3, 
  RMB4, RMB5, RMB6, RMB7, ROL, ROR, RTI, RTS, 
  SBC, SEC, SED, SEI, SMB0, SMB1, SMB2, SMB3, 
  SMB4, SMB5, SMB6, SMB7, STA, STX, STY, STZ, 
  TAX, TAY, TRB, TSB, TSX, TXA, TXS, TYA, 
};

Mnemonic Instruction::m65ce02_mnemonics[] = {
  ADC, AND, ASL, ASR, ASW, AUG, BBR0, BBR1, 
  BBR2, BBR3, BBR4, BBR5, BBR6, BBR7, BBS0, BBS1, 
  BBS2, BBS3, BBS4, BBS5, BBS6, BBS7, BCC, BCS, 
  BEQ, BIT, BMI, BNE, BPL, BRA, BRK, BSR, 
  BVC, BVS, CLC, CLD, CLE, CLI, CLV, CMP, 
  CPX, CPY, CPZ, DEC, DEW, DEX, DEY, DEZ, 
  EOR, INC, INW, INX, INY, INZ, JMP, JSR, 
  LDA, LDX, LDY, LDZ, LSR, NEG, NOP, ORA, 
  PHA, PHP, PHW, PHX, PHY, PHZ, PLA, PLP, 
  PLX, PLY, PLZ, RMB0, RMB1, RMB2, RMB3, RMB4, 
  RMB5, RMB6, RMB7, ROL, ROR, ROW, RTI, RTN, 
  RTS, SBC, SEC, SED, SEE, SEI, SMB0, SMB1, 
  SMB2, SMB3, SMB4, SMB5, SMB6, SMB7, STA, STX, 
  STY, STZ, TAB, TAX, TAY, TAZ, TBA, TRB, 
  TSB, TSX, TSY, TXA, TXS, TYA, TYS, TZA, 
};

Mnemonic Instruction::m65816_mnemonics[] = {
  ADC, AND, ASL, BCC, BCS, BEQ, BIT, BMI, 
  BNE, BPL, BRA, BRK, BRL, BVC, BVS, CLC, 
  CLD, CLI, CLV, CMP, COP, CPX, CPY, DEC, 
  DEX, DEY, EOR, INC, INX, INY, JML, JMP, 
  JSL, JSR, LDA, LDX, LDY, LSR, MVN, MVP, 
  NOP, ORA, PEA, PEI, PER, PHA, PHB, PHD, 
  PHK, PHP, PHX, PHY, PLA, PLB, PLD, PLP, 
  PLX, PLY, REP, ROL, ROR, RTI, RTL, RTS, 
  SBC, SEC, SED, SEI, SEP, STA, STP, STX, 
  STY, STZ, TAX, TAY, TCD, TCS, TDC, TRB, 
  TSB, TSC, TSX, TXA, TXS, TXY, TYA, TYX, 
  WAI, WDM, XBA, XCE, 
};

template<unsigned...>
struct make_bitmask;

template<>
struct make_bitmask<> : public std::integral_constant<uint32_t, 0>
{};

template<unsigned First, unsigned... Rest>
struct make_bitmask<First, Rest...> :
public std::integral_constant<uint32_t, (1 << First) | make_bitmask<Rest...>::value >
{};


Instruction Instruction::m6502_instructions[] = {
  Instruction(m6502, ADC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, AND, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m6502, BCC, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BCS, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BEQ, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BIT, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m6502, BMI, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BNE, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BPL, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(m6502, BVC, make_bitmask<
    relative
    >::value),
  Instruction(m6502, BVS, make_bitmask<
    relative
    >::value),
  Instruction(m6502, CLC, make_bitmask<
    implied
    >::value),
  Instruction(m6502, CLD, make_bitmask<
    implied
    >::value),
  Instruction(m6502, CLI, make_bitmask<
    implied
    >::value),
  Instruction(m6502, CLV, make_bitmask<
    implied
    >::value),
  Instruction(m6502, CMP, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m6502, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m6502, DEC, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m6502, DEX, make_bitmask<
    implied
    >::value),
  Instruction(m6502, DEY, make_bitmask<
    implied
    >::value),
  Instruction(m6502, EOR, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, INC, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m6502, INX, make_bitmask<
    implied
    >::value),
  Instruction(m6502, INY, make_bitmask<
    implied
    >::value),
  Instruction(m6502, JMP, make_bitmask<
    absolute,
    absolute_indirect
    >::value),
  Instruction(m6502, JSR, make_bitmask<
    absolute
    >::value),
  Instruction(m6502, LDA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(m6502, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m6502, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m6502, NOP, make_bitmask<
    implied
    >::value),
  Instruction(m6502, ORA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, PHA, make_bitmask<
    implied
    >::value),
  Instruction(m6502, PHP, make_bitmask<
    implied
    >::value),
  Instruction(m6502, PLA, make_bitmask<
    implied
    >::value),
  Instruction(m6502, PLP, make_bitmask<
    implied
    >::value),
  Instruction(m6502, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m6502, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m6502, RTI, make_bitmask<
    implied
    >::value),
  Instruction(m6502, RTS, make_bitmask<
    implied
    >::value),
  Instruction(m6502, SBC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, SEC, make_bitmask<
    implied
    >::value),
  Instruction(m6502, SED, make_bitmask<
    implied
    >::value),
  Instruction(m6502, SEI, make_bitmask<
    implied
    >::value),
  Instruction(m6502, STA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m6502, STX, make_bitmask<
    absolute,
    zp,
    zp_y
    >::value),
  Instruction(m6502, STY, make_bitmask<
    absolute,
    zp,
    zp_x
    >::value),
  Instruction(m6502, TAX, make_bitmask<
    implied
    >::value),
  Instruction(m6502, TAY, make_bitmask<
    implied
    >::value),
  Instruction(m6502, TSX, make_bitmask<
    implied
    >::value),
  Instruction(m6502, TXA, make_bitmask<
    implied
    >::value),
  Instruction(m6502, TXS, make_bitmask<
    implied
    >::value),
  Instruction(m6502, TYA, make_bitmask<
    implied
    >::value),
};

Instruction Instruction::m65c02_instructions[] = {
  Instruction(m65c02, ADC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, AND, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, BCC, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BCS, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BEQ, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BIT, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, BMI, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BNE, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BPL, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BRA, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(m65c02, BVC, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, BVS, make_bitmask<
    relative
    >::value),
  Instruction(m65c02, CLC, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, CLD, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, CLI, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, CLV, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, CMP, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65c02, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65c02, DEC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, DEX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, DEY, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, EOR, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, INC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, INX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, INY, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, JMP, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(m65c02, JSR, make_bitmask<
    absolute
    >::value),
  Instruction(m65c02, LDA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(m65c02, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, NOP, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, ORA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, PHA, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PHP, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PHX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PHY, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PLA, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PLP, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PLX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, PLY, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, RTI, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, RTS, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, SBC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, SEC, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, SED, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, SEI, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, STA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65c02, STP, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, STX, make_bitmask<
    absolute,
    zp,
    zp_y
    >::value),
  Instruction(m65c02, STY, make_bitmask<
    absolute,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, STZ, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m65c02, TAX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, TAY, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, TRB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65c02, TSB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65c02, TSX, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, TXA, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, TXS, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, TYA, make_bitmask<
    implied
    >::value),
  Instruction(m65c02, WAI, make_bitmask<
    implied
    >::value),
};

Instruction Instruction::mw65c02_instructions[] = {
  Instruction(mw65c02, ADC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, AND, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, BBR0, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR1, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR2, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR3, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR4, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR5, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR6, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBR7, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS0, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS1, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS2, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS3, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS4, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS5, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS6, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BBS7, make_bitmask<
    zp_relative
    >::value),
  Instruction(mw65c02, BCC, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BCS, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BEQ, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BIT, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, BMI, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BNE, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BPL, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BRA, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(mw65c02, BVC, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, BVS, make_bitmask<
    relative
    >::value),
  Instruction(mw65c02, CLC, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, CLD, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, CLI, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, CLV, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, CMP, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(mw65c02, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(mw65c02, DEC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, DEX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, DEY, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, EOR, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, INC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, INX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, INY, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, JMP, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(mw65c02, JSR, make_bitmask<
    absolute
    >::value),
  Instruction(mw65c02, LDA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(mw65c02, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, NOP, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, ORA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, PHA, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PHP, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PHX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PHY, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PLA, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PLP, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PLX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, PLY, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, RMB0, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB1, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB2, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB3, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB4, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB5, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB6, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, RMB7, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, RTI, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, RTS, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, SBC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, SEC, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, SED, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, SEI, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, SMB0, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB1, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB2, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB3, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB4, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB5, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB6, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, SMB7, make_bitmask<
    zp
    >::value),
  Instruction(mw65c02, STA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mw65c02, STP, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, STX, make_bitmask<
    absolute,
    zp,
    zp_y
    >::value),
  Instruction(mw65c02, STY, make_bitmask<
    absolute,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, STZ, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(mw65c02, TAX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, TAY, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, TRB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(mw65c02, TSB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(mw65c02, TSX, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, TXA, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, TXS, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, TYA, make_bitmask<
    implied
    >::value),
  Instruction(mw65c02, WAI, make_bitmask<
    implied
    >::value),
};

Instruction Instruction::mr65c02_instructions[] = {
  Instruction(mr65c02, ADC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, AND, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, BBR0, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR1, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR2, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR3, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR4, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR5, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR6, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBR7, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS0, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS1, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS2, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS3, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS4, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS5, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS6, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BBS7, make_bitmask<
    zp_relative
    >::value),
  Instruction(mr65c02, BCC, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BCS, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BEQ, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BIT, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, BMI, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BNE, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BPL, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BRA, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(mr65c02, BVC, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, BVS, make_bitmask<
    relative
    >::value),
  Instruction(mr65c02, CLC, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, CLD, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, CLI, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, CLV, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, CMP, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(mr65c02, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(mr65c02, DEC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, DEX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, DEY, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, EOR, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, INC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, INX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, INY, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, JMP, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(mr65c02, JSR, make_bitmask<
    absolute
    >::value),
  Instruction(mr65c02, LDA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(mr65c02, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, NOP, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, ORA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, PHA, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PHP, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PHX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PHY, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PLA, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PLP, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PLX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, PLY, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, RMB0, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB1, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB2, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB3, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB4, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB5, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB6, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, RMB7, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, RTI, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, RTS, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, SBC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, SEC, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, SED, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, SEI, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, SMB0, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB1, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB2, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB3, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB4, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB5, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB6, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, SMB7, make_bitmask<
    zp
    >::value),
  Instruction(mr65c02, STA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(mr65c02, STX, make_bitmask<
    absolute,
    zp,
    zp_y
    >::value),
  Instruction(mr65c02, STY, make_bitmask<
    absolute,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, STZ, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(mr65c02, TAX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, TAY, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, TRB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(mr65c02, TSB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(mr65c02, TSX, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, TXA, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, TXS, make_bitmask<
    implied
    >::value),
  Instruction(mr65c02, TYA, make_bitmask<
    implied
    >::value),
};

Instruction Instruction::m65ce02_instructions[] = {
  Instruction(m65ce02, ADC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, AND, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, ASR, make_bitmask<
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, ASW, make_bitmask<
    absolute
    >::value),
  Instruction(m65ce02, AUG, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, BBR0, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR1, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR2, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR3, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR4, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR5, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR6, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBR7, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS0, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS1, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS2, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS3, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS4, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS5, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS6, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BBS7, make_bitmask<
    zp_relative
    >::value),
  Instruction(m65ce02, BCC, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BCS, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BEQ, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BIT, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, BMI, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BNE, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BPL, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BRA, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(m65ce02, BSR, make_bitmask<
    relative_long
    >::value),
  Instruction(m65ce02, BVC, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, BVS, make_bitmask<
    relative,
    relative_long
    >::value),
  Instruction(m65ce02, CLC, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, CLD, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, CLE, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, CLI, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, CLV, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, CMP, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65ce02, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65ce02, CPZ, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65ce02, DEC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, DEW, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, DEX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, DEY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, DEZ, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, EOR, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, INC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, INW, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, INX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, INY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, INZ, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, JMP, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(m65ce02, JSR, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(m65ce02, LDA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative_y,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(m65ce02, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, LDZ, make_bitmask<
    absolute,
    absolute_x,
    immediate
    >::value),
  Instruction(m65ce02, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, NEG, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, NOP, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, ORA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, PHA, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PHP, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PHW, make_bitmask<
    absolute,
    immediate
    >::value),
  Instruction(m65ce02, PHX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PHY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PHZ, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PLA, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PLP, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PLX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PLY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, PLZ, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, RMB0, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB1, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB2, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB3, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB4, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB5, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB6, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, RMB7, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, ROW, make_bitmask<
    absolute
    >::value),
  Instruction(m65ce02, RTI, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, RTN, make_bitmask<
    immediate
    >::value),
  Instruction(m65ce02, RTS, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, SBC, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, SEC, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, SED, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, SEE, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, SEI, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, SMB0, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB1, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB2, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB3, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB4, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB5, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB6, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, SMB7, make_bitmask<
    zp
    >::value),
  Instruction(m65ce02, STA, make_bitmask<
    absolute,
    absolute_x,
    absolute_y,
    stack_relative_y,
    zp,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z,
    zp_x
    >::value),
  Instruction(m65ce02, STX, make_bitmask<
    absolute,
    absolute_y,
    zp,
    zp_y
    >::value),
  Instruction(m65ce02, STY, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, STZ, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m65ce02, TAB, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TAX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TAY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TAZ, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TBA, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TRB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65ce02, TSB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65ce02, TSX, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TSY, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TXA, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TXS, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TYA, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TYS, make_bitmask<
    implied
    >::value),
  Instruction(m65ce02, TZA, make_bitmask<
    implied
    >::value),
};

Instruction Instruction::m65816_instructions[] = {
  Instruction(m65816, ADC, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, AND, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, ASL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, BCC, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BCS, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BEQ, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BIT, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65816, BMI, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BNE, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BPL, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BRA, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BRK, make_bitmask<
    interrupt
    >::value),
  Instruction(m65816, BRL, make_bitmask<
    relative_long
    >::value),
  Instruction(m65816, BVC, make_bitmask<
    relative
    >::value),
  Instruction(m65816, BVS, make_bitmask<
    relative
    >::value),
  Instruction(m65816, CLC, make_bitmask<
    implied
    >::value),
  Instruction(m65816, CLD, make_bitmask<
    implied
    >::value),
  Instruction(m65816, CLI, make_bitmask<
    implied
    >::value),
  Instruction(m65816, CLV, make_bitmask<
    implied
    >::value),
  Instruction(m65816, CMP, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, COP, make_bitmask<
    interrupt
    >::value),
  Instruction(m65816, CPX, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65816, CPY, make_bitmask<
    absolute,
    immediate,
    zp
    >::value),
  Instruction(m65816, DEC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, DEX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, DEY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, EOR, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, INC, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, INX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, INY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, JML, make_bitmask<
    absolute_indirect_long,
    absolute_long
    >::value),
  Instruction(m65816, JMP, make_bitmask<
    absolute,
    absolute_indirect,
    absolute_indirect_x
    >::value),
  Instruction(m65816, JSL, make_bitmask<
    absolute_long
    >::value),
  Instruction(m65816, JSR, make_bitmask<
    absolute,
    absolute_indirect_x
    >::value),
  Instruction(m65816, LDA, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, LDX, make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
    >::value),
  Instruction(m65816, LDY, make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
    >::value),
  Instruction(m65816, LSR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, MVN, make_bitmask<
    block
    >::value),
  Instruction(m65816, MVP, make_bitmask<
    block
    >::value),
  Instruction(m65816, NOP, make_bitmask<
    implied
    >::value),
  Instruction(m65816, ORA, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, PEA, make_bitmask<
    absolute
    >::value),
  Instruction(m65816, PEI, make_bitmask<
    zp_indirect
    >::value),
  Instruction(m65816, PER, make_bitmask<
    relative_long
    >::value),
  Instruction(m65816, PHA, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHB, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHD, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHK, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHP, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PHY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLA, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLB, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLD, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLP, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, PLY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, REP, make_bitmask<
    immediate
    >::value),
  Instruction(m65816, ROL, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, ROR, make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
    >::value),
  Instruction(m65816, RTI, make_bitmask<
    implied
    >::value),
  Instruction(m65816, RTL, make_bitmask<
    implied
    >::value),
  Instruction(m65816, RTS, make_bitmask<
    implied
    >::value),
  Instruction(m65816, SBC, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    immediate,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, SEC, make_bitmask<
    implied
    >::value),
  Instruction(m65816, SED, make_bitmask<
    implied
    >::value),
  Instruction(m65816, SEI, make_bitmask<
    implied
    >::value),
  Instruction(m65816, SEP, make_bitmask<
    immediate
    >::value),
  Instruction(m65816, STA, make_bitmask<
    absolute,
    absolute_long,
    absolute_long_x,
    absolute_x,
    absolute_y,
    stack_relative,
    stack_relative_y,
    zp,
    zp_indirect,
    zp_indirect_long,
    zp_indirect_long_y,
    zp_indirect_x,
    zp_indirect_y,
    zp_x
    >::value),
  Instruction(m65816, STP, make_bitmask<
    implied
    >::value),
  Instruction(m65816, STX, make_bitmask<
    absolute,
    zp,
    zp_y
    >::value),
  Instruction(m65816, STY, make_bitmask<
    absolute,
    zp,
    zp_x
    >::value),
  Instruction(m65816, STZ, make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
    >::value),
  Instruction(m65816, TAX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TAY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TCD, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TCS, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TDC, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TRB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65816, TSB, make_bitmask<
    absolute,
    zp
    >::value),
  Instruction(m65816, TSC, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TSX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TXA, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TXS, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TXY, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TYA, make_bitmask<
    implied
    >::value),
  Instruction(m65816, TYX, make_bitmask<
    implied
    >::value),
  Instruction(m65816, WAI, make_bitmask<
    implied
    >::value),
  Instruction(m65816, WDM, make_bitmask<
    interrupt
    >::value),
  Instruction(m65816, XBA, make_bitmask<
    implied
    >::value),
  Instruction(m65816, XCE, make_bitmask<
    implied
    >::value),
};
