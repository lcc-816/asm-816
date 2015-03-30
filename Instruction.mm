//
//  Mnemonic.cpp
//  65calc02
//
//  Created by Kelvin Sherlock on 9/5/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#include "Instruction.h"
#include <Foundation/NSString.h>

#include <utility>

#include <tuple>
#include <unordered_map>

typedef std::tuple<Machine, Mnemonic, AddressMode> MMA;
typedef std::pair<Machine, Mnemonic> MachineMnemonic;


namespace std { 

        
        template<> struct hash<MachineMnemonic> : public unary_function<MachineMnemonic, size_t>
        {
            size_t operator()(MachineMnemonic mm) const
            {
                return mm.first | mm.second << 8;
            }
            
        };
        
        template<> struct hash<MMA> : public unary_function<MMA, size_t>
        {
            size_t operator()(MMA mma) const
            {
                return get<0>(mma) | get<1>(mma) << 8 | get<2>(mma) << 16;
            }
        };
        
};




#include "gen-i-table.h"


#pragma mark - 


// JSR long -> JSL
// JMP long -> JMP
// BRK #imm -> BRK xx
// PEA #imm -> PEA abs
// JMP (zp) -> JMP (abs)
// etc.

// need extra flag so, eg "brk <12" is an error (explicit address mode) 
// but "brk 12" is not
bool Instruction::coerce(Instruction &instruction, AddressMode &addressMode, bool explicitAddress)
{    
    Mnemonic mnemonic = instruction.mnemonic();
    Machine machine = instruction.machine();
    
    
    if (instruction.hasAddressMode(addressMode)) return true;

    // handle relative here as it is somewhat common.
    if (instruction.hasAddressMode(relative) || instruction.hasAddressMode(relative_long))
    {
        if (addressMode == zp || addressMode == absolute)
        {
            if (explicitAddress) return false;
            
            addressMode = relative;
            return true;
        }
    }
    
    
    if (machine == m65816 || machine == m65802)
    {
        // stuff that changes the instruction.
        if (mnemonic == JSR)
        {
            if (addressMode == absolute_long)
            {
                instruction = Instruction(machine, JSL);
                return true;
            }
        }
        else if (mnemonic == JMP)
        {
            if (addressMode == absolute_long)
            {
                instruction = Instruction(machine, JML);
                return true;
            }
            else if (addressMode == absolute_indirect_long)
            {
                instruction = Instruction(machine, JML);            
                return true;
            }
            else if (addressMode == zp_indirect_long)
            {
                if (explicitAddress) return false;
                
                addressMode = absolute_indirect_long;
                instruction = Instruction(machine, JML);                  
                return true;
            }
        }
        else if (mnemonic == PEA)
        {
            if (addressMode == immediate)
            {
                addressMode = absolute;
                return true;
            }
        }
    }

    // block / zp,relative ? 
    
    // brk #ff allowed even if explicit address...
    if (addressMode == immediate && instruction.hasAddressMode(interrupt))
    {
        addressMode = interrupt;
        return true;
    }
    
    if (explicitAddress) return false;
    
    
    // upgrade/downgrade the address mode.
    switch (addressMode)
    {
        default: 
            return false;
            
        case zp_indirect_long:
            // jml [abs]?
            if (instruction.hasAddressMode(absolute_indirect_long))
            {
                addressMode = absolute_indirect_long;
                return true;
            }
            break;
        
        case zp_indirect:
            // jsr (abs)?
            if (instruction.hasAddressMode(absolute_indirect))
            {
                addressMode = absolute_indirect;
                return true;
            }
            break;
            
        case zp_indirect_x:
            // jsr (abs,x)?
            if (instruction.hasAddressMode(absolute_indirect_x))
            {
                addressMode = absolute_indirect_x;
                return true;
            }
            break;
            
        case zp:
            // break zp?
            if (instruction.hasAddressMode(interrupt))
            {
                addressMode = interrupt;
                return true;
            }
            // hmm, any xxx zp -> xxx abs?
            if (instruction.hasAddressMode(absolute))
            {
                addressMode = absolute;
                return true;
            }
            break;
            
        case immediate:
            // break #immediate
            if (instruction.hasAddressMode(interrupt))
            {
                addressMode = interrupt;
                return true;
            }
            break;   
            
        case absolute_long:
            if (instruction.hasAddressMode(absolute))
            {
                addressMode = absolute;
                return true;
            }
            if (instruction.hasAddressMode(zp))
            {
                addressMode = zp;
            }
            break;
            
        case absolute_long_x:
            // abslong,x -> abs,x -> zp,x
            if (instruction.hasAddressMode(absolute_x))
            {
                addressMode = absolute_x;
                return true;
            }
            if (instruction.hasAddressMode(zp_x))
            {
                addressMode = zp_x;
            }
            break;
            
        case absolute_indirect:
            // (abs) -> (zp)
            if (instruction.hasAddressMode(zp_indirect))
            {
                addressMode = zp_indirect;
                return true;
            }
            break;
            
        case absolute_indirect_x:
            // (abs,x) -> (zp,x)
            if (instruction.hasAddressMode(zp_indirect_x))
            {
                addressMode = zp_indirect_x;
                return true;
            }
            break;            
            
        case absolute_indirect_long:
            // [abs] -> [zp]
            if (instruction.hasAddressMode(zp_indirect_long))
            {
                addressMode = zp_indirect_long;
                return true;
            }
            break;  
            
    }


    return false;
}

