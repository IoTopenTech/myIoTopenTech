################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/adc_if.c \
../Core/Src/dma.c \
../Core/Src/flash_if.c \
../Core/Src/gpio.c \
../Core/Src/main.c \
../Core/Src/rtc.c \
../Core/Src/stm32_lpm_if.c \
../Core/Src/stm32wlxx_hal_msp.c \
../Core/Src/stm32wlxx_it.c \
../Core/Src/subghz.c \
../Core/Src/sys_app.c \
../Core/Src/sys_debug.c \
../Core/Src/sys_sensors.c \
../Core/Src/system_stm32wlxx.c \
../Core/Src/timer_if.c \
../Core/Src/usart.c \
../Core/Src/usart_if.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/adc_if.o \
./Core/Src/dma.o \
./Core/Src/flash_if.o \
./Core/Src/gpio.o \
./Core/Src/main.o \
./Core/Src/rtc.o \
./Core/Src/stm32_lpm_if.o \
./Core/Src/stm32wlxx_hal_msp.o \
./Core/Src/stm32wlxx_it.o \
./Core/Src/subghz.o \
./Core/Src/sys_app.o \
./Core/Src/sys_debug.o \
./Core/Src/sys_sensors.o \
./Core/Src/system_stm32wlxx.o \
./Core/Src/timer_if.o \
./Core/Src/usart.o \
./Core/Src/usart_if.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/adc_if.d \
./Core/Src/dma.d \
./Core/Src/flash_if.d \
./Core/Src/gpio.d \
./Core/Src/main.d \
./Core/Src/rtc.d \
./Core/Src/stm32_lpm_if.d \
./Core/Src/stm32wlxx_hal_msp.d \
./Core/Src/stm32wlxx_it.d \
./Core/Src/subghz.d \
./Core/Src/sys_app.d \
./Core/Src/sys_debug.d \
./Core/Src/sys_sensors.d \
./Core/Src/system_stm32wlxx.d \
./Core/Src/timer_if.d \
./Core/Src/usart.d \
./Core/Src/usart_if.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DCORE_CM4 -DUSE_HAL_DRIVER -DSTM32WLE5xx -c -I../Core/Inc -I../LoRaWAN/App -I../LoRaWAN/Target -I../Drivers/STM32WLxx_HAL_Driver/Inc -I../Drivers/STM32WLxx_HAL_Driver/Inc/Legacy -I../Utilities/trace/adv_trace -I../Utilities/misc -I../Utilities/sequencer -I../Utilities/timer -I../Utilities/lpm/tiny_lpm -I../Middlewares/Third_Party/LoRaWAN/LmHandler/Packages -I../Middlewares/Third_Party/SubGHz_Phy -I../Middlewares/Third_Party/SubGHz_Phy/stm32_radio_driver -I../Drivers/CMSIS/Device/ST/STM32WLxx/Include -I../Middlewares/Third_Party/LoRaWAN/Crypto -I../Middlewares/Third_Party/LoRaWAN/Mac/Region -I../Middlewares/Third_Party/LoRaWAN/Mac -I../Middlewares/Third_Party/LoRaWAN/LmHandler -I../Middlewares/Third_Party/LoRaWAN/Utilities -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/adc_if.d ./Core/Src/adc_if.o ./Core/Src/adc_if.su ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/flash_if.d ./Core/Src/flash_if.o ./Core/Src/flash_if.su ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/rtc.su ./Core/Src/stm32_lpm_if.d ./Core/Src/stm32_lpm_if.o ./Core/Src/stm32_lpm_if.su ./Core/Src/stm32wlxx_hal_msp.d ./Core/Src/stm32wlxx_hal_msp.o ./Core/Src/stm32wlxx_hal_msp.su ./Core/Src/stm32wlxx_it.d ./Core/Src/stm32wlxx_it.o ./Core/Src/stm32wlxx_it.su ./Core/Src/subghz.d ./Core/Src/subghz.o ./Core/Src/subghz.su ./Core/Src/sys_app.d ./Core/Src/sys_app.o ./Core/Src/sys_app.su ./Core/Src/sys_debug.d ./Core/Src/sys_debug.o ./Core/Src/sys_debug.su ./Core/Src/sys_sensors.d ./Core/Src/sys_sensors.o ./Core/Src/sys_sensors.su ./Core/Src/system_stm32wlxx.d ./Core/Src/system_stm32wlxx.o ./Core/Src/system_stm32wlxx.su ./Core/Src/timer_if.d ./Core/Src/timer_if.o ./Core/Src/timer_if.su ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su ./Core/Src/usart_if.d ./Core/Src/usart_if.o ./Core/Src/usart_if.su

.PHONY: clean-Core-2f-Src

