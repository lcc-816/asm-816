/* gen-address-modes.rb Sat Jul 25 11:50:48 2015 */
template<unsigned...>
struct make_bitmask;

template<>
struct make_bitmask<> : public std::integral_constant<uint32_t, 0>
{};

template<unsigned First, unsigned... Rest>
struct make_bitmask<First, Rest...> :
public std::integral_constant<uint32_t, (1 << First) | make_bitmask<Rest...>::value >
{};

template<Machine, Mnemonic>
struct address_modes;

template<> struct address_modes<m6502, ADC> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, AND> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, BCC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BCS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BEQ> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BIT> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m6502, BMI> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BNE> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BPL> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m6502, BVC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, BVS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m6502, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, CMP> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m6502, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m6502, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, EOR> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute
> {};

template<> struct address_modes<m6502, JSR> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<m6502, LDA> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<m6502, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, ORA> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, SBC> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, STA> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, STX> :
public make_bitmask<
    absolute,
    zp,
    zp_y
> {};

template<> struct address_modes<m6502, STY> :
public make_bitmask<
    absolute,
    zp,
    zp_x
> {};

template<> struct address_modes<m6502, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m6502, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, ADC> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, AND> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, BCC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BCS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BEQ> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BIT> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, BMI> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BNE> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BPL> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BRA> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m65816, BRL> :
public make_bitmask<
    relative_long
> {};

template<> struct address_modes<m65816, BVC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, BVS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65816, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, CMP> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, COP> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m65816, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65816, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65816, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, EOR> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, JML> :
public make_bitmask<
    absolute_long,
    absolute_indirect_long
> {};

template<> struct address_modes<m65816, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<m65816, JSL> :
public make_bitmask<
    absolute_long
> {};

template<> struct address_modes<m65816, JSR> :
public make_bitmask<
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<m65816, LDA> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<m65816, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, MVN> :
public make_bitmask<
    block
> {};

template<> struct address_modes<m65816, MVP> :
public make_bitmask<
    block
> {};

template<> struct address_modes<m65816, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, ORA> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, PEA> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<m65816, PEI> :
public make_bitmask<
    zp_indirect
> {};

template<> struct address_modes<m65816, PER> :
public make_bitmask<
    relative_long
> {};

template<> struct address_modes<m65816, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHB> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHK> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PHY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLB> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, PLY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, REP> :
public make_bitmask<
    immediate
> {};

template<> struct address_modes<m65816, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, RTL> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, SBC> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    immediate,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, SEP> :
public make_bitmask<
    immediate
> {};

template<> struct address_modes<m65816, STA> :
public make_bitmask<
    stack_relative_y,
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    absolute_long,
    absolute_long_x,
    stack_relative,
    zp,
    zp_x,
    zp_indirect_long,
    zp_indirect_long_y
> {};

template<> struct address_modes<m65816, STP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, STX> :
public make_bitmask<
    absolute,
    zp,
    zp_y
> {};

template<> struct address_modes<m65816, STY> :
public make_bitmask<
    absolute,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, STZ> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m65816, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TCD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TCS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TDC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TRB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65816, TSB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65816, TSC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TXY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, TYX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, WAI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, WDM> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m65816, XBA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65816, XCE> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, ADC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, AND> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, BCC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BCS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BEQ> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BIT> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, BMI> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BNE> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BPL> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BRA> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m65c02, BVC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, BVS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<m65c02, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, CMP> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65c02, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65c02, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, EOR> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<m65c02, JSR> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<m65c02, LDA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<m65c02, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, ORA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PHX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PHY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PLX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, PLY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, SBC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, STA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, STP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, STX> :
public make_bitmask<
    absolute,
    zp,
    zp_y
> {};

template<> struct address_modes<m65c02, STY> :
public make_bitmask<
    absolute,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, STZ> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m65c02, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, TRB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65c02, TSB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65c02, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65c02, WAI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, ADC> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, AND> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, ASR> :
public make_bitmask<
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, ASW> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<m65ce02, AUG> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, BBR0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBR7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BBS7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<m65ce02, BCC> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BCS> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BEQ> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BIT> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, BMI> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BNE> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BPL> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BRA> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<m65ce02, BSR> :
public make_bitmask<
    relative_long
> {};

template<> struct address_modes<m65ce02, BVC> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, BVS> :
public make_bitmask<
    relative,
    relative_long
> {};

template<> struct address_modes<m65ce02, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, CLE> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, CMP> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65ce02, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65ce02, CPZ> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<m65ce02, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, DEW> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, DEZ> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, EOR> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, INW> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, INZ> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<m65ce02, JSR> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<m65ce02, LDA> :
public make_bitmask<
    stack_relative_y,
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<m65ce02, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, LDZ> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate
> {};

