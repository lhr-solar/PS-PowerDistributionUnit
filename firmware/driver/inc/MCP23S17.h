#pragma once

#include "stm32xx_hal.h"

// DRIVER FOR MCP23S17 SPI GPIO Expander
// Datasheet: datasheets/MCP23017_MCP23S17.pdf (from repo root)

// REGISTERS -----------------------------------------------------------

#define MCP23S17_READ_OPCODE 0x41   // needs address if hardware addressing is enabled
#define MCP23S17_WRITE_OPCODE 0x40  // needs address if hardware addressing is enabled

									// For each port (A, B)...
#define MCP23S17_REG_IODIRA 0x00	// GPIO Direction Registers
#define MCP23S17_REG_IODIRB 0x01
#define MCP23S17_REG_IPOLA 0x02		// GPIO Input Polarity Registers
#define MCP23S17_REG_IPOLB 0x03
#define MCP23S17_REG_GPINTENA 0x04	// GPIO Interrupt Enable Registers
#define MCP23S17_REG_GPINTENB 0x05
#define MCP23S17_REG_DEFVALA 0x06	// GPIO Default Value Registers (for interrupt use)
#define MCP23S17_REG_DEFVALB 0x07
#define MCP23S17_REG_INTCONA 0x08	// GPIO Interrupt Mode Registers
#define MCP23S17_REG_INTCONB 0x09
#define MCP23S17_REG_IOCON 0x0A		// Configuration Registers
#define MCP23S17_REG_IOCON2 0x0B
#define MCP23S17_REG_GPPUA 0x0C		// GPIO Pull-up Enable/Disable Registers
#define MCP23S17_REG_GPPUB 0x0D
#define MCP23S17_REG_INTFA 0x0E		// Interrupt Flag Registers
#define MCP23S17_REG_INTFB 0x0F
#define MCP23S17_REG_INTCAPA 0x10	// Interrupt Captured GPIO State Registers
#define MCP23S17_REG_INTCAPB 0x11
#define MCP23S17_REG_GPIOA 0x12		// GPIO Registers
#define MCP23S17_REG_GPIOB 0x13
#define MCP23S17_REG_OLATA 0x14		// Output Latch Registers
#define MCP23S17_REG_OLATB 0x15
#define MCP23S17_REG_INVALID 0x16

#define MCP23S17_REG_INVALID_CHECK (reg_addr+num_regs-1) >= MCP23S17_REG_INVALID

#define MCP23S17_IOCON_MIRROR_MASK 0x40
#define MCP23S17_IOCON_HAEN_MASK 0x08
#define MCP23S17_IOCON_ODR_MASK 0x04
#define MCP23S17_IOCON_INTPOL_MASK 0x02

// DEVICE ---------------------------------------------------------------------

typedef struct {
	SPI_HandleTypeDef* spi;     // STM32 HAL SPI handle
	GPIO_TypeDef* cs_port;
	uint16_t cs_pin;
	uint8_t addr;              	// 3-bit hardware address << 1
								// A2, A1, A0 (set on IC pins)
								// If hardware addressing is disabled, set to 0.
								// Value is left-shifted by the init function when used.
} MCP23S17_HandleTypeDef;

typedef enum {
	MCP23S17_😢,				// MCP23S17 sad
	MCP23S17_🙂,				// MCP23S17 happy
} MCP23S17_Status;

// PORTS/PINS -----------------------------------------------------------------

typedef enum {
	MCP23S17_GPIOA,
	MCP23S17_GPIOB,
	MCP23S17_PORT_INVALID,
} MCP23S17_Port;

typedef enum {
	MCP23S17_PIN0,
	MCP23S17_PIN1,
	MCP23S17_PIN2,
	MCP23S17_PIN3,
	MCP23S17_PIN4,
	MCP23S17_PIN5,
	MCP23S17_PIN6,
	MCP23S17_PIN7,
	MCP23S17_PIN_INVALID,
} MCP23S17_Pin;

