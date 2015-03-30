/* gen-i-table.pl Thu Sep 22 17:22:11 2011 */

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

Instruction Instruction::m6502_instructions[] = {
  Instruction(m6502, ADC, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, AND, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, ASL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m6502, BCC, 1 << relative),
  Instruction(m6502, BCS, 1 << relative),
  Instruction(m6502, BEQ, 1 << relative),
  Instruction(m6502, BIT, 1 << absolute | 1 << zp),
  Instruction(m6502, BMI, 1 << relative),
  Instruction(m6502, BNE, 1 << relative),
  Instruction(m6502, BPL, 1 << relative),
  Instruction(m6502, BRK, 1 << interrupt),
  Instruction(m6502, BVC, 1 << relative),
  Instruction(m6502, BVS, 1 << relative),
  Instruction(m6502, CLC, 1 << implied),
  Instruction(m6502, CLD, 1 << implied),
  Instruction(m6502, CLI, 1 << implied),
  Instruction(m6502, CLV, 1 << implied),
  Instruction(m6502, CMP, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, CPX, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m6502, CPY, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m6502, DEC, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, DEX, 1 << implied),
  Instruction(m6502, DEY, 1 << implied),
  Instruction(m6502, EOR, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, INC, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, INX, 1 << implied),
  Instruction(m6502, INY, 1 << implied),
  Instruction(m6502, JMP, 1 << absolute_indirect | 1 << absolute),
  Instruction(m6502, JSR, 1 << absolute),
  Instruction(m6502, LDA, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, LDX, 1 << absolute | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_y),
  Instruction(m6502, LDY, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m6502, LSR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m6502, NOP, 1 << implied),
  Instruction(m6502, ORA, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, PHA, 1 << implied),
  Instruction(m6502, PHP, 1 << implied),
  Instruction(m6502, PLA, 1 << implied),
  Instruction(m6502, PLP, 1 << implied),
  Instruction(m6502, ROL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m6502, ROR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m6502, RTI, 1 << implied),
  Instruction(m6502, RTS, 1 << implied),
  Instruction(m6502, SBC, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << immediate | 1 << zp | 1 << zp_x
    ),
  Instruction(m6502, SEC, 1 << implied),
  Instruction(m6502, SED, 1 << implied),
  Instruction(m6502, SEI, 1 << implied),
  Instruction(m6502, STA, 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute | 1 << absolute_x
     | 1 << absolute_y | 1 << zp | 1 << zp_x),
  Instruction(m6502, STX, 1 << absolute | 1 << zp | 1 << zp_y),
  Instruction(m6502, STY, 1 << absolute | 1 << zp | 1 << zp_x),
  Instruction(m6502, TAX, 1 << implied),
  Instruction(m6502, TAY, 1 << implied),
  Instruction(m6502, TSX, 1 << implied),
  Instruction(m6502, TXA, 1 << implied),
  Instruction(m6502, TXS, 1 << implied),
  Instruction(m6502, TYA, 1 << implied),
};

