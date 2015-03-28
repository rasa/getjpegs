// Copyright (c) 2005-2015 Ross Smith II. See Mit LICENSE in /LICENSE

// Scan the raw sectors for lost jpeg files and save as much as is recoverable

/*
To do:

* Add getopt functionality:

  -d | --drive 0-7  (physical drives) (default is 0)

  -d | --drive a-z (logical drives)

  -s | --start n (starting sector, default is 0)

  -e | --end n (ending sector, default is last sector of drive)

  -m | --max_size (maximum jpeg file to save, default is 4,000,000 bytes)

  -o | --output (directory to save jpegs, default is current directory)

  -l | --log filename (default is ./getjpegs.log)

  -v | --verbose (more verbosity, the default)

  -q | --quiet (less verbosity)

  -V | --version (display version information and quit)

fix elapsed time

Good jpegs to be named:
dnnnnnnn.jpg
|------- sequential number starting a one
+------- drive number (or letter)

Invalid jpegs to be named:

badjpegs/dnnnnnnn.jpg
         |------- sequential number starting a one
         +------- drive number (or letter)

Don't overwrite files, use the next available number

  If the user doesn't specify on the command line,
  ask user what drive by displaying a menu and wait for input.

*/

// Portions of this code originally came from diskid32 <http://www.winsim.com/diskid32/diskid32.html>

// Uses the "Small JPEG Decoder Library" <http://www.users.voicenet.com/~richgel/>

// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/devio/base/calling_deviceiocontrol.asp
// has cleaner code for getting the size of a drive, but only works for NT/2000/XP.

//  for displaying the details of hard drives in a command window

//  06/11/2000  Lynn McGuire  written with many contributions from others,
//                            IDE drives only under Windows NT/2K and 9X,
//                            maybe SCSI drives later
//  11/20/2003  Lynn McGuire  added ReadPhysicalDriveInNTWithZeroRights

#define PRINTING_TO_CONSOLE_ALLOWED

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
//#include <Winbase.h>
#include <direct.h>

#include "jpegdecoder.h"

#if _MSC_VER >= 1500
#include <winioctl.h>
#else
#undef _WINIOCTL_
#endif

#include "version.h"

#define APPNAME			VER_INTERNAL_NAME
#define APPVERSION		VER_STRING2
#define APPCOPYRIGHT	VER_LEGAL_COPYRIGHT

   //  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(1)

//#define MIN_FILE_SIZE (300000)
#define MAX_FILE_SIZE (3750000)

#define BYTES_PER_SECTOR (512)

#define OUTPUT_BUFFER_SIZE (MAX_FILE_SIZE + BYTES_PER_SECTOR + 1)

#define  IDENTIFY_BUFFER_SIZE  512

   //  IOCTL commands
#define  DFP_GET_VERSION          0x00074080
#define  DFP_SEND_DRIVE_COMMAND   0x0007c084
#define  DFP_RECEIVE_DRIVE_DATA   0x0007c088

   //  GETVERSIONOUTPARAMS contains the data returned from the
   //  Get Driver Version function.
typedef struct _GETVERSIONOUTPARAMS {
   BYTE bVersion;      // Binary driver version.
   BYTE bRevision;     // Binary driver revision.
   BYTE bReserved;     // Not used.
   BYTE bIDEDeviceMap; // Bit map of IDE devices.
   DWORD fCapabilities; // Bit mask of driver capabilities.
   DWORD dwReserved[4]; // For future use.
} GETVERSIONOUTPARAMS, *PGETVERSIONOUTPARAMS, *LPGETVERSIONOUTPARAMS;

   //  IDE registers
#ifndef _WINIOCTL_
typedef struct _IDEREGS {
   BYTE bFeaturesReg;       // Used for specifying SMART "commands".
   BYTE bSectorCountReg;    // IDE sector count register
   BYTE bSectorNumberReg;   // IDE sector number register
   BYTE bCylLowReg;         // IDE low order cylinder value
   BYTE bCylHighReg;        // IDE high order cylinder value
   BYTE bDriveHeadReg;      // IDE drive/head register
   BYTE bCommandReg;        // Actual IDE command.
   BYTE bReserved;          // reserved for future use.  Must be zero.
} IDEREGS, *PIDEREGS, *LPIDEREGS;

   //  SENDCMDINPARAMS contains the input parameters for the
   //  Send Command to Drive function.