template<> struct address_modes<m65ce02, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, NEG> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, ORA> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PHW> :
public make_bitmask<
    absolute,
    immediate
> {};

template<> struct address_modes<m65ce02, PHX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PHY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PHZ> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PLX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PLY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, PLZ> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, RMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, RMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, ROW> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<m65ce02, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, RTN> :
public make_bitmask<
    immediate
> {};

template<> struct address_modes<m65ce02, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, SBC> :
public make_bitmask<
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, SEE> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, SMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, SMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<m65ce02, STA> :
public make_bitmask<
    stack_relative_y,
    zp_indirect_y,
    zp_indirect_z,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, STX> :
public make_bitmask<
    absolute,
    absolute_y,
    zp,
    zp_y
> {};

template<> struct address_modes<m65ce02, STY> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, STZ> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<m65ce02, TAB> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TAZ> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TBA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TRB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65ce02, TSB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<m65ce02, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TSY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TYS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<m65ce02, TZA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, ADC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, AND> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, BBR0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBR7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BBS7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mr65c02, BCC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BCS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BEQ> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BIT> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, BMI> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BNE> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BPL> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BRA> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<mr65c02, BVC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, BVS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mr65c02, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, CMP> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<mr65c02, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<mr65c02, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, EOR> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<mr65c02, JSR> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<mr65c02, LDA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<mr65c02, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, ORA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PHX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PHY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PLX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, PLY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, RMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, RMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, SBC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, SMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, SMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mr65c02, STA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, STX> :
public make_bitmask<
    absolute,
    zp,
    zp_y
> {};

template<> struct address_modes<mr65c02, STY> :
public make_bitmask<
    absolute,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, STZ> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<mr65c02, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, TRB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<mr65c02, TSB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<mr65c02, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mr65c02, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, ADC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, AND> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, ASL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, BBR0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBR7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS0> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS1> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS2> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS3> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS4> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS5> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS6> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BBS7> :
public make_bitmask<
    zp_relative
> {};

template<> struct address_modes<mw65c02, BCC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BCS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BEQ> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BIT> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, BMI> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BNE> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BPL> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BRA> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BRK> :
public make_bitmask<
    interrupt
> {};

template<> struct address_modes<mw65c02, BVC> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, BVS> :
public make_bitmask<
    relative
> {};

template<> struct address_modes<mw65c02, CLC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, CLD> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, CLI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, CLV> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, CMP> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, CPX> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<mw65c02, CPY> :
public make_bitmask<
    absolute,
    immediate,
    zp
> {};

template<> struct address_modes<mw65c02, DEC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, DEX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, DEY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, EOR> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, INC> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, INX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, INY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, JMP> :
public make_bitmask<
    absolute_indirect,
    absolute_indirect_x,
    absolute
> {};

template<> struct address_modes<mw65c02, JSR> :
public make_bitmask<
    absolute
> {};

template<> struct address_modes<mw65c02, LDA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, LDX> :
public make_bitmask<
    absolute,
    absolute_y,
    immediate,
    zp,
    zp_y
> {};

template<> struct address_modes<mw65c02, LDY> :
public make_bitmask<
    absolute,
    absolute_x,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, LSR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, NOP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, ORA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, PHA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PHP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PHX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PHY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PLA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PLP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PLX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, PLY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, RMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, RMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, ROL> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, ROR> :
public make_bitmask<
    absolute,
    absolute_x,
    implied,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, RTI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, RTS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, SBC> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    immediate,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, SEC> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, SED> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, SEI> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, SMB0> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB1> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB2> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB3> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB4> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB5> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB6> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, SMB7> :
public make_bitmask<
    zp
> {};

template<> struct address_modes<mw65c02, STA> :
public make_bitmask<
    zp_indirect,
    zp_indirect_y,
    zp_indirect_x,
    absolute,
    absolute_x,
    absolute_y,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, STP> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, STX> :
public make_bitmask<
    absolute,
    zp,
    zp_y
> {};

template<> struct address_modes<mw65c02, STY> :
public make_bitmask<
    absolute,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, STZ> :
public make_bitmask<
    absolute,
    absolute_x,
    zp,
    zp_x
> {};

template<> struct address_modes<mw65c02, TAX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, TAY> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, TRB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<mw65c02, TSB> :
public make_bitmask<
    absolute,
    zp
> {};

template<> struct address_modes<mw65c02, TSX> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, TXA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, TXS> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, TYA> :
public make_bitmask<
    implied
> {};

template<> struct address_modes<mw65c02, WAI> :
public make_bitmask<
    implied
> {};

