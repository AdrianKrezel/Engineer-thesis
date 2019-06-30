################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../wav_player/dzwiek.c \
../wav_player/mmc.c \
../wav_player/pff.c 

OBJS += \
./wav_player/dzwiek.o \
./wav_player/mmc.o \
./wav_player/pff.o 

C_DEPS += \
./wav_player/dzwiek.d \
./wav_player/mmc.d \
./wav_player/pff.d 


# Each subdirectory must supply rules for building sources it contributes
wav_player/%.o: ../wav_player/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


