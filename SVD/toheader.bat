rem SVDConv EFR32FG14P231F256GM48_full.svd --generate=header --fields=macro --fields=enum -b errors.log
rem SVDConv EFR32FG23A010F128GM40_full.svd --generate=header --fields=macro -b errors.log
rem SVDConv EFR32FG23A010F128GM40_full.svd --generate=header --fields=struct -b errors.log
rem SVDConv EFR32FG23A010F128GM40_full.svd --generate=header --fields=enum -b errors.log
rem SVDConv EFR32FG23A010F128GM40_short.svd --generate=sfr -b errors.log

SVDConv tremo.svd --generate=sfr -b errors.log

rem -b	Log File	Specify the log file name for writing messages. Default: screen.
rem -o	Output Path	Specify an output path for the generated device header file or log file. Default: current directory.
rem --generate=header	Generate Device Header File	Generates the device header file. The name of the generated file is derived from the value of the tag <devicename> in the CMSIS-SVD file. Refer to device.
rem --generate=partition	Generate Partition file for Cortex-M Security Extensions (Armv8-M)	Generates the device partition file. The name of the generated file is composed of partition_ and the value of the device <name> (for example, partition_CMSDK_ARMv8MBL.h). Refer to /device element. The content of the file uses Configuration Wizard annotations and is derived from the SAU-specific information of the /device/cpu/sauRegionsConfig element and the interrupts specified in the /device/peripherals element.
rem --fields=enum	Bit-field Enums	Generates enum lists for each field value description contained in the CMSIS-SVD input file. Must be used in combination with --generate=header.
rem --fields=macro	Bit-field Macros	Generates position and mask C-Macros for each field description contained in the CMSIS-SVD input file. Must be used in combination with --generate=header.
rem --fields=struct	Bit-field Structs	Generates bit fields for each field description contained in the CMSIS-SVD input file. Must be used in combination with --generate=header.
rem --fields=struct-ansic	ANSI Bit-field Structs	Generates MISRA-compliant structures for each bitfield. The generated code is not CMSIS-compliant! Must be used in combination with --generate=header.
rem --strict	Strict error checking	RECOMMENDED! Applies strict error checking and generates a lot more messages.

