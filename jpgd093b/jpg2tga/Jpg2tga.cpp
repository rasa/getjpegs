//------------------------------------------------------------------------------
// jpg2tga.cpp
// JPEG to TGA file conversion example program.
// Last updated: Nov. 16, 2000 
// Copyright (C) 1994-2000 Rich Geldreich
// richgel@voicenet.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//------------------------------------------------------------------------------
#include "jpegdecoder.h"
#include "writetga.h"
#include <iostream>
#include <fstream>
//------------------------------------------------------------------------------
// Uncomment to disable output file writing (for VTune benchmarking)
//#define NO_OUTPUT
//------------------------------------------------------------------------------
const bool use_mmx = false;
//------------------------------------------------------------------------------
// For debugging: purposely clear MMX state, to break any 'bad' code that depends
// on the regs being preserved across calls.
static void empty_mmx(void)
{
#ifdef _DEBUG
  _asm
  {
    pxor mm0, mm0
    pxor mm1, mm1
    pxor mm2, mm2
    pxor mm3, mm3
    pxor mm4, mm4
    pxor mm5, mm5
    pxor mm6, mm6
    pxor mm7, mm7
    emms
  }
#endif
}
//------------------------------------------------------------------------------
int main(int arg_c, char *arg_v[])
{
  printf("JPEG to TGA file conversion example. Compiled %s %s\n", __DATE__, __TIME__);
  printf("Copyright (C) 1994-2000 Rich Geldreich\n");
  printf("MMX enabled: %i\n", use_mmx);

  if (arg_c != 4)
  {
    printf("Usage: jpg2tga <source_file> <dest_file> <xml-file>\n");
    printf("Outputs greyscale and truecolor 24-bit TGA files.\n");
    return (EXIT_FAILURE);
  }

  const char *Psrc_filename = arg_v[1];
  const char *Pdst_filename = arg_v[2];
  const char *Pxml_filename = arg_v[3];

  printf("Source file:      \"%s\"\n", Psrc_filename);
  printf("Destination file: \"%s\"\n", Pdst_filename);
  printf("xml file: \"%s\"\n", Pxml_filename);

  std::ofstream xmlStream(Pxml_filename);
  

  jpeg_decoder_file_stream input_file_stream;

  if (input_file_stream.open(Psrc_filename))
  {
    printf("Error: Unable to open file \"%s\" for reading!\n", Psrc_filename);
    return (EXIT_FAILURE);
  }

  xmlStream << "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n";
  xmlStream << "<jpegml>\n";
  jpeg_decoder jpegDecoder(&input_file_stream, xmlStream, use_mmx);

  if (jpegDecoder.get_error_code() != 0)
  {
    printf("Error: Decoder failed! Error status: %i\n", jpegDecoder.get_error_code());
    return (EXIT_FAILURE);
  }

  tga_writer tgaWriter;

  const bool status = tgaWriter.open(Pdst_filename,
    jpegDecoder.get_width(),
    jpegDecoder.get_height(),
    (jpegDecoder.get_num_components() == 1) ? TGA_IMAGE_TYPE_GREY : TGA_IMAGE_TYPE_BGR);

  if (status)
  {
    printf("Error: Unable to open file \"%s\" for writing!\n", Pdst_filename);
    return (EXIT_FAILURE);
  }

  printf("Width: %i\n", jpegDecoder.get_width());
  printf("Height: %i\n", jpegDecoder.get_height());
  printf("Components: %i\n", jpegDecoder.get_num_components());

  // for debugging only!
  if (use_mmx)
    empty_mmx();

  if (jpegDecoder.begin())
  {
    printf("Error: Decoder failed! Error status: %i\n", jpegDecoder.get_error_code());
    remove(Pdst_filename);
    return (EXIT_FAILURE);
  }

  uchar *Pbuf = NULL;
  if (jpegDecoder.get_num_components() == 3)
  {
    Pbuf = (uchar *)malloc(jpegDecoder.get_width() * 3);
    if (!Pbuf)
    {
      printf("Error: Out of memory!\n");
      remove(Pdst_filename);
      return (EXIT_FAILURE);
    }
  }

  int lines_decoded = 0;

  for ( ; ; )
  {
    void *Pscan_line_ofs;
    uint scan_line_len;

    if (jpegDecoder.decode(&Pscan_line_ofs, &scan_line_len))
      break;

    // for debugging only!
    if (use_mmx)
      empty_mmx();

    lines_decoded++;

    bool status;

    if (jpegDecoder.get_num_components() == 3)
    {
      uchar *pSourceBuffer = (uchar *)Pscan_line_ofs;
      uchar *pDestinationBuffer = Pbuf;
      const int sourceBytesPerPixel = jpegDecoder.get_bytes_per_pixel();

      for (int x = jpegDecoder.get_width(); x > 0; x--, pSourceBuffer += sourceBytesPerPixel, pDestinationBuffer += 3)
      {
        pDestinationBuffer[0] = pSourceBuffer[2];
        pDestinationBuffer[1] = pSourceBuffer[1];
        pDestinationBuffer[2] = pSourceBuffer[0];
      }

      status = tgaWriter.write_line(Pbuf);
    }
    else
    {
      status = tgaWriter.write_line(Pscan_line_ofs);
    }

    if (status)
    {
      printf("Error: Unable to write to file \"%s\"!\n", Pdst_filename);

      free(Pbuf);
      remove(Pdst_filename);

      return (EXIT_FAILURE);
    }
  }

  free(Pbuf);

  if (tgaWriter.close())
  {
    printf("Error: Unable to write to file \"%s\"!\n", Pdst_filename);

    remove(Pdst_filename);

    return (EXIT_FAILURE);
  }


  if (jpegDecoder.get_error_code())
  {
    printf("Error: Decoder failed! Error status: %i\n", jpegDecoder.get_error_code());

    return (EXIT_FAILURE);
  }

  printf("Lines decoded: %i\n", lines_decoded);
  printf("Input file size:  %i\n", input_file_stream.get_size());
  printf("Input bytes actually read: %i\n", jpegDecoder.get_total_bytes_read());

  xmlStream << "</jpegml>\n";
  xmlStream.close();
  return (EXIT_SUCCESS);
}
//------------------------------------------------------------------------------

