# Core

## Functionality

This layer contains the main functionality of the ExtPack library being the interface between hardware (low level implementation) and more abstract logical layers.  
More in detail it contains hardware independent core functionality used for (nearly) every ExtPack interaction.  

This includes:
- Plain command data transmit / receive
- Command formatting
- Events
- Transmit ring buffers
- Unit (meta)data storage
- Constant definitions
- ExtPack (unit) initialization
- ISR callback management