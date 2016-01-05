//
//  OpCode.cpp
//  65calc02
//
//  Created by Kelvin Sherlock on 9/13/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#include "OpCode.h"

#include "gen-o-table.h"

#include <unordered_map>
#include <tuple>

struct MMA {
    Machine machine;
    Mnemonic mnemonic;
    AddressMode addressMode;
    
    MMA(Machine a, Mnemonic b, AddressMode c) : 
        machine(a), mnemonic(b), addressMode(c)
    {
    }
    MMA(OpCode op) :
    machine(op.machine()), mnemonic(op.mnemonic()), addressMode(op.addressMode())
    {
    }
    
    size_t hash() const
    {
        return machine | mnemonic << 8 | addressMode << 16;
    }
    bool operator==(const MMA& rhs) const
    {
        return machine == rhs.machine
        && mnemonic == rhs.mnemonic
        && addressMode == rhs.addressMode;
    }
};

//typedef std::tr1::tuple<Machine, Mnemonic, AddressMode> MMA;
typedef std::pair<Machine, uint8_t> MO;

namespace std { 
        
        template<> struct hash<MMA> : public unary_function<MMA, size_t>
        {
            size_t operator()(MMA mma) const
            {
                return mma.hash();
                //return get<0>(mma) | get<1>(mma) << 8 | get<2>(mma) << 16;
            }
        };

        
        template<> struct hash<MO> : public unary_function<MO, size_t>
        {
            size_t operator()(MO mo) const
            {
                return mo.first << 8 | mo.second;
            }
        };        
            
};

typedef std::unordered_map<MMA, OpCode> HashTable;

static HashTable hash;

void OpCode::InitHashTable()
{
    // map tuple of machine + mnemonic + address mode -> opcode
    

    if (hash.empty())
    {
        // 6502 includes empty entries but not extra nops.
        for (unsigned i = 0; i < 256; ++i)
        {
            const OpCode &op = m6502_opcodes[i];
            if (op)
            {                
                hash[MMA(op)] = op;
            }
        }
        
        // 65c02 -- includes extra nops.
        for (unsigned i = 0; i < 256; ++i)
        {
            const OpCode &op = m65c02_opcodes[i];
            
            if (op.mnemonic() == NOP && i != 0xea) continue;
            hash[MMA(op)] = op;
        }
        
        // w65c02 -- includes extra nops.
        for (unsigned i = 0; i < 256; ++i)
        {
            const OpCode &op = mw65c02_opcodes[i];
            if (op.mnemonic() == NOP && i != 0xea) continue;
            hash[MMA(op)] = op;
        }
        
        // rockwell -- includes extra nops.
        for (unsigned i = 0; i < 256; ++i)
        {
            const OpCode &op = mr65c02_opcodes[i];
            if (op.mnemonic() == NOP && i != 0xea) continue;
            hash[MMA(op)] = op;
        }
        
        // 65816 -- all instructions valid.
        for (unsigned i = 0; i < 256; ++i)
        {
            const OpCode &op = m65816_opcodes[i];
            hash[MMA(op)] = op;
        }
    }

}
OpCode::OpCode(Instruction instr, AddressMode addressMode) : OpCode()
{

    InitHashTable();
    
    HashTable::iterator iter;
    iter = hash.find(MMA(instr.machine(), instr.mnemonic(), addressMode));
    
    if (iter == hash.end()) return;
    
    *this = iter->second;
}


OpCode::OpCode(Machine machine, Mnemonic m, AddressMode addressMode) : 
    OpCode(Instruction(machine, m), addressMode)
{
}


OpCode::OpCode(Machine machine, uint8_t opcode) : OpCode()
{
    switch (machine)
    {
        case kUndefinedMachine:
            break;
            
        case m6502:
            *this = m6502_opcodes[opcode];
            break;
        case m65c02:
            *this = m65c02_opcodes[opcode];
            break;
        case mw65c02:
            *this = mw65c02_opcodes[opcode];
            break;
        case mr65c02:
            *this = mr65c02_opcodes[opcode];
            break;
        case m65802:
        case m65816:
        case m65816_e:
            *this = m65816_opcodes[opcode];
            break;
        case m65ce02:
            *this = m65ce02_opcodes[opcode];
            break;
    }
}

