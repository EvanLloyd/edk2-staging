/** @file
*
*  Copyright (c) 2016 - 2017, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

#include <IndustryStandard/Acpi.h>
#include <Library/UefiLib.h>
#include "AcpiParser.h"
#include "AcpiView.h"

// Local Variables
STATIC CONST UINT8* MadtInterruptControllerType;
STATIC CONST UINT8* MadtInterruptControllerLength;


/** An ACPI_PARSER array describing the GICC Interrupt
    Controller Structure.

**/
STATIC CONST ACPI_PARSER GicCParser[] = {
  {L"Type", 1, 0, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Length", 1, 1, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 2, 2, L"0x%x", NULL, NULL, NULL, NULL},

  {L"CPU Interface Number", 4, 4, L"0x%x", NULL, NULL, NULL, NULL},
  {L"ACPI Processor UID", 4, 8, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Flags", 4, 12, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Parking Protocol Version", 4, 16, L"0x%x", NULL, NULL, NULL, NULL},


  {L"Performance Interrupt GSIV", 4, 20, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Parked Address", 8, 24, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"Physical Base Address", 8, 32, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"GICV", 8, 40, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"GICH", 8, 48, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"VGIC Maintenance interrupt", 4, 56, L"0x%x", NULL, NULL, NULL, NULL},
  {L"GICR Base Address", 8, 60, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"MPIDR", 8, 68, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"Processor Power Efficiency Class", 1, 76, L"0x%x", NULL, NULL, NULL,
   NULL},
  {L"Reserved", 3, 77, L"%x %x %x", Dump3Chars, NULL, NULL, NULL}
};

/** An ACPI_PARSER array describing the GICD Interrupt
    Controller Structure.

**/
STATIC CONST ACPI_PARSER GicDParser[] = {
  {L"Type", 1, 0, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Length", 1, 1, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 2, 2, L"0x%x", NULL, NULL, NULL, NULL},

  {L"GIC ID", 4, 4, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Physical Base Address", 8, 8, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"System Vector Base", 4, 16, L"0x%x", NULL, NULL, NULL, NULL},
  {L"GIC Version", 1, 20, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 3, 21, L"%x %x %x", Dump3Chars, NULL, NULL, NULL}
};


/** An ACPI_PARSER array describing the MSI Frame Interrupt
    Controller Structure.

**/
STATIC CONST ACPI_PARSER GicMSIFrameParser[] = {
  {L"Type", 1, 0, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Length", 1, 1, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 2, 2, L"0x%x", NULL, NULL, NULL, NULL},

  {L"MSI Frame ID", 4, 4, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Physical Base Address", 8, 8, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"Flags", 4, 16, L"0x%x", NULL, NULL, NULL, NULL},

  {L"SPI Count", 2, 20, L"%d", NULL, NULL, NULL, NULL},
  {L"SPI Base", 2, 22, L"0x%x", NULL, NULL, NULL, NULL}
};

/** An ACPI_PARSER array describing the GICR Interrupt
    Controller Structure.

**/
STATIC CONST ACPI_PARSER GicRParser[] = {
  {L"Type", 1, 0, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Length", 1, 1, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 2, 2, L"0x%x", NULL, NULL, NULL, NULL},

  {L"Discovery Range Base Address", 8, 4, L"0x%lx", NULL, NULL, NULL,
   NULL},
  {L"Discovery Range Length", 4, 12, L"0x%x", NULL, NULL, NULL, NULL}
};

/** An ACPI_PARSER array describing the GIC ITS Interrupt
    Controller Structure.

**/
STATIC CONST ACPI_PARSER GicITSParser[] = {
  {L"Type", 1, 0, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Length", 1, 1, L"%d", NULL, NULL, NULL, NULL},
  {L"Reserved", 2, 2, L"0x%x", NULL, NULL, NULL, NULL},

  {L"GIC ITS ID", 4, 4, L"0x%x", NULL, NULL, NULL, NULL},
  {L"Physical Base Address", 8, 8, L"0x%lx", NULL, NULL, NULL, NULL},
  {L"Reserved", 4, 20, L"0x%x", NULL, NULL, NULL, NULL}
};


/** An ACPI_PARSER array describing the ACPI MADT Table.

**/
STATIC CONST ACPI_PARSER MadtParser[] = {
  PARSE_ACPI_HEADER (NULL, NULL, NULL),
  {L"Local Interrupt Controller Address", 4, 36, L"0x%x", NULL, NULL, NULL,
   NULL},
  {L"Flags", 4, 40, L"0x%x", NULL, NULL, NULL, NULL}
};


/** An ACPI_PARSER array describing the MADT Interrupt
    Controller Structure Header Structure.
**/
STATIC CONST ACPI_PARSER MadtInterruptControllerHeaderParser[] = {
  {NULL, 1, 0, NULL, NULL, (VOID**)&MadtInterruptControllerType, NULL, NULL},
  {L"Length", 1, 1, NULL, NULL, (VOID**)&MadtInterruptControllerLength, NULL,
   NULL},
  {L"Reserved", 2, 2, NULL, NULL, NULL, NULL, NULL}
};


/** This function parses the ACPI MADT table.
  This function parses the MADT table and optionally traces the ACPI
  table fields.

  This function currently parses the following Interrupt Controller
  Structures:
    - GICC
    - GICD
    - GIC MSI Frame
    - GICR
    - GIC ITS

  This function also performs validation of the ACPI table fields.

  @params [in] Trace              If TRUE, trace the ACPI fields.
  @params [in] Ptr                Pointer to the start of the buffer.
  @params [in] AcpiTableLength    Length of the ACPI table.
  @params [in] AcpiTableRevision  Revision of the ACPI table.
**/
VOID
ParseAcpiMadt (
  IN BOOLEAN Trace,
  IN UINT8*  Ptr,
  IN UINT32  AcpiTableLength,
  IN UINT8   AcpiTableRevision
  )
{
  UINT32 Offset;
  UINT8* InterruptContollerPtr;

  if (!Trace) {
    return;
  }

  Offset = ParseAcpi (
             TRUE,
             0,
             "MADT",
             Ptr,
             AcpiTableLength,
             PARSER_PARAMS (MadtParser)
             );
  InterruptContollerPtr = Ptr + Offset;

  while (Offset < AcpiTableLength) {
    // Parse Interrupt Controller Structure to obtain Length.
    ParseAcpi (
      FALSE,
      0,
      NULL,
      InterruptContollerPtr,
      2,  //  Length is 1 byte at offset 1
      PARSER_PARAMS (MadtInterruptControllerHeaderParser)
      );

    if (((Offset + (*MadtInterruptControllerLength)) > AcpiTableLength) ||
        (*MadtInterruptControllerLength < 4)) {
      IncrementErrorCount ();
      Print (
         L"ERROR: Invalid Interrupt Controller Length,"
          " Type = %d, Length = %d",
         *MadtInterruptControllerType,
         *MadtInterruptControllerLength
         );
      break;
    }

    switch (*MadtInterruptControllerType) {
      case EFI_ACPI_6_1_GIC: {
        ParseAcpi (
          TRUE,
          2,
          "GICC",
          InterruptContollerPtr,
          *MadtInterruptControllerLength,
          PARSER_PARAMS (GicCParser)
          );
        break;
      }

      case EFI_ACPI_6_1_GICD: {
        ParseAcpi (
          TRUE,
          2,
          "GICD",
          InterruptContollerPtr,
          *MadtInterruptControllerLength,
          PARSER_PARAMS (GicDParser)
          );
        break;
      }

      case EFI_ACPI_6_1_GIC_MSI_FRAME: {
        ParseAcpi (
          TRUE,
          2,
          "GIC MSI Frame",
          InterruptContollerPtr,
          *MadtInterruptControllerLength,
          PARSER_PARAMS (GicMSIFrameParser)
          );
        break;
      }

      case EFI_ACPI_6_1_GICR: {
        ParseAcpi (
          TRUE,
          2,
          "GICR",
          InterruptContollerPtr,
          *MadtInterruptControllerLength,
          PARSER_PARAMS (GicRParser)
          );
        break;
      }

      case EFI_ACPI_6_1_GIC_ITS: {
        ParseAcpi (
          TRUE,
          2,
          "GIC ITS",
          InterruptContollerPtr,
          *MadtInterruptControllerLength,
          PARSER_PARAMS (GicITSParser)
          );
        break;
      }

      default: {
        IncrementErrorCount ();
        Print (
          L"ERROR: Unknown Interrupt Controller Structure,"
            " Type = %d, Length = %d",
          *MadtInterruptControllerType,
          *MadtInterruptControllerLength
          );
      }
    } // switch
    InterruptContollerPtr += *MadtInterruptControllerLength;
    Offset += *MadtInterruptControllerLength;
  } // while

}
