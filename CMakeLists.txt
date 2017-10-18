
cmake_minimum_required(VERSION 3.1)
project(asm-816 VERSION 0.0 LANGUAGES CXX C)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)


option(EGGS_VARIANT_WITH_TESTS "" OFF)


add_subdirectory(variant)
add_subdirectory(cxx)
add_subdirectory(fmt)
add_subdirectory(afp)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_custom_command(
	OUTPUT grammar.cpp grammar.h grammar.out
	COMMAND lemon++ grammar.lemon
	COMMAND mv -f grammar.cpp grammar.h grammar.out ${CMAKE_CURRENT_BINARY_DIR}/
	MAIN_DEPENDENCY grammar.lemon 
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# from https://github.com/gsauthof/cmake-ragel
macro(RAGEL_TARGET Name Input Output)
	set(RAGEL_EXECUTABLE "ragel")
	set(RAGEL_TARGET_usage
	          "RAGEL_TARGET(<Name> <Input> <Output> [COMPILE_FLAGS <string>]")
	if(${ARGC} GREATER 3)
	  if(${ARGC} EQUAL 5)
	    if("${ARGV3}" STREQUAL "COMPILE_FLAGS")
	      set(RAGEL_EXECUTABLE_opts  "${ARGV4}")
	      separate_arguments(RAGEL_EXECUTABLE_opts)
	    else()
	      message(SEND_ERROR ${RAGEL_TARGET_usage})
	    endif()
	  else()
	    message(SEND_ERROR ${RAGEL_TARGET_usage})
	  endif()
	endif()

	add_custom_command(OUTPUT ${Output}
	  COMMAND ${RAGEL_EXECUTABLE}
	  ARGS    ${RAGEL_EXECUTABLE_opts} -o${CMAKE_CURRENT_BINARY_DIR}/${Output} ${Input}
	  DEPENDS ${Input}
	  COMMENT
	      "[RAGEL][${Name}] Compiling state machine with Ragel ${RAGEL_VERSION}"
	  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

	set(RAGEL_${Name}_DEFINED       TRUE)
	set(RAGEL_${Name}_OUTPUTS       ${Output})
	set(RAGEL_${Name}_INPUT         ${Input})
	set(RAGEL_${Name}_COMPILE_FLAGS ${RAGEL_EXECUTABLE_opts})
endmacro()

RAGEL_TARGET(lexer lexer.rl lexer.cpp COMPILE_FLAGS "-p -G1 -L -C")


set(SRC main.cpp grammar.cpp parser.cpp lexer.cpp Instruction.cpp Machine.cpp OpCode.cpp
	Expression.cpp Expression_evaluate.cpp Expression_simplify.cpp Expression_visitor.cpp
	dp_register_set.cpp classify.cpp peephole.cpp classify.cpp basic_block.cpp
	intern.cpp symbol_table.cpp data.cpp omf.cpp assign_pc.cpp assign_reg.cpp
	line.cpp branch.cpp propagate_const.cpp reg_const.cpp
	register_lifetime.cpp register_set.cpp
	opt_consolidate.cpp opt_dead_block_elimination.cpp opt_data_flow_analysis.cpp
	opt_merge_blocks.cpp
	printer.cpp
	)


add_executable(asm-816 ${SRC})
target_link_libraries(asm-816 CXX_LIB fmt afp mpark_variant)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	# Clang or AppleClang
	target_compile_definitions(asm-816 PRIVATE _LIBCPP_DISABLE_AVAILABILITY)
	target_compile_options(asm-816 PRIVATE -Wall -Wno-multichar -Wno-unused-const-variable)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	# gcc
	target_compile_options(asm-816 PRIVATE -Wall -Wno-multichar)
endif()