const_vector<OpCode> OpCode::OpCodes(Machine machine)
{
#undef xxx
#define xxx(table) { return const_vector<OpCode>(table, 256); }
    

    switch (machine)
    {
        default:
        case kUndefinedMachine:
            return const_vector<OpCode>();
            break;
            
        case m6502:
            xxx(m6502_opcodes);
            break;
        case m65c02:
            xxx(m65c02_opcodes);
            break;
        case mw65c02:
            xxx(mw65c02_opcodes);
            break;
        case mr65c02:
            xxx(mr65c02_opcodes);
            break;
        case m65802:
        case m65816:
            xxx(m65816_opcodes);
            break;
            
    }
    

}



int OpCode::opcode(Machine machine, Mnemonic mnemonic, AddressMode addressMode)
{
    InitHashTable();

    HashTable::iterator iter;
    
    iter = hash.find(MMA(machine, mnemonic, addressMode));
    
    if (iter == hash.end()) return -1;
    
    return iter->second.opcode();
}

#ifdef __OBJC__

NSString *OpCode::formatOperand(uint32_t operand, uint16_t address, bool longM, bool longX) const
{
    unsigned b;
    
    b = bytes(longM, longX) - 1;
    if (_addressMode == immediate && _machine == m65816)
    {
        if (b == 4)
            return [NSString stringWithFormat: @"#%04X", operand];
    }
    
    switch (_addressMode)
    {
        case kUndefinedAddressMode:
            return nil;
            
        case implied:
            return @"";
            
        case immediate:
            return [NSString stringWithFormat: @"#%02X", operand];
            
        case interrupt:
        case zp:
            return [NSString stringWithFormat: @"%02X", operand];
            
        case zp_x:
            return [NSString stringWithFormat: @"%02X,X", operand];
            
            
        case zp_y:
            return [NSString stringWithFormat: @"%02X,Y", operand];            
            
        case zp_indirect:
            return [NSString stringWithFormat: @"(%02X)", operand];
            
        case zp_indirect_x:
            return [NSString stringWithFormat: @"(%02X,X)", operand];
            
        case zp_indirect_y:
            return [NSString stringWithFormat: @"(%02X),Y", operand];
            
        case zp_indirect_long:
            return [NSString stringWithFormat: @"[%02X]", operand];
            
        case zp_indirect_long_y:
            return [NSString stringWithFormat: @"[%02X],Y", operand];
            
        case absolute:
            return [NSString stringWithFormat: @"%04X", operand];
            
        case absolute_x:
            return [NSString stringWithFormat: @"%04X,X", operand];
            
        case absolute_y:
            return [NSString stringWithFormat: @"%04X,Y", operand];
            
        case absolute_indirect:
            return [NSString stringWithFormat: @"(%04X)", operand];
            
        case absolute_indirect_x:
            return [NSString stringWithFormat: @"(%04X,X)", operand];
            
        case absolute_long:
            return [NSString stringWithFormat: @"%06X", operand];
            
        case absolute_long_x:
            return [NSString stringWithFormat: @"%06X,X", operand];
            
        case absolute_indirect_long:
            return [NSString stringWithFormat: @"[%04X]", operand];
            
        case block:
            return [NSString stringWithFormat: @"%02X,%02X", operand >> 8, operand & 0xff];
            
        case stack_relative:
            return [NSString stringWithFormat: @"%02X,S", operand];
            
        case stack_relative_y:
            return [NSString stringWithFormat: @"(%02X,S),Y", operand];
            
        case relative_long:
            if (true)
            {
                uint16_t tmp;
                //bool n;
                
                operand &= 0xffff;
                //n = operand & 0x8000;
                tmp = address + 3 + operand;
                
                tmp &= 0xffff;
#if 0
                // todo -- if operand == 0, "*"
                if (n) 
                    return [NSString stringWithFormat: @"%04X (*-%04X)",
                            tmp, 0x1000 - operand];
                
                else
                    return [NSString stringWithFormat: @"%04X (*+%04X)",
                            tmp, operand];                    
#else
                return [NSString stringWithFormat: @"%04X", tmp];
#endif
                
            }
        case relative:
            if (true)
            {
                uint16_t tmp;
                bool n;
                
                operand &= 0xff;
                n = operand & 0x80;
                tmp = address + 2 + operand;

                if (n) tmp += 0xff00;
                tmp &= 0xffff;
#if 0
                if (n)
                {
                    tmp += 0xff00;
                    
                    return [NSString stringWithFormat: @"%04X (*-%02X)",
                            tmp, 0x100 - operand];
                }
                else
                    return [NSString stringWithFormat: @"%04X (*+%02X)",
                            tmp, operand];
      
            
#else
                return [NSString stringWithFormat: @"%04X", tmp];
#endif
            }
            
        case zp_relative:
            if (true)
            {
                uint16_t tmp;
                bool n;
                unsigned zp;
                
                zp = operand & 0xff;
                operand >>= 8;
                n = operand & 0x80;
                
                tmp = address + 3 + (operand & 0xff);
                if (n) tmp += 0xff00;
                
#if 0
                if (n) 
                {
                    tmp += 0xff00;
 
                    return [NSString stringWithFormat: @"%02X,%04X (*-%02X)",
                            zp, tmp, 0x100 - operand];
                } 
                else
                    return [NSString stringWithFormat: @"%02X,%04X (*+%02X)",
                            zp, tmp, operand];
#else
                return [NSString stringWithFormat: @"%02X,%04X", zp, tmp];
#endif
            }
    }
    
    
    return nil;
    
}


