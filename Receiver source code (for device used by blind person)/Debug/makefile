################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include wav_player/subdir.mk
-include bluetooth/subdir.mk
-include LCD/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
INZYNIERKA_niewidomy.lss \

SIZEDUMMY += \
sizedummy \


# All Target
all: INZYNIERKA_niewidomy.elf secondary-outputs

# Tool invocations
INZYNIERKA_niewidomy.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,INZYNIERKA_niewidomy.map -mmcu=atmega32 -o "INZYNIERKA_niewidomy.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

INZYNIERKA_niewidomy.lss: INZYNIERKA_niewidomy.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S INZYNIERKA_niewidomy.elf  >"INZYNIERKA_niewidomy.lss"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: INZYNIERKA_niewidomy.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 INZYNIERKA_niewidomy.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) INZYNIERKA_niewidomy.elf
	-@echo ' '

secondary-outputs: $(LSS) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