const_vector<Instruction> Instruction::Instructions(Machine machine)
{
#undef xxx
#define xxx(table) { return const_vector<Instruction>(table, sizeof(table) / sizeof(table[0])); }


    switch (machine)
    {
        case kUndefinedMachine:
            return const_vector<Instruction>();
            break;
            
        case m6502:
            xxx(m6502_instructions);
            break;
            
        case m65c02:
            xxx(m65c02_instructions);
            break;
            
        case mw65c02:
            xxx(mw65c02_instructions);
            break;
            
        case mr65c02:
            xxx(mr65c02_instructions);
            break;
            
        case m65802:
        case m65816:
        case m65816_e:
            xxx(m65816_instructions);
            break;
            
    }

#undef xxx
}







/*
 * Return the number of bytes for this instruction with specified address mode and m/x bits.
 *
 */
unsigned Instruction::bytes(AddressMode addressMode, bool longM, bool longXY) const
{
    if (addressMode == immediate)
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
            
                
            default: 
                break;

        }
        
    }
    
    switch (addressMode)
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

NSString *Instruction::formatOperand(uint32_t operand, AddressMode addressMode, uint16_t address, bool longM, bool longX)
{
    unsigned b;
    
    b = bytes(addressMode, longM, longX) - 1;
    if (addressMode == immediate && _machine == m65816)
    {
        if (b == 4)
            return [NSString stringWithFormat: @"#%04X", operand];
    }
    
    switch (addressMode)
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
                bool n;
                
                operand &= 0xffff;
                n = operand & 0x8000;
                tmp = address + 3 + operand;
                
                if (n) 
                    return [NSString stringWithFormat: @"%04X (*-%04X)",
                            tmp, 0x1000 - operand];
                
                else
                    return [NSString stringWithFormat: @"%04X (*+%04X)",
                            tmp, operand];                    
                
            }
        case relative:
            if (true)
            {
                uint16_t tmp;
                bool n;
                
                operand &= 0xff;
                n = operand & 0x80;
                tmp = address + 2 + (operand & 0xff);
                
                if (n)
                {
                    tmp += 0xff00;

                    return [NSString stringWithFormat: @"%04X (*-%02X)",
                               tmp, 0x100 - operand];
                }
                else
                    return [NSString stringWithFormat: @"%04X (*+%02X)",
                            tmp, operand];
            }

        case zp_relative:
            if (true)
            {
                uint16_t tmp;
                bool n;
                unsigned zp;
                
                zp = operand >> 8;
                operand &= 0xff;
                n = operand & 0x80;
                
                tmp = address + 2 + (operand & 0xff);
                if (n) 
                {
                    tmp += 0xff00;
                    
                    return [NSString stringWithFormat: @"%02X,%04X (*-%02X)",
                            zp, tmp, 0x100 - operand];
                } 
                else
                    return [NSString stringWithFormat: @"%02X,%04X (*+%02X)",
                            zp, tmp, operand];
                
            }
    }
    
    
    
    return nil;
    
}


static uint_least32_t NameToInt(const char *cp)
{
    uint_least32_t rv = 0;
    
    if (cp)
    {
        // todo -- allow RMB.0 format...
        
        char c;
        int i = 0;
        
        while ((c = *cp++))
        {
            if (i == 4) return 0;
            
            if (c >= '0' && c <= '9')
            {
                rv = (rv << 8) | c;
                ++i;
                continue;
            }
            
            if (c >= 'a' && c <= 'z')
            {
                c = c - 'a' + 'A'; 
                rv = (rv << 8) | c;
                ++i;
                continue;
            }
            
            if (c >= 'A' && c <= 'Z')
            {
                rv = (rv << 8) | c;
                ++i;
                continue; 
            }
            
            if (c == '.' && i == 3)
            {
                // RMB.0 is allowed.
                switch (rv)
                {
                    case 'R' << 16 | 'M' << 8 | 'B':
                    case 'S' << 16 | 'M' << 8 | 'B':
                    case 'B' << 16 | 'B' << 8 | 'R':
                    case 'B' << 16 | 'B' << 8 | 'S':
                        continue;
                    default:
                        break;
                }
            }
            
            return 0;
        }
        
    }
    
    return rv;
    
}


typedef std::pair<Machine, uint_least32_t> MachineName;

namespace std {
    
        template<> struct hash<MachineName> 
        : public unary_function<MachineName, size_t>
        {
            size_t operator()(MachineName mn) const
            {
                hash<uint_least32_t> hasher;
                return hasher(mn.first) + hasher(mn.second);
            }
            
        };
}
    
