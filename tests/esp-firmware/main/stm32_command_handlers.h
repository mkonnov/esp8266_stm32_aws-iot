#ifndef STM32_COMMAND_HANDLERS_H
#define STM32_COMMAND_HANDLERS_H

int command_stm32_open(const uint8_t *data, size_t len);
int command_stm32_close(const uint8_t *data, size_t len);

#endif /* STM32_COMMAND_HANDLERS_H */
