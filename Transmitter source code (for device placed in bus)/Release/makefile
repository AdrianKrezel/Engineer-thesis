################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
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
INZYNIERKA_autobus.lss \

FLASH_IMAGE += \
INZYNIERKA_autobus.hex \

EEPROM_IMAGE += \
INZYNIERKA_autobus.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: INZYNIERKA_autobus.elf secondary-outputs

# Tool invocations
INZYNIERKA_autobus.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,INZYNIERKA_autobus.map -mmcu=atmega32 -o "INZYNIERKA_autobus.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

INZYNIERKA_autobus.lss: INZYNIERKA_autobus.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S INZYNIERKA_autobus.elf  >"INZYNIERKA_autobus.lss"
	@echo 'Finished building: $@'
	@echo ' '

INZYNIERKA_autobus.hex: INZYNIERKA_autobus.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -O ihex INZYNIERKA_autobus.elf  "INZYNIERKA_autobus.hex"
	@echo 'Finished building: $@'
	@echo ' '

INZYNIERKA_autobus.eep: INZYNIERKA_autobus.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex INZYNIERKA_autobus.elf  "INZYNIERKA_autobus.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: INZYNIERKA_autobus.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 INZYNIERKA_autobus.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(FLASH_IMAGE)$(ELFS)$(LSS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS) INZYNIERKA_autobus.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