Instruction::Instruction(Machine machine, const char *mnemonic) : Instruction()
{
    //returns empty if unable to match mnemonic.
    // since all mnemonics are 3 (or 4, RMBx, et alia) characters,
    // they can be converted to a 24 or 32-bit integer.  So there.
    // Even better, the alphabet is 36 characters, so they can be stored in 5 * 4 = 20 bits.
    //

    typedef std::unordered_map<MachineName, Instruction> MachineNameInstructionHash;
    
    static MachineNameInstructionHash hash;
    
    if (machine == m65802) machine = m65816;
    
    if (hash.empty())
    {
        static Machine machine_array[] = {
                m6502, m65c02, mw65c02, mr65c02, m65816
        };
        
        uint_least32_t i;
        const_vector<Machine> machines(machine_array, sizeof(machine_array) / sizeof(machine_array[0]));
        
        const_vector<Machine>::iterator m_iter;
        
        
        for (m_iter = machines.begin(); m_iter != machines.end(); ++m_iter)
        {
            const_vector<Instruction> instructions;
            const_vector<Instruction>::iterator i_iter;

            instructions = Instructions(*m_iter);
            for (i_iter = instructions.begin(); i_iter != instructions.end(); ++i_iter)
            {
                i = NameToInt(i_iter->toString());
                
                hash[MachineName(*m_iter, i)] = *i_iter;
            }
        }
        
        
        // aliases..
        
        // INA/DEA are aliases for INC a/DEC a 
        // (not available for the 6502)
        
        i = NameToInt("INA");
        for (m_iter = machines.begin() + 1; m_iter != machines.end(); ++m_iter)
        {
            hash[MachineName(*m_iter, i)] = Instruction(*m_iter, INC, 1 << implied);
        }

        
        i = NameToInt("DEA");
        for (m_iter = machines.begin() + 1; m_iter != machines.end(); ++m_iter)
        {
            hash[MachineName(*m_iter, i)] = Instruction(*m_iter, DEC, 1 << implied);
        }
        
        // BLT = BCC
        // BGE = BCS        
        i = NameToInt("BLT");
        for (m_iter = machines.begin(); m_iter != machines.end(); ++m_iter)
        {
            hash[MachineName(*m_iter, i)] = Instruction(*m_iter, BCC, 1 << relative);
        }

        i = NameToInt("BGE");
        for (m_iter = machines.begin(); m_iter != machines.end(); ++m_iter)
        {
            hash[MachineName(*m_iter, i)] = Instruction(*m_iter, BCS, 1 << relative);
        }
        
        // 65816 -- 
        // TAD -> TCD
        // TAS -> TCS
        // TDA -> TDC
        // TSA -> TSC
        // SWA -> XBA
        
        hash[MachineName(m65816, NameToInt("TAD"))] = Instruction(m65816, TCD, 1 << implied);
        hash[MachineName(m65816, NameToInt("TCS"))] = Instruction(m65816, TCS, 1 << implied);
        hash[MachineName(m65816, NameToInt("TDA"))] = Instruction(m65816, TDC, 1 << implied);
        hash[MachineName(m65816, NameToInt("TSA"))] = Instruction(m65816, TSC, 1 << implied);
        hash[MachineName(m65816, NameToInt("SWA"))] = Instruction(m65816, XBA, 1 << implied);
        
        
        
    }
    
    MachineNameInstructionHash::iterator iter;
    
    MachineName mn(machine, NameToInt(mnemonic));
    
    iter = hash.find(mn);
    if (iter == hash.end()) return;
    
    *this = iter->second;

}




Instruction::Instruction(Machine machine, Mnemonic mnemonic) : Instruction()
{

    
    typedef std::unordered_map<MachineMnemonic, uint_least32_t> MachineMnemonicModesHash;
    
    
    static MachineMnemonicModesHash hash;
    
    if (hash.empty())
    {
        static Machine machine_array[] = {
            m6502, m65c02, mw65c02, mr65c02, m65816
        };
        
        const_vector<Machine> machines(machine_array, sizeof(machine_array) / sizeof(machine_array[0]));
        
        const_vector<Machine>::iterator m_iter;
        
        
        for (m_iter = machines.begin(); m_iter != machines.end(); ++m_iter)
        {
            const_vector<Instruction> instructions;
            const_vector<Instruction>::iterator i_iter;
            
            instructions = Instructions(*m_iter);
            for (i_iter = instructions.begin(); i_iter != instructions.end(); ++i_iter)
            {
                hash[MachineMnemonic(*m_iter, i_iter->mnemonic())] = i_iter->addressModes();
            }
        }
        
        
    }
    
    MachineMnemonicModesHash::iterator iter;
    MachineMnemonic key(machine, mnemonic);
    
    iter = hash.find(key);
    
    if (iter == hash.end()) return;
    
    _machine = machine;
    _mnemonic = mnemonic;
    _addressModes = iter->second;
}






