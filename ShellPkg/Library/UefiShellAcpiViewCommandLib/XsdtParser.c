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

#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include "AcpiParser.h"
#include "AcpiView.h"
#include "AcpiTableParser.h"

// Local variables
STATIC CONST UINT32* TableSignature;
STATIC CONST UINT32* XsdtLength;


/** An ACPI_PARSER array describing the ACPI XSDT table.

**/
STATIC CONST ACPI_PARSER XsdtParser[] = {
  PARSE_ACPI_HEADER (&TableSignature, &XsdtLength, NULL)
};

/** This function parses the ACPI XSDT table
  and optionally traces the ACPI table fields.

  This function also performs validation of the XSDT table.

  @params [in] Trace              If TRUE, trace the ACPI fields.
  @params [in] Ptr                Pointer to the start of the buffer.
  @params [in] AcpiTableLength    Length of the ACPI table.
  @params [in] AcpiTableRevision  Revision of the ACPI table.
**/
VOID
ParseAcpiXsdt (
  IN BOOLEAN Trace,
  IN UINT8*  Ptr,
  IN UINT32  AcpiTableLength,
  IN UINT8   AcpiTableRevision
  )
{
  UINT32        Offset;
  UINT32        TableOffset;
  UINT64*       TablePointer;
  UINTN         EntryIndex;
  CHAR16        Buffer[32];

  // Parse the ACPI header to get the length
  ParseAcpi (
    FALSE,
    0,
    "XSDT",
    Ptr,
    ACPI_DESCRIPTION_HEADER_LENGTH,
    PARSER_PARAMS (XsdtParser)
    );

  Offset = ParseAcpi (
             Trace,
             0,
             "XSDT",
             Ptr,
             *XsdtLength,
             PARSER_PARAMS (XsdtParser)
             );

  TableOffset = Offset;

  if (Trace) {
    EntryIndex =0;
    TablePointer = (UINT64*)(Ptr + TableOffset);
    while (Offset < (*XsdtLength)) {
      CONST UINT32* Signature;
      CONST UINT32* Length;
      CONST UINT8*  Revision;

      if (NULL != (UINT64*)(UINTN)(*TablePointer)) {
        UINT8*      Ptr;

        ParseAcpiHeader (
          (UINT8*)(UINTN)(*TablePointer),
          &Signature,
          &Length,
          &Revision
          );

        Ptr = (UINT8*)Signature;

        UnicodeSPrint (
          Buffer,
          sizeof (Buffer),
          L"Entry[%d] - %c%c%c%c",
          EntryIndex++,
          Ptr[0],
          Ptr[1],
          Ptr[2],
          Ptr[3]
          );
      } else {
        UnicodeSPrint (
          Buffer,
          sizeof (Buffer),
          L"Entry[%d]",
          EntryIndex++
          );
      }

      PrintFieldName (2, Buffer);
      Print (L"0x%lx\n", *TablePointer);

      // Validate the table pointers are not NULL
      if (NULL == (UINT64*)(UINTN)(*TablePointer)) {
        IncrementErrorCount ();
        Print (
          L"ERROR: Invalid table entry at 0x%lx, table address is 0x%lx\n",
          TablePointer,
          *TablePointer
          );
      }
      Offset += sizeof (UINT64);
      TablePointer++;
    } // while
  }

  // Process the tables
  Offset = TableOffset;
  TablePointer = (UINT64*)(Ptr + TableOffset);
  while (Offset < (*XsdtLength)) {
    if (NULL != (UINT64*)(UINTN)(*TablePointer)) {
      ProcessAcpiTable ((UINT8*)(UINTN)(*TablePointer));
    }
    Offset += sizeof (UINT64);
    TablePointer++;
  } // while
}