typedef struct _SENDCMDINPARAMS {
   DWORD     cBufferSize;   //  Buffer size in bytes
   IDEREGS   irDriveRegs;   //  Structure with drive register values.
   BYTE bDriveNumber;       //  Physical drive number to send
                            //  command to (0,1,2,3).
   BYTE bReserved[3];       //  Reserved for future expansion.
   DWORD     dwReserved[4]; //  For future use.
   BYTE      bBuffer[1];    //  Input buffer.
} SENDCMDINPARAMS, *PSENDCMDINPARAMS, *LPSENDCMDINPARAMS;
#endif

   //  Valid values for the bCommandReg member of IDEREGS.
#define  IDE_ATAPI_IDENTIFY  0xA1  //  Returns ID sector for ATAPI.
#define  IDE_ATA_IDENTIFY    0xEC  //  Returns ID sector for ATA.

#ifndef _WINIOCTL_
   // Status returned from driver
typedef struct _DRIVERSTATUS {
   BYTE  bDriverError;  //  Error code from driver, or 0 if no error.
   BYTE  bIDEStatus;    //  Contents of IDE Error register.
                        //  Only valid when bDriverError is SMART_IDE_ERROR.
   BYTE  bReserved[2];  //  Reserved for future expansion.
   DWORD  dwReserved[2];  //  Reserved for future expansion.
} DRIVERSTATUS, *PDRIVERSTATUS, *LPDRIVERSTATUS;

   // Structure returned by PhysicalDrive IOCTL for several commands
typedef struct _SENDCMDOUTPARAMS {
   DWORD         cBufferSize;   //  Size of bBuffer in bytes
   DRIVERSTATUS  DriverStatus;  //  Driver status structure.
   BYTE          bBuffer[1];    //  Buffer of arbitrary length in which to store the data read from the                                                       // drive.
} SENDCMDOUTPARAMS, *PSENDCMDOUTPARAMS, *LPSENDCMDOUTPARAMS;
#endif

   // Define global buffers.
