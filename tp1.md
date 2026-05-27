# 1 - Exploration

## 1.1

linux   /boot/vmlinuz-6.8.0-117-generic root=UUID=29ab125e-b1dd-4281-b40a-ac3d383112ab ro  quiet splash $vt_handoff

## 1.2

quiet: Disable log messages
splash: Causes the splash screen to be shown.
ro: Mount root device read-only on boot
$vt_handoff:
    vt = virtualterminal
    handoff is unique to ubuntu. it allows the kernel to maintain the current contents of video memory on a virtual terminal. 

# 2 - Données persistantes

## 2.1 Partitions

On utilise lsblk:
On a donc un bloc device composé de 9 partitions

nvme0n1     259:0    0 476.9G  0 disk 
├─nvme0n1p1 259:1    0   260M  0 part /boot/efi
├─nvme0n1p2 259:2    0    16M  0 part 
├─nvme0n1p3 259:3    0     8G  0 part 
├─nvme0n1p4 259:4    0 244.1G  0 part 
├─nvme0n1p5 259:5    0     1G  0 part 
├─nvme0n1p6 259:6    0  1016M  0 part 
├─nvme0n1p7 259:7    0 221.5G  0 part /
├─nvme0n1p8 259:8    0   900M  0 part 
└─nvme0n1p9 259:9    0   260M  0 part

On peut ensuire utiliser df -T pour obtenir les filesystem de chaque partition:

Filesystem     Type     1K-blocks      Used Available Use% Mounted on
tmpfs          tmpfs      1606540      3312   1603228   1% /run
/dev/nvme0n1p7 ext4     227452372 200595048  15230572  93% /
tmpfs          tmpfs      8032684         4   8032680   1% /dev/shm
tmpfs          tmpfs         5120         0      5120   0% /run/lock
efivarfs       efivarfs       192       108        80  58% /sys/firmware/efi/efivars
/dev/nvme0n1p1 vfat        262144     37332    224812  15% /boot/efi
tmpfs          tmpfs      1606536      1692   1604844   1% /run/user/1000

# 2.2

Sur ubuntu, initramfs est stocke sous la forme d'une archive cpio a /boot/initrd.img (qui est ici un symlink)

// todo verify

```SHELL
l -l /boot/initrd.img
lrwxrwxrwx 1 root root 29 May 21 22:57 /boot/initrd.img -> initrd.img-5.15.0-119-generic
```

```SHELL
file /boot/initrd.img-5.15.0-119-generic
/boot/initrd.img-5.15.0-119-generic: ASCII cpio archive (SVR4 with no CRC)

```

On utilise `df -T` pour connaitre la partition dans laquelle se trouve le fichier:

```SHELL
df -T /boot/initrd.img
Filesystem     Type 1K-blocks      Used Available Use% Mounted on
/dev/nvme0n1p7 ext4 227452372 200595524  15230096  93% /
```

Le format cpio est utilise de facon similaire a `tar` pour creer des archives en concatenant plusieurs fichier. On peut donc representer des hierarchies de fichier en un seul fichier.

# 2.3 

L'image contient l'ensemble des repertoires necessaire au filesystem root

On peut en dump le contenu avec `lsinitramfs`:

```SHELL
lsinitramfs /boot/initrd.img-5.15.0-119-generic

.
kernel
kernel/x86
kernel/x86/microcode
kernel/x86/microcode/AuthenticAMD.bin
kernel
kernel/x86
kernel/x86/microcode
kernel/x86/microcode/.enuineIntel.align.0123456789abc
kernel/x86/microcode/GenuineIntel.bin
.
bin
conf
conf/arch.conf
conf/conf.d
conf/initramfs.conf
etc
etc/console-setup
...
```



// todo

# 2.3 Noyeau

Sur ubuntu, le fichier kernel est stocke dans le repertoire /boot sous le nom vmlinuz. C'est egalement un symlink vers une image kernel compressee en format `bzimage`.

```SHELL
ls /boot/vmlinuz
lrwxrwxrwx 1 root root 26 May 21 22:57 /boot/vmlinuz -> vmlinuz-5.15.0-119-generic
```

```SHELL
/boot/vmlinuz-5.15.0-119-generic: Linux kernel x86 boot executable bzImage, version 5.15.0-119-generic (buildd@lcy02-amd64-075) #129-Ubuntu SMP Fri Aug 2 19:25:20 UTC 2024, RO-rootFS, swap_dev 0XB, Normal VGA

```

# 3. Linux

## 3.1 Archive

Le format du fichier obtenu est un executable en format ELF.
On le remarque en utilisant la commande `file` sur le fichier de sortie (ou via le header ELF).


```SHELL
sudo ./etract-vmlinux/boot/vmlinuz-6.8.0-117-generic  > out.elf
file out.elf
out.elf: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, BuildID[sha1]=<UUID>, stripped
```

## 3.2 Readelf

En consultant les Section headers des deux fichiers ELF, on remarque plusieurs differences notables:

Program Headers:

On remarque que `sh` est de type DYN (Position Independant Executable file), tandis que l'image kernel est de type EXEC.

Section Headers:

- Certaines sections comme .text et .bss sont bien plus importants sur l'image kernel en raison de la quantite de donnee.
- Des symboles supplementaires sont exportes tels que `__ksymtab` ou `__kcrctab`.

// TODO

## 3.3 Init

On utilise le programme C suivant:

```C
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    puts("quelque chose dans la console");
    sleep(30);
    return 0;
}
```

Qu'on compile et compresse avec cpio et gzip:

```SHELL
gcc -static main.c -o init && echo init | cpio -oH newc | gzip > initramfs.gz 
```

On utilise le format `newc` qui est celui de l'image `initrd.img`

```SHELL
file /boot/initrd.img-5.15.0-119-generic
/boot/initrd.img-5.15.0-119-generic: ASCII cpio archive (SVR4 with no CRC)
```

Lors de l'execution, les logs indiquent que l'executable `init` est utilise
"Run /init as init process".

Apres 30 secondes, le message "Kernel panic - not syncing: Attempted to kill init!" est affiche.

Cela s'explique car init est le processus parent de tous les autres processus et n'est donc pas cense se terminer avant l'arret de la machine.

# 3.4


