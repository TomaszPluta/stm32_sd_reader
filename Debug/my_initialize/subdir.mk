################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../my_initialize/initialize.c 

OBJS += \
./my_initialize/initialize.o 

C_DEPS += \
./my_initialize/initialize.d 


# Each subdirectory must supply rules for building sources it contributes
my_initialize/%.o: ../my_initialize/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"/home/tomek/stm32workspace/FreeRTOS_proj/FreeRTOS/include" -I"/home/tomek/stm32workspace/FreeRTOS_proj/my_initialize" -I"/home/tomek/stm32workspace/FreeRTOS_proj/helpers" -I"/home/tomek/stm32workspace/FreeRTOS_proj/FATFs" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