BYTE IdOutCmd [sizeof (SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1];

char *ConvertToString (DWORD diskdata [256], int firstIndex, int lastIndex);
__int64 PrintIdeInfo (int drive, DWORD diskdata [256]);
BOOL DoIDENTIFY (HANDLE, PSENDCMDINPARAMS, PSENDCMDOUTPARAMS, BYTE, BYTE,
                 PDWORD);

   //  Max number of drives assuming primary/secondary, master/slave topology
#define  MAX_IDE_DRIVES  16

//  Required to ensure correct PhysicalDrive IOCTL structure setup
#pragma pack(4)

   // DoIDENTIFY
   // FUNCTION: Send an IDENTIFY command to the drive
   // bDriveNum = 0-3
   // bIDCmd = IDE_ATA_IDENTIFY or IDE_ATAPI_IDENTIFY
BOOL DoIDENTIFY (HANDLE hPhysicalDriveIOCTL, PSENDCMDINPARAMS pSCIP,
                 PSENDCMDOUTPARAMS pSCOP, BYTE bIDCmd, BYTE bDriveNum,
                 PDWORD lpcbBytesReturned) {
      // Set up data structures for IDENTIFY command.
   pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;
   pSCIP -> irDriveRegs.bFeaturesReg = 0;
   pSCIP -> irDriveRegs.bSectorCountReg = 1;
   pSCIP -> irDriveRegs.bSectorNumberReg = 1;
   pSCIP -> irDriveRegs.bCylLowReg = 0;
   pSCIP -> irDriveRegs.bCylHighReg = 0;

      // Compute the drive number.
   pSCIP -> irDriveRegs.bDriveHeadReg = 0xA0 | ((bDriveNum & 1) << 4);

      // The command can either be IDE identify or ATAPI identify.
   pSCIP -> irDriveRegs.bCommandReg = bIDCmd;
   pSCIP -> bDriveNumber = bDriveNum;
   pSCIP -> cBufferSize = IDENTIFY_BUFFER_SIZE;

   return ( DeviceIoControl (hPhysicalDriveIOCTL, DFP_RECEIVE_DRIVE_DATA,
               (LPVOID) pSCIP,
               sizeof(SENDCMDINPARAMS) - 1,
               (LPVOID) pSCOP,
               sizeof(SENDCMDOUTPARAMS) + IDENTIFY_BUFFER_SIZE - 1,
               lpcbBytesReturned, NULL) );
}

__int64 PrintIdeInfo (int drive, DWORD diskdata [256]) {
   __int64 sectors = 0;
   __int64 bytes = 0;

#ifdef PRINTING_TO_CONSOLE_ALLOWED

   //  calculate size based on 28 bit or 48 bit addressing
		//  48 bit addressing is reflected by bit 10 of word 83
	if (diskdata [83] & 0x400)
		sectors = diskdata [103] * 65536I64 * 65536I64 * 65536I64 +
					diskdata [102] * 65536I64 * 65536I64 +
					diskdata [101] * 65536I64 +
					diskdata [100];
	else
		sectors = diskdata [61] * 65536 + diskdata [60];
		//  there are 512 bytes in a sector
	bytes = sectors * 512;

	printf("%2d ", drive);
	printf ("%-20s ", ConvertToString (diskdata, 27, 46));
	printf("%3I64d ", bytes / 1024 / 1024 / 1024);
	printf("%6I64d ", bytes / 1024 / 1024);
	printf("%9I64d ", sectors);
	printf("%12I64d ", bytes);
	printf("\n");

#endif  // PRINTING_TO_CONSOLE_ALLOWED

	return sectors;
}

char *ConvertToString (DWORD diskdata [256], int firstIndex, int lastIndex) {
   static char string [1024];
   int index = 0;
   int position = 0;

      //  each integer has two characters stored in it backwards
   for (index = firstIndex; index <= lastIndex; index++) {
         //  get high byte for 1st character
      string [position] = (char) (diskdata [index] / 256);
      position++;

         //  get low byte for 2nd character
      string [position] = (char) (diskdata [index] % 256);
      position++;
   }

      //  end the string
   string [position] = '\0';

      //  cut off the trailing blanks
   for (index = position - 1; index > 0 && ' ' == string [index]; index--)
      string [index] = '\0';

   return string;
}

void FatalError(char *str) {
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL );
	fprintf(stderr, "\n%s: %s\n", str, lpMsgBuf);
	exit(1);
}

class jpeg_decoder_memory_stream : public jpeg_decoder_stream {
public:
  char *buffer_start;
  int buffer_len;
  int current_pos;
  bool eof_flag;
  bool error_flag;

  jpeg_decoder_memory_stream() {
	close();
	buffer_start = NULL;
	buffer_len = 0;
  }

  void close(void) {
	current_pos = 0;
    eof_flag = false;
    error_flag = false;
  }

  virtual ~jpeg_decoder_memory_stream() {
    close();
	buffer_start = NULL;
	buffer_len = 0;
  }

  bool open(char *_buffer_start, int _buffer_len) {
    close();
	buffer_start = _buffer_start;
	buffer_len = _buffer_len;
    return (true);
  }

  virtual int read(uchar *Pbuf, int max_bytes_to_read, bool *Peof_flag) {
    if (eof_flag) {
      *Peof_flag = true;
      return (0);
    }

    if (error_flag)
      return (-1);

	if (current_pos + max_bytes_to_read > buffer_len) {
		int bytes_read = buffer_len - current_pos;
		memcpy(Pbuf, buffer_start + current_pos, bytes_read);
		current_pos += bytes_read;
	    eof_flag = true;
        *Peof_flag = true;
		return (bytes_read);
	}

	memcpy(Pbuf, buffer_start + current_pos, max_bytes_to_read);
	current_pos += max_bytes_to_read;
	return max_bytes_to_read;
  }

  bool get_error_status(void) {
    return (error_flag);
  }

  bool reset(void) {
    if (error_flag)
      return (true);

	close();
    return (false);
  }

  int get_size(void) {
	return buffer_len;
  }
};
typedef jpeg_decoder_memory_stream *Pjpeg_decoder_memory_stream;