#define MCP23S17_PORT_PIN_INVALID_CHECK port >= MCP23S17_PORT_INVALID || pin >= MCP23S17_PIN_INVALID

// DEVICE CONFIGURATION -------------------------------------------------------

// Device Addressing Configuration
typedef enum {
	MCP23S17_ADDRESSING_DISABLE,		// Device addressing disabled.
	MCP23S17_ADDRESSING_ENABLE,			// Device addressing enabled.
} MCP23S17_Config_Addressing;

// INT Pins Mirroring
typedef enum {
	MCP23S17_CONFIG_INT_SEPARATE,		// INTA and INTB pins are separate.
	MCP23S17_CONFIG_INT_MIRRORED,		// INTA and INTB pins are tied internally.
} MCP23S17_Config_IntMirror;

// INT Pins Operating Mode
typedef enum {
	MCP23S17_CONFIG_INT_PP,				// INT pins are push-pull.
	MCP23S17_CONFIG_INT_OD,				// INT pins are open-drain.
} MCP23S17_Config_IntDrive;

// INT Pins Polarity (only applicable if configured as push-pull)
typedef enum {
	MCP23S17_CONFIG_INT_ACTIVE_LOW,		// INT pins are active low.
	MCP23S17_CONFIG_INT_ACTIVE_HIGH,	// INT pins are active high.
} MCP23S17_Config_IntPol;

// PIN CONFIGURATION ----------------------------------------------------------

// GPIO Direction
typedef enum {
	MCP23S17_DIR_OUTPUT,			// GPIO is configured as output.
	MCP23S17_DIR_INPUT,				// GPIO is configured as input.
} MCP23S17_Dir;

// GPIO Pullup Enabled/Disabled
typedef enum {
	MCP23S17_PULLUP_DISABLED, 		// Internal GPIO pullup disabled.
	MCP23S17_PULLUP_ENABLED,		// Internal GPIO pullup enabled.
} MCP23S17_Pullup;

// GPIO Input Polarity (only affects inputs)
typedef enum {
	MCP23S17_POLARITY_SAME,			// GPIO polarity as-is (active-high).
	MCP23S17_POLARITY_INVERT,		// GPIO polarity inverted (active-low).
} MCP23S17_InputPolarity;

// GPIO Interrupt Enabled/Disabled
typedef enum {
	MCP23S17_INT_DISABLED,			// Interrupts disabled on GPIO pin.
	MCP23S17_INT_ENABLED,			// Interrupts enabled on GPIO pin.
} MCP23S17_InterruptEnable;

typedef enum {
	MCP23S17_INT_ON_CHANGE,			// Interrupt occurs on pin state change.
	MCP23S17_INT_DEFVAL_COMPARE,	// Interrupt occurs on deviation from default pin state.
									// Default pin state is set in DEFVAL register.
} MCP23S17_InterruptMode;

// GPIO Output Pin Configuration Struct
typedef struct {
	// GPIO config
	MCP23S17_Port port;
	MCP23S17_Pin pin;
	bool initial_state;
} MCP23S17_PinConfigOutput;

// GPIO Input Pin Configuration Struct
typedef struct {
	// GPIO config
	MCP23S17_Port port;
	MCP23S17_Pin pin;
	MCP23S17_Pullup pullup;
	MCP23S17_InputPolarity inpol;
	// If interrupts disabled (inten == MCP23S17_INT_DISABLED), intmode and default_value have no effect.
	MCP23S17_InterruptEnable inten;
	MCP23S17_InterruptMode intmode;
	bool default_value;
} MCP23S17_PinConfigInput;

// DEVICE FUNCTIONS -----------------------------------------------------------
// meant to be used interally

