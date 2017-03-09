################################################################################
# ReadMe.MD for acpiview staging branch..
#  Contents:
#    1. Introduction
#    2. Branch Owner
#    3. Feature Summary
#    4. Timeline
#    5. Related Modules
#    6. Related Links
#    7. Misc
#
###############################################################################

1. Introduction
   This acpiview staging branch is requested by Evan Lloyd <evan.lloyd@arm.com>
   following suggestions from Jiewen Yao <jiewen.yao@intel.com> in response
   to a patch submitted to the edk2-devel mailing list.
   The aim is to allow collaborative development of acpiview for all platforms.

2. Branch Owners
   * Evan Lloyd <evan.lloyd@arm.com>
   * Jiewen Yao <jiewen.yao@intel.com>

3. Feature Summary
   Acpiview allows examination of ACPI table contents from the UEFI Shell.
   This can help with investigations, especially at that stage where the tables
   are not enabling an OS to boot.  The program is not exhaustive, and only
   encapsulates detailed knowledge of a limited number of table types.
   There is no immediate intent to provide disassembly of AML tables.

   Default behaviour is to display the content of all tables installed.
   'Known' table types will be parsed and displayed with descriptions and
   field values.  Where appropriate a degree of consistency checking is
   done and errors may be reported in the output.
   Other table types will be displayed as an array of Hexadecimal bytes.

   To facilitate debugging, the -t and -b options can be used to generate a
   binary file image of a table that can be copied elsewhere for
   investigation using tools such as those provided by acpica.org.  This is
   especially relevant for AML type tables like DSDT and SSDT.

4. Timeline
   N/A

5. Related Modules
   ShellPkg

6. Related Links
   N/A

7. Misc
   N/A