NSString *OpCode::formatHex(uint32_t operand, bool longM, bool longX) const
{
    static const char *hex = "0123456789ABCDEF";
    unsigned b;
    unichar tmp[4 * 3];
    unsigned length;
    
    
    b = bytes(longM, longX);
    
    
    //for (unsigned i = 0; i < sizeof(tmp); ++i) tmp[i] = ' ';

    tmp[0] = hex[(_opcode >> 4) & 0x0f];
    tmp[1] = hex[_opcode & 0x0f];

    length = 2;

    
    if (b >= 2 && b <= 4)
    {
        
        while (--b)
        {
            tmp[length++] = ' ';
            tmp[length++] = hex[(operand >> 4) & 0x0f];
            tmp[length++] = hex[operand & 0x0f];
            operand >>= 8;
        }
        //length--;
    }
    
    return [NSString stringWithCharacters: tmp length: length];
    
}
#endif

unsigned OpCode::bytes(bool longM, bool longXY) const
{
    if (_addressMode == immediate)
    {
        switch (_mnemonic)
        {
            case LDX:
            case LDY:
            case CPX:
            case CPY:
                return longXY ? 3 : 2;
                
            case ADC:
            case AND:
            case BIT:
            case CMP:
            case EOR:
            case LDA:
            case ORA:
            case SBC:
                return longM ? 3 : 2;
                
            case PHK:
                return 3;
                
            default: 
                break;
                
        }
        
    }
    
    switch (_addressMode)
    {
        case implied:
            return 1;
            
        case immediate:
            return 2; // should be caught above.
            
        case zp:
        case zp_x:
        case zp_y:
        case zp_indirect:
        case zp_indirect_x:
        case zp_indirect_y:
        case zp_indirect_z:
        case zp_indirect_long:
        case zp_indirect_long_y:
        case stack_relative:
        case stack_relative_y:
            return 2;
            
        case zp_relative:
            // bbr0 zp,offset
            return 3;
            
            
        case absolute:
        case absolute_x:
        case absolute_y:
        case absolute_indirect:
        case absolute_indirect_x:
            return 3;
            
        case relative:
            return 2;

        case relative_long:
            return 3;
            
        case interrupt:
            // brk, cop, wdm 
            return 2;
            
        case absolute_long:
        case absolute_long_x:
            return 4;
            
        case absolute_indirect_long:
            // jml [abs]
            return 3;
            
        case block:
            // mvn, srcb, destb
            return 3;
            
        case kUndefinedAddressMode:
            return 0;
    }
    
}



