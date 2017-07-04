# Ghost Bird File System Reader #

A tool for writing data to virtual image inited by Ghost Bird File System.



## Command ##

Here are some command:

- -i: init this image with MBR mode
- -cp: create partition
- -dp: delete partition
- -df: delete file
- -f: format a partition with GBFS
- -ws: write sector
- -rs: read sector
- -wf: write file
- -rf: read file
- -si: show image information
- -sp: show partition information
- -sf: show directory information
- -sh: hex viewer
- -v: version information

### -ws ###
    gfr -ws src_file dst_file LBA

the location can be filled with value as follow:

- mbr:
- pt:
- LBA: sector number, start at 1