Instruction Instruction::m65c02_instructions[] = {
  Instruction(m65c02, ADC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, AND, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, ASL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, BCC, 1 << relative),
  Instruction(m65c02, BCS, 1 << relative),
  Instruction(m65c02, BEQ, 1 << relative),
  Instruction(m65c02, BIT, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, BMI, 1 << relative),
  Instruction(m65c02, BNE, 1 << relative),
  Instruction(m65c02, BPL, 1 << relative),
  Instruction(m65c02, BRA, 1 << relative),
  Instruction(m65c02, BRK, 1 << interrupt),
  Instruction(m65c02, BVC, 1 << relative),
  Instruction(m65c02, BVS, 1 << relative),
  Instruction(m65c02, CLC, 1 << implied),
  Instruction(m65c02, CLD, 1 << implied),
  Instruction(m65c02, CLI, 1 << implied),
  Instruction(m65c02, CLV, 1 << implied),
  Instruction(m65c02, CMP, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, CPX, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m65c02, CPY, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m65c02, DEC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, DEX, 1 << implied),
  Instruction(m65c02, DEY, 1 << implied),
  Instruction(m65c02, EOR, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, INC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, INX, 1 << implied),
  Instruction(m65c02, INY, 1 << implied),
  Instruction(m65c02, JMP, 1 << absolute_indirect | 1 << absolute_indirect_x | 1 << absolute),
  Instruction(m65c02, JSR, 1 << absolute),
  Instruction(m65c02, LDA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, LDX, 1 << absolute | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_y),
  Instruction(m65c02, LDY, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, LSR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, NOP, 1 << implied),
  Instruction(m65c02, ORA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, PHA, 1 << implied),
  Instruction(m65c02, PHP, 1 << implied),
  Instruction(m65c02, PHX, 1 << implied),
  Instruction(m65c02, PHY, 1 << implied),
  Instruction(m65c02, PLA, 1 << implied),
  Instruction(m65c02, PLP, 1 << implied),
  Instruction(m65c02, PLX, 1 << implied),
  Instruction(m65c02, PLY, 1 << implied),
  Instruction(m65c02, ROL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, ROR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, RTI, 1 << implied),
  Instruction(m65c02, RTS, 1 << implied),
  Instruction(m65c02, SBC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65c02, SEC, 1 << implied),
  Instruction(m65c02, SED, 1 << implied),
  Instruction(m65c02, SEI, 1 << implied),
  Instruction(m65c02, STA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << zp | 1 << zp_x
    ),
  Instruction(m65c02, STP, 1 << implied),
  Instruction(m65c02, STX, 1 << absolute | 1 << zp | 1 << zp_y),
  Instruction(m65c02, STY, 1 << absolute | 1 << zp | 1 << zp_x),
  Instruction(m65c02, STZ, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(m65c02, TAX, 1 << implied),
  Instruction(m65c02, TAY, 1 << implied),
  Instruction(m65c02, TRB, 1 << absolute | 1 << zp),
  Instruction(m65c02, TSB, 1 << absolute | 1 << zp),
  Instruction(m65c02, TSX, 1 << implied),
  Instruction(m65c02, TXA, 1 << implied),
  Instruction(m65c02, TXS, 1 << implied),
  Instruction(m65c02, TYA, 1 << implied),
  Instruction(m65c02, WAI, 1 << implied),
};

Instruction Instruction::mw65c02_instructions[] = {
  Instruction(mw65c02, ADC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, AND, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, ASL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, BBR0, 1 << zp_relative),
  Instruction(mw65c02, BBR1, 1 << zp_relative),
  Instruction(mw65c02, BBR2, 1 << zp_relative),
  Instruction(mw65c02, BBR3, 1 << zp_relative),
  Instruction(mw65c02, BBR4, 1 << zp_relative),
  Instruction(mw65c02, BBR5, 1 << zp_relative),
  Instruction(mw65c02, BBR6, 1 << zp_relative),
  Instruction(mw65c02, BBR7, 1 << zp_relative),
  Instruction(mw65c02, BBS0, 1 << zp_relative),
  Instruction(mw65c02, BBS1, 1 << zp_relative),
  Instruction(mw65c02, BBS2, 1 << zp_relative),
  Instruction(mw65c02, BBS3, 1 << zp_relative),
  Instruction(mw65c02, BBS4, 1 << zp_relative),
  Instruction(mw65c02, BBS5, 1 << zp_relative),
  Instruction(mw65c02, BBS6, 1 << zp_relative),
  Instruction(mw65c02, BBS7, 1 << zp_relative),
  Instruction(mw65c02, BCC, 1 << relative),
  Instruction(mw65c02, BCS, 1 << relative),
  Instruction(mw65c02, BEQ, 1 << relative),
  Instruction(mw65c02, BIT, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, BMI, 1 << relative),
  Instruction(mw65c02, BNE, 1 << relative),
  Instruction(mw65c02, BPL, 1 << relative),
  Instruction(mw65c02, BRA, 1 << relative),
  Instruction(mw65c02, BRK, 1 << interrupt),
  Instruction(mw65c02, BVC, 1 << relative),
  Instruction(mw65c02, BVS, 1 << relative),
  Instruction(mw65c02, CLC, 1 << implied),
  Instruction(mw65c02, CLD, 1 << implied),
  Instruction(mw65c02, CLI, 1 << implied),
  Instruction(mw65c02, CLV, 1 << implied),
  Instruction(mw65c02, CMP, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, CPX, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(mw65c02, CPY, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(mw65c02, DEC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, DEX, 1 << implied),
  Instruction(mw65c02, DEY, 1 << implied),
  Instruction(mw65c02, EOR, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, INC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, INX, 1 << implied),
  Instruction(mw65c02, INY, 1 << implied),
  Instruction(mw65c02, JMP, 1 << absolute_indirect | 1 << absolute_indirect_x | 1 << absolute),
  Instruction(mw65c02, JSR, 1 << absolute),
  Instruction(mw65c02, LDA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, LDX, 1 << absolute | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_y),
  Instruction(mw65c02, LDY, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, LSR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, NOP, 1 << implied),
  Instruction(mw65c02, ORA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, PHA, 1 << implied),
  Instruction(mw65c02, PHP, 1 << implied),
  Instruction(mw65c02, PHX, 1 << implied),
  Instruction(mw65c02, PHY, 1 << implied),
  Instruction(mw65c02, PLA, 1 << implied),
  Instruction(mw65c02, PLP, 1 << implied),
  Instruction(mw65c02, PLX, 1 << implied),
  Instruction(mw65c02, PLY, 1 << implied),
  Instruction(mw65c02, RMB0, 1 << zp),
  Instruction(mw65c02, RMB1, 1 << zp),
  Instruction(mw65c02, RMB2, 1 << zp),
  Instruction(mw65c02, RMB3, 1 << zp),
  Instruction(mw65c02, RMB4, 1 << zp),
  Instruction(mw65c02, RMB5, 1 << zp),
  Instruction(mw65c02, RMB6, 1 << zp),
  Instruction(mw65c02, RMB7, 1 << zp),
  Instruction(mw65c02, ROL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, ROR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, RTI, 1 << implied),
  Instruction(mw65c02, RTS, 1 << implied),
  Instruction(mw65c02, SBC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mw65c02, SEC, 1 << implied),
  Instruction(mw65c02, SED, 1 << implied),
  Instruction(mw65c02, SEI, 1 << implied),
  Instruction(mw65c02, SMB0, 1 << zp),
  Instruction(mw65c02, SMB1, 1 << zp),
  Instruction(mw65c02, SMB2, 1 << zp),
  Instruction(mw65c02, SMB3, 1 << zp),
  Instruction(mw65c02, SMB4, 1 << zp),
  Instruction(mw65c02, SMB5, 1 << zp),
  Instruction(mw65c02, SMB6, 1 << zp),
  Instruction(mw65c02, SMB7, 1 << zp),
  Instruction(mw65c02, STA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << zp | 1 << zp_x
    ),
  Instruction(mw65c02, STP, 1 << implied),
  Instruction(mw65c02, STX, 1 << absolute | 1 << zp | 1 << zp_y),
  Instruction(mw65c02, STY, 1 << absolute | 1 << zp | 1 << zp_x),
  Instruction(mw65c02, STZ, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(mw65c02, TAX, 1 << implied),
  Instruction(mw65c02, TAY, 1 << implied),
  Instruction(mw65c02, TRB, 1 << absolute | 1 << zp),
  Instruction(mw65c02, TSB, 1 << absolute | 1 << zp),
  Instruction(mw65c02, TSX, 1 << implied),
  Instruction(mw65c02, TXA, 1 << implied),
  Instruction(mw65c02, TXS, 1 << implied),
  Instruction(mw65c02, TYA, 1 << implied),
  Instruction(mw65c02, WAI, 1 << implied),
};

Instruction Instruction::mr65c02_instructions[] = {
  Instruction(mr65c02, ADC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, AND, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, ASL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, BBR0, 1 << zp_relative),
  Instruction(mr65c02, BBR1, 1 << zp_relative),
  Instruction(mr65c02, BBR2, 1 << zp_relative),
  Instruction(mr65c02, BBR3, 1 << zp_relative),
  Instruction(mr65c02, BBR4, 1 << zp_relative),
  Instruction(mr65c02, BBR5, 1 << zp_relative),
  Instruction(mr65c02, BBR6, 1 << zp_relative),
  Instruction(mr65c02, BBR7, 1 << zp_relative),
  Instruction(mr65c02, BBS0, 1 << zp_relative),
  Instruction(mr65c02, BBS1, 1 << zp_relative),
  Instruction(mr65c02, BBS2, 1 << zp_relative),
  Instruction(mr65c02, BBS3, 1 << zp_relative),
  Instruction(mr65c02, BBS4, 1 << zp_relative),
  Instruction(mr65c02, BBS5, 1 << zp_relative),
  Instruction(mr65c02, BBS6, 1 << zp_relative),
  Instruction(mr65c02, BBS7, 1 << zp_relative),
  Instruction(mr65c02, BCC, 1 << relative),
  Instruction(mr65c02, BCS, 1 << relative),
  Instruction(mr65c02, BEQ, 1 << relative),
  Instruction(mr65c02, BIT, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, BMI, 1 << relative),
  Instruction(mr65c02, BNE, 1 << relative),
  Instruction(mr65c02, BPL, 1 << relative),
  Instruction(mr65c02, BRA, 1 << relative),
  Instruction(mr65c02, BRK, 1 << interrupt),
  Instruction(mr65c02, BVC, 1 << relative),
  Instruction(mr65c02, BVS, 1 << relative),
  Instruction(mr65c02, CLC, 1 << implied),
  Instruction(mr65c02, CLD, 1 << implied),
  Instruction(mr65c02, CLI, 1 << implied),
  Instruction(mr65c02, CLV, 1 << implied),
  Instruction(mr65c02, CMP, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, CPX, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(mr65c02, CPY, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(mr65c02, DEC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, DEX, 1 << implied),
  Instruction(mr65c02, DEY, 1 << implied),
  Instruction(mr65c02, EOR, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, INC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, INX, 1 << implied),
  Instruction(mr65c02, INY, 1 << implied),
  Instruction(mr65c02, JMP, 1 << absolute_indirect | 1 << absolute_indirect_x | 1 << absolute),
  Instruction(mr65c02, JSR, 1 << absolute),
  Instruction(mr65c02, LDA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, LDX, 1 << absolute | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_y),
  Instruction(mr65c02, LDY, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, LSR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, NOP, 1 << implied),
  Instruction(mr65c02, ORA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, PHA, 1 << implied),
  Instruction(mr65c02, PHP, 1 << implied),
  Instruction(mr65c02, PHX, 1 << implied),
  Instruction(mr65c02, PHY, 1 << implied),
  Instruction(mr65c02, PLA, 1 << implied),
  Instruction(mr65c02, PLP, 1 << implied),
  Instruction(mr65c02, PLX, 1 << implied),
  Instruction(mr65c02, PLY, 1 << implied),
  Instruction(mr65c02, RMB0, 1 << zp),
  Instruction(mr65c02, RMB1, 1 << zp),
  Instruction(mr65c02, RMB2, 1 << zp),
  Instruction(mr65c02, RMB3, 1 << zp),
  Instruction(mr65c02, RMB4, 1 << zp),
  Instruction(mr65c02, RMB5, 1 << zp),
  Instruction(mr65c02, RMB6, 1 << zp),
  Instruction(mr65c02, RMB7, 1 << zp),
  Instruction(mr65c02, ROL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, ROR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, RTI, 1 << implied),
  Instruction(mr65c02, RTS, 1 << implied),
  Instruction(mr65c02, SBC, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(mr65c02, SEC, 1 << implied),
  Instruction(mr65c02, SED, 1 << implied),
  Instruction(mr65c02, SEI, 1 << implied),
  Instruction(mr65c02, SMB0, 1 << zp),
  Instruction(mr65c02, SMB1, 1 << zp),
  Instruction(mr65c02, SMB2, 1 << zp),
  Instruction(mr65c02, SMB3, 1 << zp),
  Instruction(mr65c02, SMB4, 1 << zp),
  Instruction(mr65c02, SMB5, 1 << zp),
  Instruction(mr65c02, SMB6, 1 << zp),
  Instruction(mr65c02, SMB7, 1 << zp),
  Instruction(mr65c02, STA, 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x | 1 << absolute
     | 1 << absolute_x | 1 << absolute_y | 1 << zp | 1 << zp_x
    ),
  Instruction(mr65c02, STX, 1 << absolute | 1 << zp | 1 << zp_y),
  Instruction(mr65c02, STY, 1 << absolute | 1 << zp | 1 << zp_x),
  Instruction(mr65c02, STZ, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(mr65c02, TAX, 1 << implied),
  Instruction(mr65c02, TAY, 1 << implied),
  Instruction(mr65c02, TRB, 1 << absolute | 1 << zp),
  Instruction(mr65c02, TSB, 1 << absolute | 1 << zp),
  Instruction(mr65c02, TSX, 1 << implied),
  Instruction(mr65c02, TXA, 1 << implied),
  Instruction(mr65c02, TXS, 1 << implied),
  Instruction(mr65c02, TYA, 1 << implied),
};

Instruction Instruction::m65816_instructions[] = {
  Instruction(m65816, ADC, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, AND, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, ASL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, BCC, 1 << relative),
  Instruction(m65816, BCS, 1 << relative),
  Instruction(m65816, BEQ, 1 << relative),
  Instruction(m65816, BIT, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, BMI, 1 << relative),
  Instruction(m65816, BNE, 1 << relative),
  Instruction(m65816, BPL, 1 << relative),
  Instruction(m65816, BRA, 1 << relative),
  Instruction(m65816, BRK, 1 << interrupt),
  Instruction(m65816, BRL, 1 << relative),
  Instruction(m65816, BVC, 1 << relative),
  Instruction(m65816, BVS, 1 << relative),
  Instruction(m65816, CLC, 1 << implied),
  Instruction(m65816, CLD, 1 << implied),
  Instruction(m65816, CLI, 1 << implied),
  Instruction(m65816, CLV, 1 << implied),
  Instruction(m65816, CMP, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, COP, 1 << interrupt),
  Instruction(m65816, CPX, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m65816, CPY, 1 << absolute | 1 << immediate | 1 << zp),
  Instruction(m65816, DEC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, DEX, 1 << implied),
  Instruction(m65816, DEY, 1 << implied),
  Instruction(m65816, EOR, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, INC, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, INX, 1 << implied),
  Instruction(m65816, INY, 1 << implied),
  Instruction(m65816, JML, 1 << absolute_long | 1 << absolute_indirect_long),
  Instruction(m65816, JMP, 1 << absolute_indirect | 1 << absolute_indirect_x | 1 << absolute),
  Instruction(m65816, JSL, 1 << absolute_long),
  Instruction(m65816, JSR, 1 << absolute_indirect_x | 1 << absolute),
  Instruction(m65816, LDA, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, LDX, 1 << absolute | 1 << absolute_y | 1 << immediate | 1 << zp
     | 1 << zp_y),
  Instruction(m65816, LDY, 1 << absolute | 1 << absolute_x | 1 << immediate | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, LSR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, MVN, 1 << block),
  Instruction(m65816, MVP, 1 << block),
  Instruction(m65816, NOP, 1 << implied),
  Instruction(m65816, ORA, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, PEA, 1 << absolute),
  Instruction(m65816, PEI, 1 << zp),
  Instruction(m65816, PER, 1 << relative),
  Instruction(m65816, PHA, 1 << implied),
  Instruction(m65816, PHB, 1 << implied),
  Instruction(m65816, PHD, 1 << implied),
  Instruction(m65816, PHK, 1 << implied),
  Instruction(m65816, PHP, 1 << implied),
  Instruction(m65816, PHX, 1 << implied),
  Instruction(m65816, PHY, 1 << implied),
  Instruction(m65816, PLA, 1 << implied),
  Instruction(m65816, PLB, 1 << implied),
  Instruction(m65816, PLD, 1 << implied),
  Instruction(m65816, PLP, 1 << implied),
  Instruction(m65816, PLX, 1 << implied),
  Instruction(m65816, PLY, 1 << implied),
  Instruction(m65816, REP, 1 << immediate),
  Instruction(m65816, ROL, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, ROR, 1 << absolute | 1 << absolute_x | 1 << implied | 1 << zp
     | 1 << zp_x),
  Instruction(m65816, RTI, 1 << implied),
  Instruction(m65816, RTL, 1 << implied),
  Instruction(m65816, RTS, 1 << implied),
  Instruction(m65816, SBC, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << immediate | 1 << stack_relative | 1 << zp
     | 1 << zp_x | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, SEC, 1 << implied),
  Instruction(m65816, SED, 1 << implied),
  Instruction(m65816, SEI, 1 << implied),
  Instruction(m65816, SEP, 1 << immediate),
  Instruction(m65816, STA, 1 << stack_relative_y | 1 << zp_indirect | 1 << zp_indirect_y | 1 << zp_indirect_x
     | 1 << absolute | 1 << absolute_x | 1 << absolute_y | 1 << absolute_long
     | 1 << absolute_long_x | 1 << stack_relative | 1 << zp | 1 << zp_x
     | 1 << zp_indirect_long | 1 << zp_indirect_long_y),
  Instruction(m65816, STP, 1 << implied),
  Instruction(m65816, STX, 1 << absolute | 1 << zp | 1 << zp_y),
  Instruction(m65816, STY, 1 << absolute | 1 << zp | 1 << zp_x),
  Instruction(m65816, STZ, 1 << absolute | 1 << absolute_x | 1 << zp | 1 << zp_x
    ),
  Instruction(m65816, TAX, 1 << implied),
  Instruction(m65816, TAY, 1 << implied),
  Instruction(m65816, TCD, 1 << implied),
  Instruction(m65816, TCS, 1 << implied),
  Instruction(m65816, TDC, 1 << implied),
  Instruction(m65816, TRB, 1 << absolute | 1 << zp),
  Instruction(m65816, TSB, 1 << absolute | 1 << zp),
  Instruction(m65816, TSC, 1 << implied),
  Instruction(m65816, TSX, 1 << implied),
  Instruction(m65816, TXA, 1 << implied),
  Instruction(m65816, TXS, 1 << implied),
  Instruction(m65816, TXY, 1 << implied),
  Instruction(m65816, TYA, 1 << implied),
  Instruction(m65816, TYX, 1 << implied),
  Instruction(m65816, WAI, 1 << implied),
  Instruction(m65816, WDM, 1 << interrupt),
  Instruction(m65816, XBA, 1 << implied),
  Instruction(m65816, XCE, 1 << implied),
};
