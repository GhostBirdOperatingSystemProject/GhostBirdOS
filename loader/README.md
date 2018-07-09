Copyright 2018 GhostBird OS

Lab Explorer Loader

GhostBird OS is licensed under the BSD 2-Clause "Simplified" License.

��������������������ڴӴ������ṩ��������̽�����ں��������ڵĶ�����ܡ�

Description: The program is used to boot Lab Explorer from Hard Disk, including other relevant functions.


����Ҫ�󣺱���������������Intel x86 �������ܹ��ϣ�Ҫ������������80386�����ϼ����ڴ����ٲ�������4MB�������������ں�����ռ䣩��IDEͨ����Ӳ����������һ��FAT32�����������װ�������ļ�������п��ܣ��뾡���ܵ���������п������⻯����VT-x/AMD-V������Ӳ������Ҫ�󲢲�һ����һ�ɲ���ģ��ں����Ŀ�����һЩ���ƿ��ܻ�½���ſ�

Requirements: The loader runs on Intel x86 architectures(at least 80386), requiring a memory of greater than 4MB. An IDE hard disk with a FAT32 active partition(used to storage configurations) is also needed. Please enable virtualization(such as VT-x/AMD-V) when running in virtual machine. The requirements mentioned above is not always still, and some restrictions may be liberalized in the further development.


��ʾ���������������û�о����ۺϼ��飬ǿ�ҽ��鱾���������ʹ�������������ʹ�ñ���������


Notes: This program hasn't tested under complex circunstances. We insist that users should run the loader in the VMs.


�汾������loader�İ汾��Ϣ����/comfigure.h�ж����version��һ���ʽΪ��A.b type c��������AΪ���汾�ţ�bΪ�ΰ汾�ţ�typeΪ���ͣ�ͨ���г��棨General����������Building�����ڲ⣨Alpha���Լ����⣨Beta����

Version description: The version information is only defined as "version" in /comfigure.h, the normal format is "A.b type c".
A is the main version, b is the secondary version, type normally includes General/Building/Alpha/Beta.


�ļ������������������Դ�����ļ�����ͨ�������������͵��ļ���

File description: The source code folder is ordinarily filled with files with these types:

.C�ļ���C���Ա�д��Դ�����ļ� Source code written in C

.h�ļ���C���Ա�дʱԤ����ͷ�ļ� Header files when writting in C

.asm�ļ���������Ա�дʱ��Դ�����ļ� Source code written in Assembly Language

.O�ļ�������ʱ������Ŀ���ļ� Object Files when compiling

.bin�ļ����޸�ʽ�����Ʊ����ļ� Raw Binary Files


�����ṹ����һЩȫ���Ե��ļ�֮�⣬����Ŀ¼�д���̽�����ں�����������ɵ����󲿷֣�����������¼Դ���벿�֣�/mbr���ͱ�������Դ���벿�֣�/boot�����֡�

Structure in this directory: Apart from global files, this class of directory contains the two main parts of kernel loader: Master Boot Record (/mbr) and source code used in reserved sectors(/boot).


��ʾ������������ĸ����ļ����д��������Ա���Դ���롢�����ļ����������˼·��ʵ��ԭ�������������������⡣

Notes: Please pay attention to READMEs, mostly distributed in every class of directories, which describe source code, files, designs and realization principles in the same class.


��Ŀ¼�´��ڵ��ļ����ļ��У�

Folders and Files in this directory:

\boot��������������������Դ�����ļ��� Folder of the source code of loader program in reserved sectors

\mbr����������¼��Դ�����ļ��� Folder of the MBR source code 

\configure.h�������ļ�����ͷ�ļ�����ʽ������ Configuration Files. (Refered as a header file)

\Makefile���ṩȫ�ֵı��롢���������� It is just a makefile ;)

\README.md�����ļ�������������˵������������ You're reading.

\RGB card��RGB��ѡɫʱ�Ĳο��� RGB table for reference.


���˼·��ʵ��ԭ��

Design and Realization Principle

����������MBR����������������������ص��ڴ���ɱ�������������������豸���ڴ桢�������ĳ�ʼ���Լ�������صĳ�ʼ�����ṩ���ӻ��������棬�����ش����ڷ����е����������ļ��������������ļ�˵���ں˴���λ�ã�����ں˼��ص�ָ���ڴ档

After the loader in reserved sectors loaded to the memory by MBR, the initialization of devices, memory, processor and other hardwares will be done. And a visual operation interface will be provided. Finally, it will read configuration in the partition, which gives the location of the kernel on the hard disk, and load the kernel into memory.


��Ҫ����������ߣ�
Toolkits needed:

GCC

Nasm

һ��ʮ�����Ʊ༭�� A Hex Editior

һ��֧�ָ����Ĵ���༭������ѡ��Windows���Ƽ�Notepad++��  A code editior supporting highlight.

VirtualBox�����������ʹ�þɵġ��ȶ��汾�� VirtualBox (Old version is more stable)

