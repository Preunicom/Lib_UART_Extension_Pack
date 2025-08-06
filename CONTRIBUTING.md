# Contributing to Lib_UART_Extension_Pack

Your are welcome to contribute to the project by creatign pull requests.  

## Setup
Before contributing make sure you have the ExtPack project running on an FPGA (and you are ready to adjust it's configuration).  
Additionally, make sure you have some supported microcontrollers ready to test your code with.

## Add unit

Please orientate your code style on other implementation of the project.
Stay modular and use the layer architecture.

1) Add **unit file** (.h and .c if applicable) in the Util folder
2) Add **advanced unit file** (.h and .c if applicable) in the Service folder if there are more advanced features available for the unit
3) Add and document the **unit type macro** in ExtPack_Defs.h
4) Add your unit in the **overview table of the README** file.
5) Implement and document (Doxygen comments) the **functionality** (orientate on other unit implementations)
6) **Test your code** on different controllers.
7) **Review your documentation** (Doxygen + README)
8) Add **at least one example** using the new unit.
9) **Test your example** on different controllers.

## Add microcontroller without family

See [below](#Add-microcontroller-family) but implement the functionality directly in the controller file (.c) instead of including the family file (.c) there.

## Add microcontroller family

1) **Add a controller family file** (.c) using the naming convention used by the other families.
2) **Add files for all controllers** (.c) in the family using the naming convention used by the other controllers.
3) **Implement** following features matching to the function names of the low level header file:
    - Fallback default value for SEND_BUF_LEN
    - Sending with buffer for 2 byte pairs
    - Sending without buffer for 2 byte pairs
    - Receiving of two byte pairs
    - Recv: Unit check (no access mode bits set)
    - Recv: Calling the process data function (stated in the low level header as extern)
    - Entering/Exiting critical zone
4) **Include your controller family file** (.c) in all controller files (.c).
5) **Document** your implementation and **review** it (especially the doxygen generated documentation). (**README overview table + Doxygen comments**)
6) **Test your implementation** on all microcontrollers with different units and functions of Util and Service layer.

## Issues

GitHub issues are used to report bugs or request features in this project.  
Please use the issue templates to create a new issue and fill it out properly.

## License

By contributing to the project, you agree that your contributions will be licensed
under the LICENSE file in the root directory of this source tree.
   