char *JpegStrError(int error_code) {
	switch (error_code) {
		case -200:
			return "Bad dht counts";
		case -201:
			return "Bad dht index";
		case -202:
			return "Bad dht marker";
		case -203:
			return "Bad dqt marker";
		case -204:
			return "Bad dqt table";
		case -205:
			return "Bad precision";
		case -206:
			return "Bad height";
		case -207:
			return "Bad width";
		case -208:
			return "Too many components";
		case -209:
			return "Bad sof length";
		case -210:
			return "Bad variable marker";
		case -211:
			return "Bad dri length";
		case -212:
			return "Bad sos length";
		case -213:
			return "Bad sos comp id";
		case -214:
			return "Extra bytes before marker";
		case -215:
			return "No arithmitic support";
		case -216:
			return "Unexpected marker";
		case -217:
			return "Not jpeg";
		case -218:
			return "Unsupported marker";
		case -219:
			return "Bad dqt length";
		case -221:
			return "Too many blocks";
		case -222:
			return "Undefined quant table";
		case -223:
			return "Undefined huff table";
		case -224:
			return "Not single scan";
		case -225:
			return "Unsupported colorspace";
		case -226:
			return "Unsupported samp factors";
		case -227:
			return "Decode error";
		case -228:
			return "Bad restart marker";
		case -229:
			return "Assertion error";
		case -230:
			return "Bad sos spectral";
		case -231:
			return "Bad sos successive";
		case -232:
			return "Stream read";
		case -233:
			return "Not enough memory";
		default:
			return "Unknown error";
	}
}

void JPegError(int error_code, char *prefix) {
	char *strerror = JpegStrError(error_code);
	fprintf(stderr, "\nJPEG Error: %s: %s (%d)\n", prefix, strerror, error_code);
}

bool write_jpeg_file(char *output_buffer, int bytes, int next_jpeg_no, char *prefix, int drive) {
	Pjpeg_decoder_memory_stream Pinput_stream = new jpeg_decoder_memory_stream();

	Pinput_stream->open(output_buffer, bytes);

	Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, false);

	int last_error_code = 0;

	while (true) {
		if (Pd->get_error_code() != 0) {
			JPegError(Pd->get_error_code(), "open");
			break;
		}

		if (Pd->begin()) {
			JPegError(Pd->get_error_code(), "begin");
			break;
		}

		void *Pscan_line_ofs;
		uint scan_line_len;

		int lines = 0;
		while (lines < 1000000) {
			if (Pd->decode(&Pscan_line_ofs, &scan_line_len)) {
				break;
			}

			++lines;
		}

		printf("\nDecoded %5d lines, buffer %7d read %7d (%7d)\n",
			lines,
			Pinput_stream->get_size(),
			Pinput_stream->current_pos,
			Pd->get_total_bytes_read()
		);

		bytes = Pd->get_total_bytes_read();

		if (Pd->get_error_code() != 0) {
			JPegError(Pd->get_error_code(), "decode");
			break;
		}

		break;
	}

	last_error_code = Pd->get_error_code() < 0 ? -Pd->get_error_code() : Pd->get_error_code();

	delete Pd;
	delete Pinput_stream;

	static char filename[256];
	sprintf(filename, "%d/%03d/%08d.jpg", drive, last_error_code, next_jpeg_no);
	printf("Writing %s: %7d bytes\n", filename, bytes);

	FILE *fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Can't open %s\n", filename);
		FatalError("fopen");
	}

	int rv = fwrite(output_buffer, sizeof(char), bytes, fp);

	if (rv != bytes) {
		fprintf(stderr, "Write error writing to %s\n", filename);
		FatalError("fwrite");
	}

	rv = fclose(fp);

	if (rv) {
		fprintf(stderr, "Write error writing to %s\n", filename);
		FatalError("fclose");
	}

	return true;
}

