//
//  Machine.cpp
//  65calc02
//
//  Created by Kelvin Sherlock on 9/7/2011.
//  Copyright 2011 Kelvin W Sherlock LLC. All rights reserved.
//

#include "Machine.h"

#include <Foundation/NSString.h>
#include <CoreFoundation/CFString.h>


#include "gen-m-table.h"

const char *AddressModeToString(AddressMode mode)
{
    switch (mode)
    {
        case kUndefinedAddressMode:
            return NULL;
            
        case implied:
            return "Implied";
            
        case relative:
            return "Relative";
            
        case relative_long:
            return "Relative Long";
        
        case immediate:
            return "Immediate";
        case interrupt:
            return "Interrupt";
            
        case zp:
            return "Zero Page";
        case zp_x:
            return "Zero Page,X";
        case zp_y:
            return "Zero Page,Y";
        case zp_indirect:
            return "(Zero Page)";
        case zp_indirect_x:
            return "(Zero Page,X)";
        case zp_indirect_y:
            return "(Zero Page),Y";
        case zp_relative:
            return "Zero Page,Relative";
        case zp_indirect_long:
            return "[Zero Page]";
        case zp_indirect_long_y:
            return "[Zero Page],Y";
        case absolute:
            return "Absolute";
        case absolute_x:
            return "Absolute,X";
        case absolute_y:
            return "Absolute,Y";
        case absolute_indirect:
            return "(Absolute)";
        case absolute_indirect_x:
            return "(Absolute,X)";
        case absolute_indirect_long:
            return "[Absolute]";
        case absolute_long:
            return "Absolute Long";
        case absolute_long_x:
            return "Absolute Long,X";
            
        case block:
            return "Block Move";
        case stack_relative:
            return "Stack Relative,S";
        case stack_relative_y:
            return "(Stack Relative,S),Y";
            
    }
    return NULL;
}

NSString *AddressModeToNSString(AddressMode mode)
{
    switch (mode)
    {
        case kUndefinedAddressMode:
            return nil;
            
        case implied:
            return @"Implied";
            
        case relative:
            return @"Relative";
        case relative_long:
            return @"Relative Long";
            
        case immediate:
            return @"Immediate";
        case interrupt:
            return @"Interrupt";
            
        case zp:
            return @"Zero Page";
        case zp_x:
            return @"Zero Page,X";
        case zp_y:
            return @"Zero Page,Y";
        case zp_indirect:
            return @"(Zero Page)";
        case zp_indirect_x:
            return @"(Zero Page,X)";
        case zp_indirect_y:
            return @"(Zero Page),Y";
        case zp_relative:
            return @"Zero Page,Relative";
        case zp_indirect_long:
            return @"[Zero Page]";
        case zp_indirect_long_y:
            return @"[Zero Page],Y";
        case absolute:
            return @"Absolute";
        case absolute_x:
            return @"Absolute,X";
        case absolute_y:
            return @"Absolute,Y";
        case absolute_indirect:
            return @"(Absolute)";
        case absolute_indirect_x:
            return @"(Absolute,X)";
        case absolute_indirect_long:
            return @"[Absolute]";
        case absolute_long:
            return @"Absolute Long";
        case absolute_long_x:
            return @"Absolute Long,X";
            
        case block:
            return @"Block Move";
        case stack_relative:
            return @"Stack Relative,S";
        case stack_relative_y:
            return @"(Stack Relative,S),Y";
            
    }
    return NULL;
}


CFStringRef AddressModeToCFString(AddressMode mode)
{
    switch (mode)
    {
        case kUndefinedAddressMode:
            return nil;
            
        case implied:
            return CFSTR("Implied");
            
        case relative:
            return CFSTR("Relative");
            
        case relative_long:
            return CFSTR("Relative Long");
            
        case immediate:
            return CFSTR("Immediate");
        case interrupt:
            return CFSTR("Interrupt");
            
        case zp:
            return CFSTR("Zero Page");
        case zp_x:
            return CFSTR("Zero Page,X");
        case zp_y:
            return CFSTR("Zero Page,Y");
        case zp_indirect:
            return CFSTR("(Zero Page)");
        case zp_indirect_x:
            return CFSTR("(Zero Page,X)");
        case zp_indirect_y:
            return CFSTR("(Zero Page),Y");
        case zp_relative:
            return CFSTR("Zero Page,Relative");
        case zp_indirect_long:
            return CFSTR("[Zero Page]");
        case zp_indirect_long_y:
            return CFSTR("[Zero Page],Y");
        case absolute:
            return CFSTR("Absolute");
        case absolute_x:
            return CFSTR("Absolute,X");
        case absolute_y:
            return CFSTR("Absolute,Y");
        case absolute_indirect:
            return CFSTR("(Absolute)");
        case absolute_indirect_x:
            return CFSTR("(Absolute,X)");
        case absolute_indirect_long:
            return CFSTR("[Absolute]");
        case absolute_long:
            return CFSTR("Absolute Long");
        case absolute_long_x:
            return CFSTR("Absolute Long,X");
            
        case block:
            return CFSTR("Block Move");
        case stack_relative:
            return CFSTR("Stack Relative,S");
        case stack_relative_y:
            return CFSTR("(Stack Relative,S),Y");
            
    }
    return NULL;
}



