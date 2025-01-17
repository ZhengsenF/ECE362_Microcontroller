################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/game.c \
../src/main.c \
../src/midi.c \
../src/music.c \
../src/scale.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

OBJS += \
./src/game.o \
./src/main.o \
./src/midi.o \
./src/music.o \
./src/scale.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

C_DEPS += \
./src/game.d \
./src/main.d \
./src/midi.d \
./src/music.d \
./src/scale.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F051R8Tx -DSTM32F0DISCOVERY -DDEBUG -DSTM32F051 -DUSE_STDPERIPH_DRIVER -I"D:/ECE362/Project3/Utilities" -I"D:/ECE362/Project3/StdPeriph_Driver/inc" -I"D:/ECE362/Project3/inc" -I"D:/ECE362/Project3/CMSIS/device" -I"D:/ECE362/Project3/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


