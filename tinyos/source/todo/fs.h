/* ========================================================= */
/* File Name : source/kernel/fs.h                            */
/* File Date : 12 February 2015                              */
/* Author(s) : Michael Collins                               */
/* --------------------------------------------------------- */
/* file system interface.                                    */
/* --------------------------------------------------------- */
/* The disk descriptor is 64 bytes in length.  There are two */
/* copies, the first is located at the end of sector 0,  and */
/* the second appears at the end of sector 1.                */
/* --------------------------------------------------------- */
/* Nodes are listed together in a large multi sector array.  */
/* Nodes are used to avoid unneccesary disk writes,  but are */
/* a source of fragmentation.                                */
/* --------------------------------------------------------- */
/* Directories are special files containing an array of file */
/* entries.  The first entry is a secondary copy of the file */
/* entry describing this directory itself.  The second entry */
/* is a copy of the parent directory.                        */
/* --------------------------------------------------------- */
/* Links are special files, which link to another (original) */
/* file. Links can be renamed and deleted by their owner but */
/* all other access is governed by the file it references.   */
/* ========================================================= */

#ifndef FS_H
#define FS_H



#pragma pack(push, 1)
typedef struct disk {                   // Disk Volume Descriptor
    char  VolumeIdentifier[16];       // Name of the disk
    char  ManufacturerIdentifier[16]; // Name of the manufacturer

    int   MediaType;                    // 3.5Inch 1.44MB Floppy Disk
	int   BytesPerSector;               // Size in bytes of a sector
	int   SectorsPerCylinder;           // Number of sectors per cylinder
	int   CylindersPerTrack;            // Number of cylinders per track
	int   TracksPerDisk;                // Number of tracks on the disk
	int   FileNodesSectorNo;            // Sector No of the File Nodes
	int   NumFileNodeEntries;           // Total number of entries in the File Node array

	// Local copy of the root directory file entry
	int  RootFileSize;                  // The file size in bytes
	int  RootFileNode;                  // Identifies the file node
	int  FileFlags;                     // Governs file access rights, and file type
    time RootCreationTime;              // Time of creation
    time RootModifiedTime;              // Time of last modification
    time RootAccessedTime;              // Time of last access
	int  RootUserID;                    // The UserID of the files owner
	int  RootGroupID;                   // The GroupID of which the owner is a member
	char RootSizeEntry;                 // The size in bytes of this file entry
	char RootNameLength;                // Stores the length of the buffer in bytes used to record the file name
	char RootName[14];                // Stores the name of the file. the length of this field must contribute
                                        // to the total size of this file entry such that the total size of this
                                        // file entry is evenly divisable by 16. Buffer is padded with zeros.
	short FileSystemVersion;            // Version of the File System
	short Padding;                      // Used for boot signiture in second copy
} disk;                                 // Disk Volume Descriptor
#pragma pack(pop)


// format: YYYYYYYYYYYYMMMMDDDDDhhhhhmmmmmm
//         most sig <-----------> least sig
//
// legend: Y = Year (0AD to 4095AD)
//         M = Month of year (0 to 11)
//         D = Day of month (0 to 31)
//         h = Hour of day (0 to 23)
//         m = Minute of hour (0 to 59)
typedef unsigned __int32 time;


#pragma pack(push, 1)
typedef struct fs {         // File System Entry
	int  FileSize;          // The file size in bytes
	int  FileNode;          // Identifies the file node
	int  FileFlags;         // Governs file access rights, and file type
    time CreationTime;      // Time of creation
    time ModifiedTime;      // Time of last modification
    time AccessedTime;      // Time of last access
	int  UserID;            // The UserID of the files owner
	int  GroupID;           // The GroupID of which the owner is a member
	char SizeEntry;         // The size in bytes of this file entry
	char NameLength;        // Stores the length of the buffer in bytes used to record the file name
	char Name[14];        // Stores the name of the file. the length of this field must contribute
                            // to the total size of this file entry such that the total size of this
                            // file entry is evenly divisable by 16. Buffer is padded with zeros.
} fs;                       // File System Entry
#pragma pack(pop)



#pragma pack(push, 1)
typedef struct fnhdr {  //
	int SectorNoBits;   //
	int NodeEntryBits;  //
	int MaximumNodes;   //
	int TotalNodesUsed; //
} fnhdr;                //
#pragma pack(pop)



#define FS_OWNERREAD    (0x00000001)
#define FS_OWNERWRITE   (0x00000002)
#define FS_OWNEREXECUTE (0x00000004)
#define FS_OWNERDELETE  (0x00000008)

#define FS_GROUPREAD    (0x00000010)
#define FS_GROUPWRITE   (0x00000020)
#define FS_GROUPEXECUTE (0x00000040)
#define FS_GROUPDELETE  (0x00000080)

#define FS_WORLDREAD    (0x00000100)
#define FS_WORLDWRITE   (0x00000200)
#define FS_WORLDEXECUTE (0x00000400)
#define FS_WORLDDELETE  (0x00000800)

#define FS_DIRECTORY    (0x00001000)
#define FS_LINK         (0x00002000)
#define FS_PROGRAM      (0x00003000)
#define FS_TEXTDATA     (0x00004000)
#define FS_BINARYDATA   (0x00005000)



typedef struct node { // File Node
	int SectorNo;     // Identifies the sector containig this portion of a file
	int NextNode;     // Identifies the next node or zero if this is the end of the file
} node;               // File Node



typedef struct file {
	char  sig[4];  // Verify 'file'
	int   FileType;  // Identifies the file type (used for device IO)
	file *Directory; // Local copy of the parent file entry
	file *File;      // Local copy of the file entry
	int   Pointer;   // Pointer within the file
	int   Access;    // Access requested
	void *Interface; // Points to an array that interfaces to the device (disk, serial, keyboard, etc.)
} file;



file *fopen(char *Path, char *Access);
void  fclose(file *File);
int   fdelete(char *Path);
int   fcopy(char *Dest, char *Source);
int   fmove(char *Dest, char *Source);
int   fsize(file *File);
int   ftell(file *File);
int   fseek(file *File, int Offset, int Origin);
int   fread(file *File, char *Data, int Size);
int   fwrite(file *File, char *Data, int Size);



#define SEEK_SET (1)
#define SEEK_CUR (2)
#define SEEK_END (3)



#endif // FS_H


// disk geometry
//    |<-- cylinder --------------------------------------->|
// h0: 01 13 0B 1D 03 15 0D 1F 05 17 0F 21 07 19 11 23 09 1B
// h1: 0A 1C 02 14 0C 1E 04 16 0E 20 06 18 10 22 08 1A 12 24
//
// n: 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20 21 22 23 24
// h: 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01 00 01
// s: 01 03 05 07 09 0B 0D 0F 11 01 03 05 07 09 0B 0D 0F 11 02 04 06 08 0A 0C 0E 10 12 02 04 06 08 0A 0C 0E 10 12
//
// c = n / (media.TracksPerDisk * media.SectorsPerCylinder);
// n = n % (media.TracksPerDisk * media.SectorsPerCylinder) - 1;
// h = n & 1;
// s = lookup[n - 1];
// char lookup[0x24] = {
//     0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11,
//     0x01, 0x03, 0x05, 0x07, 0x09, 0x0B, 0x0D, 0x0F, 0x11,
//     0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
//     0x02, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12,
// };
//