/**
 * @brief	Writes to register(s) on MCP23S17 (primarily meant to be used internally).
 * @param	device MCP23S17 Device Handle
 * @param	reg_addr Starting Register Address
 * @param	data Pointer (uint8_t array) to data to write to register(s)
 * @param	num_regs Number of sequential register(s) to write
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_WriteRegs(MCP23S17_HandleTypeDef* device, uint8_t reg_addr, uint8_t* data, uint16_t num_regs);

/**
 * @brief	Reads register(s) on MCP23S17 (primarily meant to be used internally).
 * @param	device MCP23S17 Device Handle
 * @param	reg_addr Starting register address
 * @param	data Pointer (uint8_t array) to store data read from register(s)
 * @param	num_regs Number of sequential register(s) to read
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadRegs(MCP23S17_HandleTypeDef* device, uint8_t reg_addr, uint8_t* data, uint16_t num_regs);

// move this down to general
/**
 * @brief	Initializes MCP23S17 driver and device for use.
 * @param	device MCP23S17 Device Handle
 * @param	spi SPI Handle
 * @param	cs_port CS Port
 * @param	cs_pin CS Pin
 * @param   addr Device Hardware Address [A2, A1, A0] (set on IC pins) if hardware addressing is enabled, otherwise 000 - do not left-shift, will be done by init function
 * @param	int_mirror Device INT pin mirroring configuration (MCP23S17_CONFIG_INT_SEPARATE: interrupt pins independent,  MCP23S17_CONFIG_INT_MIRRORED: Interrupt pins tied internally)
 * @param	address_en Device hardware addressing configuration (MCP23S17_ADDRESSING_DISABLE: hardware addressing disabled, MCP23S17_ADDRESSING_ENABLE: hardware addressing enabled)
 * @param	int_odr Device INT pin mode configuration (MCP23S17_CONFIG_INT_PP: INT pin is push-pull, MCP23S17_CONFIG_INT_OD: INT pin is open-drain)
 * @param	int_pol Device INT pin polarity (MCP23S17_CONFIG_INT_ACTIVE_LOW: INT pin is active-low, MCP23S17_CONFIG_INT_ACTIVE_HIGH: INT pin is active-high)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_Init(MCP23S17_HandleTypeDef* device, SPI_HandleTypeDef* spi, GPIO_TypeDef* cs_port, uint16_t cs_pin, uint8_t addr, MCP23S17_Config_IntMirror int_mirror, MCP23S17_Config_Addressing address_en, MCP23S17_Config_IntDrive int_odr, MCP23S17_Config_IntPol int_pol);

// GENERAL GPIO FUNCTIONS -----------------------------------------------------

/**
 * @brief	Sets a GPIO pin's direction.
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	dir Pin direction: MCP23S17_DIR_OUTPUT or MCP23S17_DIR_INPUT
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetDirection_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, MCP23S17_Dir dir);

/**
 * @brief	Enables or disables a GPIO pin's pull-up resistor.
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	pu Pull up resistor configuration: MCP23S17_PULLUP_DISABLED or MCP23S17_PULLUP_ENABLED
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetPullup_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, MCP23S17_Pullup pu);

/**
 * @brief	Sets a GPIO pin's input polarity.
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	pol GPIO input polarity: MCP23S17_POLARITY_SAME or MCP23S17_POLARITY_INVERT
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetInputPolarity_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, MCP23S17_InputPolarity pol);

/**
 * @brief	Writes to a GPIO pin. (friendly)
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	state GPIO pin state
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_WriteGPIO_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, bool state);

/**
 * @brief	Writes to all GPIO pins. (NON-friendly)
 * @param	device MCP23S17 Device Handle
 * @param	state Pointer (uint8_t array) to GPIO pin states (16 bits), from port A7 (MSB) to port B0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_WriteGPIO_All(MCP23S17_HandleTypeDef* device, uint8_t* state);

/**
 * @brief	Reads a GPIO pin. 
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	state Pointer (bool) to store GPIO pin state
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadGPIO_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, bool* state);

/**
 * @brief	Reads all GPIO pins.
 * @param	device MCP23S17 Device Handle
 * @param	state Pointer (uint8_t array) to store GPIO pin states (16 bits), from port A7 (MSB) to port B0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadGPIO_All(MCP23S17_HandleTypeDef* device, uint8_t* state);

// INTERRUPT FUNCTIONS --------------------------------------------------------

/**
 * @brief	Enables or disables interrupts for a GPIO pin.  
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	inten GPIO interrupt configuration: MCP23S17_INT_DISABLED or MCP23S17_INT_ENABLED
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetInterruptEnable_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, MCP23S17_InterruptEnable inten);

/**
 * @brief	Sets a GPIO pin's interrupt mode
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	intmode GPIO interrupt mode: MCP23S17_INT_ON_CHANGE (interrupt on-change) or MCP23S17_INT_DEFVAL_COMPARE (default value comparison)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetInterruptMode_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, MCP23S17_InterruptMode intmode);

/**
 * @brief	Sets a GPIO pin's default value (for default value comparison interrupt mode)
 * @param	device MCP23S17 Device Handle
 * @param	port Device GPIO Port
 * @param	pin Device GPIO Pin
 * @param	defval GPIO default state
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetInterruptDefaultValue_Pin(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, MCP23S17_Pin pin, bool defval);

/**
 * @brief	Sets all GPIO pins' default value (for default value comparison interrupt mode)
 * @param	device MCP23S17 Device Handle
 * @param	defval Pointer (uint8_t array) to store GPIO pin default states (16 bits), from port A7 (MSB) to port B0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_SetInterruptDefaultValue_All(MCP23S17_HandleTypeDef* device, uint8_t* defval);

/**
 * @brief	Reads one GPIO port's interrupt status
 * @param	device MCP23S17 Device Handle
 * @param	port GPIO port to read interrupt status of
 * @param	state Pointer (uint8_t) to store interrupt status of GPIO port, from GPIOx7 (MSB) to GPIOx0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadInterruptStatus_Port(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, uint8_t* state);

/**
 * @brief	Reads all GPIO pins' interrupt status.
 * @param	device MCP23S17 Device Handle
 * @param	state Pointer (uint8_t array) to store GPIO pin interrupt status' (16 bits), from port A7 (MSB) to port B0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadInterruptStatus_All(MCP23S17_HandleTypeDef* device, uint8_t* state);

/**
 * @brief	Reads one GPIO port's captured state when interrupt occurred
 * @param	device MCP23S17 Device Handle
 * @param	port GPIO port to read interrupt status of
 * @param	state Pointer (uint8_t) to store captured state of GPIO port during interrupt, from GPIOx7 (MSB) to GPIOx0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadInterruptGPIOState_Port(MCP23S17_HandleTypeDef* device, MCP23S17_Port port, uint8_t* state);

/**
 * @brief	Reads all GPIO pins' captured state when interrupt occurred.
 * @param	device MCP23S17 Device Handle
 * @param	state Pointer (uint8_t array) to store GPIO pin captured states' (16 bits), from port A7 (MSB) to port B0 (LSB)
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_ReadInterruptGPIOState_All(MCP23S17_HandleTypeDef* device, uint8_t* state);

// SETUP COMBO FUNCTIONS ------------------------------------------------------

/**
 * @brief	Setup function to quickly initialize an output GPIO pin's direction and state in one call.
 * @param	device MCP23S17 Device Handle
 * @param	pin_config GPIO Output Pin Configuration Struct
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_TheOneStopShopForAllYourOutputGPIOInitNeedsOfOneSpecificPin_DoneInOneLineOrYourMoneyBack(MCP23S17_HandleTypeDef* device, MCP23S17_PinConfigOutput pin_config);

/**
 * @brief	Setup function to quickly initialize an input GPIO pin's direction, pull-up resistor, polarity, and interrupts in one call.
 * @param	device MCP23S17 Device Handle
 * @param	state GPIO Input Pin Configuration Struct
 * @returns MCP23S17 Status (MCP23S17_🙂 if successful, MCP23S17_😢 otherwise)
 */
MCP23S17_Status MCP23S17_GetAllOfYourSingleInputGPIOInitSetUpWithThisOneFunctionCallThatDoesEverythingForYourInstantly(MCP23S17_HandleTypeDef* device, MCP23S17_PinConfigInput pin_config);
