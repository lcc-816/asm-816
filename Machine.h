//
//  Machine.h
//  65calc02
//
//  Created by Kelvin Sherlock on 9/7/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#ifndef __Machine_h__
#define __Machine_h__


enum Vectors {
    VectorCOP = 0xfff4,
    VectorAbort = 0xfff8,
    VectorNMI = 0xfffa,
    VectorRESET = 0xfffc,
    VectorIRQ = 0xfffe,
    VectorBRK = 0xfffe,
    
    VectorCOP16 = 0xffe4,
    VectorBRK16 = 0xffe6,
    VectorAbort16 = 0xffe8,
    VectorNMI16 = 0xffea,
    VectorIRQ16 = 0xffee,
};

enum Machine {
    kUndefinedMachine,
    m6502,
    m65c02,
    mr65c02,
    mw65c02,
    m65ce02,
    m65802,
    m65816,
    m65816_e
    
};

enum AddressMode {
    kUndefinedAddressMode,
    implied,
    relative,
    relative_long,
    immediate,
    interrupt,
    
    zp,
    zp_x,
    zp_y,
    zp_indirect,
    zp_indirect_x,
    zp_indirect_y,
    zp_indirect_z, // 65ce02
    zp_relative, // bbr/bbs
    
    zp_indirect_long,
    zp_indirect_long_y,
    
    absolute,
    absolute_x,
    absolute_y,
    absolute_indirect,
    absolute_indirect_x,
    absolute_indirect_long,
    absolute_long,
    absolute_long_x,
    
    block,
    stack_relative,
    stack_relative_y
};


enum Mnemonic {
    kUndefinedMnemonic,
    ADC,
    AND,
    ASL,
    BBR0,
    BBR1,
    BBR2,
    BBR3,
    BBR4,
    BBR5,
    BBR6,
    BBR7,
    BBS0,
    BBS1,
    BBS2,
    BBS3,
    BBS4,
    BBS5,
    BBS6,
    BBS7,
    BCC,
    BCS,
    BEQ,
    BIT,
    BMI,
    BNE,
    BPL,
    BRA,
    BRK,
    BRL,
    BVC,
    BVS,
    CLC,
    CLD,
    CLI,
    CLV,
    CMP,
    COP,
    CPX,
    CPY,
    DEC,
    DEX,
    DEY,
    EOR,
    INC,
    INX,
    INY,
    JML,
    JMP,
    JSL,
    JSR,
    LDA,
    LDX,
    LDY,
    LSR,
    MVN,
    MVP,
    NOP,
    ORA,
    PEA,
    PEI,
    PER,
    PHA,
    PHB,
    PHD,
    PHK,
    PHP,
    PHX,
    PHY,
    PLA,
    PLB,
    PLD,
    PLP,
    PLX,
    PLY,
    REP,
    RMB0,
    RMB1,
    RMB2,
    RMB3,
    RMB4,
    RMB5,
    RMB6,
    RMB7,
    ROL,
    ROR,
    RTI,
    RTL,
    RTS,
    SBC,
    SEC,
    SED,
    SEI,
    SEP,
    SMB0,
    SMB1,
    SMB2,
    SMB3,
    SMB4,
    SMB5,
    SMB6,
    SMB7,
    STA,
    STP,
    STX,
    STY,
    STZ,
    TAX,
    TAY,
    TCD,
    TCS,
    TDC,
    TRB,
    TSB,
    TSC,
    TSX,
    TXA,
    TXS,
    TXY,
    TYA,
    TYX,
    WAI,
    WDM,
    XBA,
    XCE,

    //65ce02
    // BRU = BRA/BRL
    BSR,
    RTN,
    NEG,
    ASR,
    INW,
    DEW,
    INZ,
    DEZ,
    ASW,
    ROW,
    CPZ,
    LDZ,
    CLE,
    SEE,
    PHW,
    PHZ,
    PLZ,
    TAZ,
    TZA,
    TAB,
    TBA,
    TSY,
    TYS,
    AUG,
};

#ifdef __cplusplus
extern "C" {
#endif

    
    extern const char *AddressModeToString(AddressMode mode);
    extern const char *MnemonicToString(Mnemonic mnemonic);

#ifdef __OBJC__
    @class NSString;
    extern NSString *AddressModeToNSString(AddressMode mode);
    extern NSString *MnemonicToNSString(Mnemonic mnemonic);
    
    extern NSString *HexTable[256];
    extern NSString *HexChars[16];
        
#endif

#ifdef __COREFOUNDATION_CFSTRING__
    extern CFStringRef AddressModeToCFString(AddressMode mode);
    extern CFStringRef MnemonicToCFString(Mnemonic mnemonic);
#endif
    
#ifdef __cplusplus
}
#endif


#endif
