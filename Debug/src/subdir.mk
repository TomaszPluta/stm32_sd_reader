################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BlinkLed.c \
../src/Timer.c \
../src/_write.c \
../src/ks0108.c \
../src/main.c \
../src/sd_stm32.c 

OBJS += \
./src/BlinkLed.o \
./src/Timer.o \
./src/_write.o \
./src/ks0108.o \
./src/main.o \
./src/sd_stm32.o 

C_DEPS += \
./src/BlinkLed.d \
./src/Timer.d \
./src/_write.d \
./src/ks0108.d \
./src/main.d \
./src/sd_stm32.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"/home/tomek/stm32workspace/FreeRTOS_proj/FreeRTOS/include" -I"/home/tomek/stm32workspace/FreeRTOS_proj/my_initialize" -I"/home/tomek/stm32workspace/FreeRTOS_proj/helpers" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


