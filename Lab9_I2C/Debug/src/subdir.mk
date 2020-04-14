################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../src/micro_wait.s \
../src/stdio.s 

C_SRCS += \
../src/fifo.c \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

O_SRCS += \
../src/helper.o 

OBJS += \
./src/fifo.o \
./src/main.o \
./src/micro_wait.o \
./src/stdio.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/fifo.d \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -DSTM32F0DISCOVERY -DDEBUG -DSTM32F051 -DUSE_STDPERIPH_DRIVER -I"D:/ECE362/Lab9/Utilities" -I"D:/ECE362/Lab9/StdPeriph_Driver/inc" -I"D:/ECE362/Lab9/inc" -I"D:/ECE362/Lab9/CMSIS/device" -I"D:/ECE362/Lab9/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"D:/ECE362/Lab9/Utilities" -I"D:/ECE362/Lab9/StdPeriph_Driver/inc" -I"D:/ECE362/Lab9/inc" -I"D:/ECE362/Lab9/CMSIS/device" -I"D:/ECE362/Lab9/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


