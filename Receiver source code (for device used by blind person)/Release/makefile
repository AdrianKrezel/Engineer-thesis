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

FLASH_IMAGE += \
INZYNIERKA_niewidomy.hex \

EEPROM_IMAGE += \
INZYNIERKA_niewidomy.eep \

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

INZYNIERKA_niewidomy.hex: INZYNIERKA_niewidomy.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex INZYNIERKA_niewidomy.elf  "INZYNIERKA_niewidomy.hex"
	@echo 'Finished building: $@'
	@echo ' '

INZYNIERKA_niewidomy.eep: INZYNIERKA_niewidomy.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex INZYNIERKA_niewidomy.elf  "INZYNIERKA_niewidomy.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: INZYNIERKA_niewidomy.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 INZYNIERKA_niewidomy.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) INZYNIERKA_niewidomy.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