NSString *HexTable[256] = {

    @"00", @"01", @"02", @"03", @"04", @"05", @"06", @"07", 
    @"08", @"09", @"0A", @"0B", @"0C", @"0D", @"0E", @"0F", 
    @"10", @"11", @"12", @"13", @"14", @"15", @"16", @"17", 
    @"18", @"19", @"1A", @"1B", @"1C", @"1D", @"1E", @"1F", 
    @"20", @"21", @"22", @"23", @"24", @"25", @"26", @"27", 
    @"28", @"29", @"2A", @"2B", @"2C", @"2D", @"2E", @"2F", 
    @"30", @"31", @"32", @"33", @"34", @"35", @"36", @"37", 
    @"38", @"39", @"3A", @"3B", @"3C", @"3D", @"3E", @"3F", 
    @"40", @"41", @"42", @"43", @"44", @"45", @"46", @"47", 
    @"48", @"49", @"4A", @"4B", @"4C", @"4D", @"4E", @"4F", 
    @"50", @"51", @"52", @"53", @"54", @"55", @"56", @"57", 
    @"58", @"59", @"5A", @"5B", @"5C", @"5D", @"5E", @"5F", 
    @"60", @"61", @"62", @"63", @"64", @"65", @"66", @"67", 
    @"68", @"69", @"6A", @"6B", @"6C", @"6D", @"6E", @"6F", 
    @"70", @"71", @"72", @"73", @"74", @"75", @"76", @"77", 
    @"78", @"79", @"7A", @"7B", @"7C", @"7D", @"7E", @"7F", 
    @"80", @"81", @"82", @"83", @"84", @"85", @"86", @"87", 
    @"88", @"89", @"8A", @"8B", @"8C", @"8D", @"8E", @"8F", 
    @"90", @"91", @"92", @"93", @"94", @"95", @"96", @"97", 
    @"98", @"99", @"9A", @"9B", @"9C", @"9D", @"9E", @"9F", 
    @"A0", @"A1", @"A2", @"A3", @"A4", @"A5", @"A6", @"A7", 
    @"A8", @"A9", @"AA", @"AB", @"AC", @"AD", @"AE", @"AF", 
    @"B0", @"B1", @"B2", @"B3", @"B4", @"B5", @"B6", @"B7", 
    @"B8", @"B9", @"BA", @"BB", @"BC", @"BD", @"BE", @"BF", 
    @"C0", @"C1", @"C2", @"C3", @"C4", @"C5", @"C6", @"C7", 
    @"C8", @"C9", @"CA", @"CB", @"CC", @"CD", @"CE", @"CF", 
    @"D0", @"D1", @"D2", @"D3", @"D4", @"D5", @"D6", @"D7", 
    @"D8", @"D9", @"DA", @"DB", @"DC", @"DD", @"DE", @"DF", 
    @"E0", @"E1", @"E2", @"E3", @"E4", @"E5", @"E6", @"E7", 
    @"E8", @"E9", @"EA", @"EB", @"EC", @"ED", @"EE", @"EF", 
    @"F0", @"F1", @"F2", @"F3", @"F4", @"F5", @"F6", @"F7", 
    @"F8", @"F9", @"FA", @"FB", @"FC", @"FD", @"FE", @"FF", 
};

NSString *HexChars[16] = {
    @"0", @"1", @"2", @"3",
    @"4", @"5", @"6", @"7",    
    @"8", @"9", @"A", @"B",
    @"C", @"D", @"E", @"F",
};
