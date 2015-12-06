################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../FreeRTOS/source/croutine.c \
../FreeRTOS/source/event_groups.c \
../FreeRTOS/source/heap_1.c \
../FreeRTOS/source/list.c \
../FreeRTOS/source/port.c \
../FreeRTOS/source/queue.c \
../FreeRTOS/source/tasks.c \
../FreeRTOS/source/timers.c 

OBJS += \
./FreeRTOS/source/croutine.o \
./FreeRTOS/source/event_groups.o \
./FreeRTOS/source/heap_1.o \
./FreeRTOS/source/list.o \
./FreeRTOS/source/port.o \
./FreeRTOS/source/queue.o \
./FreeRTOS/source/tasks.o \
./FreeRTOS/source/timers.o 

C_DEPS += \
./FreeRTOS/source/croutine.d \
./FreeRTOS/source/event_groups.d \
./FreeRTOS/source/heap_1.d \
./FreeRTOS/source/list.d \
./FreeRTOS/source/port.d \
./FreeRTOS/source/queue.d \
./FreeRTOS/source/tasks.d \
./FreeRTOS/source/timers.d 


# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/source/%.o: ../FreeRTOS/source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f1-stdperiph" -I"/home/tomek/stm32workspace/FreeRTOS_proj/FreeRTOS/include" -I"/home/tomek/stm32workspace/FreeRTOS_proj/my_initialize" -I"/home/tomek/stm32workspace/FreeRTOS_proj/helpers" -I"/home/tomek/stm32workspace/FreeRTOS_proj/FATFs" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


