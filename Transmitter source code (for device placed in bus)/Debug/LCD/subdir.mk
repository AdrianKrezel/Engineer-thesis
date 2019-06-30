################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/LCD.c \
../LCD/lcd44780.c 

OBJS += \
./LCD/LCD.o \
./LCD/lcd44780.o 

C_DEPS += \
./LCD/LCD.d \
./LCD/lcd44780.d 


# Each subdirectory must supply rules for building sources it contributes
LCD/%.o: ../LCD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