int main (int argc, char * argv []) {
	 // 0 = d: starting sector: 32772600
	// 1 = e:
	// on drive 0, the first jpegs start at sector 43419184
	// on drive 1, the first jpegs start at sector 73730150
/*
getjpegs 0 43419100 | tee getjpegs_0.log
getjpegs 1 73730100 | tee getjpegs_1.log

getjpegs 0 132125500 | tee getjpegs_0b.log
getjpegs 1 167488600 | tee getjpegs_1b.log

*/

	int selected_drive = 1;
	__int64 starting_sector =  73730150;

	if (argc > 1) {
			selected_drive = atoi(argv[1]);
	}

	if (argc > 2) {
			starting_sector = _atoi64(argv[2]);
	}

printf(" # Manufacturer          GB     MB   sectors        bytes\n");
printf("-- -------------------- --- ------ --------- ------------\n");

//32772600

   int done = FALSE;
   int drive = 0;
	__int64 sectors[MAX_IDE_DRIVES];

   for (drive = 0; drive < MAX_IDE_DRIVES; drive++) {
      HANDLE hPhysicalDriveIOCTL = 0;

         //  Try to get a handle to PhysicalDrive IOCTL, report failure
         //  and exit if can't.
      char driveName [256];

      sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

         //  Windows NT, Windows 2000, must have admin rights
      hPhysicalDriveIOCTL = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);
      // if (hPhysicalDriveIOCTL == INVALID_HANDLE_VALUE)
      //    printf ("Unable to open physical drive %d, error code: 0x%lX\n",
      //            drive, GetLastError ());

      if (hPhysicalDriveIOCTL != INVALID_HANDLE_VALUE)
      {
         GETVERSIONOUTPARAMS VersionParams;
         DWORD               cbBytesReturned = 0;

            // Get the version, etc of PhysicalDrive IOCTL
         memset ((void*) &VersionParams, 0, sizeof(VersionParams));

         if ( ! DeviceIoControl (hPhysicalDriveIOCTL, DFP_GET_VERSION,
                   NULL,
                   0,
                   &VersionParams,
                   sizeof(VersionParams),
                   &cbBytesReturned, NULL) )
         {
            // printf ("DFP_GET_VERSION failed for drive %d\n", i);
            // continue;
         }

            // If there is a IDE device at number "i" issue commands
            // to the device
         if (VersionParams.bIDEDeviceMap > 0)
         {
            BYTE             bIDCmd = 0;   // IDE or ATAPI IDENTIFY cmd
            SENDCMDINPARAMS  scip;
            //SENDCMDOUTPARAMS OutCmd;

			// Now, get the ID sector for all IDE devices in the system.
               // If the device is ATAPI use the IDE_ATAPI_IDENTIFY command,
               // otherwise use the IDE_ATA_IDENTIFY command
            bIDCmd = (VersionParams.bIDEDeviceMap >> drive & 0x10) ? \
                      IDE_ATAPI_IDENTIFY : IDE_ATA_IDENTIFY;

            memset (&scip, 0, sizeof(scip));
            memset (IdOutCmd, 0, sizeof(IdOutCmd));

            if ( DoIDENTIFY (hPhysicalDriveIOCTL,
                       &scip,
                       (PSENDCMDOUTPARAMS)&IdOutCmd,
                       (BYTE) bIDCmd,
                       (BYTE) drive,
                       &cbBytesReturned))
            {
               DWORD diskdata [256];
               int ijk = 0;
               USHORT *pIdSector = (USHORT *)
                             ((PSENDCMDOUTPARAMS) IdOutCmd) -> bBuffer;

               for (ijk = 0; ijk < 256; ijk++)
                  diskdata [ijk] = pIdSector [ijk];

               sectors[drive] = PrintIdeInfo (drive, diskdata);

               done = TRUE;
            }
	    }

         CloseHandle (hPhysicalDriveIOCTL);
      }
   }

	drive = selected_drive;

	char driveName [256];
      sprintf (driveName, "\\\\.\\PhysicalDrive%d", drive);

//      sprintf (driveName, "\\\\.\\F:", drive);

      HANDLE hnd = 0;

	  hnd = CreateFile (driveName,
                               GENERIC_READ | GENERIC_WRITE,
                               FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                               OPEN_EXISTING, 0, NULL);

      if (hnd == INVALID_HANDLE_VALUE) {
		FatalError("CreateFile");
	  }

	DWORD dwBytesRead = 0;

	static char sector_data[BYTES_PER_SECTOR];

	__int64 ending_sector = sectors[selected_drive];

	printf("Reading drive %d\n", drive);

	DWORD start = GetTickCount();

	if (starting_sector) {
		__int64 starting_byte = starting_sector * 512;
		LARGE_INTEGER li;

		li.QuadPart = starting_byte;

		DWORD dw = SetFilePointer (hnd, li.LowPart, &li.HighPart, FILE_BEGIN);

		if (dw != (DWORD) starting_byte) {
			FatalError("SetFilePointer seek error");
		}

		if (GetLastError() != NO_ERROR)
		{
			FatalError("SetFilePointer");
		}
	}

	static char dirbuff[256];
	char *dir = dirbuff;

	sprintf(dir, "%d", drive);
	int rv = _mkdir(dir);

	sprintf(dir, "%d\\%03d", drive, 0);
	rv = _mkdir(dir);

	for (int i = 200; i < 233; ++i) {
		sprintf(dir, "%d\\%03d", drive, i);
		rv = _mkdir(dir);
	}

	int in_jpeg = 0;

	static char output_buffer[OUTPUT_BUFFER_SIZE];
	char *output_pointer;
	int output_bytes;
	int next_jpeg_no = 102; // fixme
	int found_ffd9 = 0;
	int end_of_data = 0;

	for (__int64 sector = starting_sector; sector <= ending_sector; ++sector) {

		double pct = ((double)(int) (sector - starting_sector)) / ((double)(int)(ending_sector - starting_sector));

		DWORD now = GetTickCount();

		DWORD elapsed = now - start;

		double left = pct ? (double) elapsed / pct : 0;

		DWORD remaining = (DWORD) left;

		DWORD seconds = remaining / 1000;

		DWORD hours = seconds / 3600;
		seconds -= hours * 3600;

		DWORD minutes = seconds / 60;
		seconds -= minutes * 60;

		DWORD eseconds = elapsed / 1000;

		DWORD ehours = eseconds / 3600;
		eseconds -= ehours * 3600;

		DWORD eminutes = eseconds / 60;
		eseconds -= eminutes * 60;

		if (sector % 100 == 0) {
			printf("Sector %10I64d of %10I64d: %7.3f%% %d:%02d:%02d elapsed %d:%02d:%02d remaining\r", sector, ending_sector, pct * 100,
				ehours, eminutes, eseconds, hours, minutes, seconds);
		}

		BOOL rv = ReadFile(hnd, sector_data, BYTES_PER_SECTOR,
				  &dwBytesRead, NULL);

		if (!rv) {
			fprintf(stderr, "\nFailed to read sector %I64d\n", sector);
			FatalError("");
		}

		if (dwBytesRead != BYTES_PER_SECTOR) {
			fprintf(stderr, "\nRead error reading sector %I64d\n", sector);
			FatalError("");
		}

		static char *exif1 = "\xff\xd8\xff\xe1";

		static char *exif2 = "\x45\x78\x69\x66";

		int exif_header_found =
			(memcmp(sector_data    , exif1, sizeof(exif1)) == 0) &&
			(memcmp(sector_data + 6, exif2, sizeof(exif2)) == 0);

		char *prefix = "";

		if (in_jpeg) {
			if (exif_header_found) {
				if (!end_of_data) {
					end_of_data = output_bytes;
					prefix = "no_eod_";
				}

				write_jpeg_file(output_buffer, end_of_data, next_jpeg_no, prefix, drive);
				++next_jpeg_no;
				in_jpeg = 0;
				end_of_data = 0;
				output_bytes = 0;
			} else {
				int bytes = BYTES_PER_SECTOR;
				memcpy(output_pointer, sector_data, bytes);
				output_pointer += bytes;

				static char *ffd9 = "\xff\xd9";

				char *eod = strstr(sector_data, ffd9);

				if (eod != NULL) {
					int bytes_this_sector = eod - sector_data;
					end_of_data = output_bytes + bytes_this_sector + sizeof(ffd9);
				}

				output_bytes += bytes;

				if (output_bytes >= MAX_FILE_SIZE) {
					if (!end_of_data) {
						end_of_data = MAX_FILE_SIZE;
						prefix = "no_eod_";
					}

					write_jpeg_file(output_buffer, end_of_data, next_jpeg_no, prefix, drive);
					++next_jpeg_no;
					in_jpeg = 0;
					output_bytes = 0;
				}
			}
		}

		if (!exif_header_found) {
			continue;
		}

//		printf("\nFound EXIF signature at sector %10I64d\n", sector);

		in_jpeg = 1;

		memcpy(output_buffer, sector_data, BYTES_PER_SECTOR);
		output_pointer = output_buffer + BYTES_PER_SECTOR;
		output_bytes = BYTES_PER_SECTOR;
	}

	CloseHandle (hnd);

//	delete [] sector_data;

	return 0;
}
