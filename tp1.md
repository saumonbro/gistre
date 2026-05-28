---
pdf_options:
  format: A4
  margin: 30mm 20mm
---
# TP1 - Boot - Simon Bousquet

# 1 - Exploration

## 1.1 - Ligne de commande

La ligne de commande est trouvable dans `/proc/cmdline`.

```
linux /boot/vmlinuz-6.8.0-117-generic root=UUID=29ab125e-b1dd-4281-b40a-ac3d383112ab ro  quiet splash $vt_handoff
```

## 1.2 - Contenu

`quiet` : Désactive les messages de log
`splash` : Affiche l'écran de démarrage (splash screen).
`ro` : Monte le périphérique root en lecture seule au démarrage.
`$vt_handoff` :
vt = terminal virtuel. handoff est propre à Ubuntu. Il permet au noyau de conserver le contenu actuel de la mémoire vidéo sur un terminal virtuel.

# 2 - Données persistantes

## 2.1 Partitions

On utilise `lsblk`.
On a donc un bloc device (ici un disque) composé de 9 partitions:

```SHELL
lsblk

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
```

On peut ensuite utiliser df -T pour afficher les systèmes de fichiers et leur type pour chaque point de mount:

```SHELL
df -T
Filesystem     Type     1K-blocks      Used Available Use% Mounted on
tmpfs          tmpfs      1606540      3312   1603228   1% /run
/dev/nvme0n1p7 ext4     227452372 200595048  15230572  93% /
tmpfs          tmpfs      8032684         4   8032680   1% /dev/shm
tmpfs          tmpfs         5120         0      5120   0% /run/lock
efivarfs       efivarfs       192       108        80  58% /sys/firmware/efi/efivars
/dev/nvme0n1p1 vfat        262144     37332    224812  15% /boot/efi
tmpfs          tmpfs      1606536      1692   1604844   1% /run/user/1000
```

## 2.2 - Initramfs

### 2.2.1 Trouvez votre initramfs, ou se situe t’il sur votre système ? Dans quelle partition ?

Sur ubuntu, initramfs est stocké sous la forme d'une archive cpio a /boot/initrd.img (qui est ici un symlink)

```SHELL
l -l /boot/initrd.img
lrwxrwxrwx 1 root root 29 May 21 22:57 /boot/initrd.img -> initrd.img-5.15.0-119-generic
```

```SHELL
file /boot/initrd.img-5.15.0-119-generic
/boot/initrd.img-5.15.0-119-generic: ASCII cpio archive (SVR4 with no CRC)
```

On utilise `df -T` pour connaître la partition dans laquelle se trouve le fichier:

```SHELL
df -T /boot/initrd.img
Filesystem     Type 1K-blocks      Used Available Use% Mounted on
/dev/nvme0n1p7 ext4 227452372 200595524  15230096  93% /
```

Le format cpio est utilisé pour créer des archives en concaténant plusieurs fichiers. On peut donc représenter des hiérarchies de fichier en un seul fichier.

### 2.2.2 Décrivez le contenu de ce dernier en quelques lignes.

L'image contient l'ensemble des répertoires et outils nécessaires pour préparer l'environnement  avant de monter le vrai filesystem et la partition racine ("/").

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

## 2.3 - Noyau

Sur ubuntu, le fichier kernel est stocké dans le répertoire /boot sous le nom `vmlinuz`. C'est ici un symlink vers une image kernel compressée en format `bzimage`.

```SHELL
ls /boot/vmlinuz
lrwxrwxrwx 1 root root 26 May 21 22:57 /boot/vmlinuz -> vmlinuz-5.15.0-119-generic
```

```SHELL
file /boot/vmlinuz-5.15.0-119-generic 
/boot/vmlinuz-5.15.0-119-generic: Linux kernel x86 boot executable bzImage, version 5.15.0-119-generic (buildd@lcy02-amd64-075) #129-Ubuntu SMP Fri Aug 2 19:25:20 UTC 2024, RO-rootFS, swap_dev 0XB, Normal VGA
```

Le but du format bzImage était originellement de faire tenir le noyau devenant de plus en plus lourd en mémoire lorsqu'en `real address mode`, notamment sur les processeurs x86, ou seulement 640 ko étaient utilisables.

# 3. Linux

## 3.1 - Archive

Le format du fichier obtenu est un exécutable en format ELF.
On le remarque en utilisant la commande `file` sur le fichier de sortie (ou via le header ELF).


```SHELL
sudo ./etract-vmlinux/boot/vmlinuz-6.8.0-117-generic  > out.elf
file out.elf
out.elf: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, BuildID[sha1]=<UUID>, stripped
```

## 3.2 - Readelf

En consultant les Section headers des deux fichiers ELF, on remarque plusieurs différences notables:

Program Headers:

- On remarque que `sh` est de type DYN (Position Independant Executable file), tandis que l'image kernel est de type EXEC.

DYN correspond à un binaire lié dynamiquement, ou les bibliothèques peuvent être chargées n'importe ou en mémoire.

EXEC correspond à un binaire lié statiquement, sans dépendances à des bibliothèques chargées dynamiquement.

Section Headers:

- Certaines sections comme .text et .bss sont bien plus importantes sur l'image kernel en raison de la quantité de données.
- Des symboles supplémentaires sont exportés tels que `__ksymtab` ou `__kcrctab`.

## 3.3 - Init

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

On utilise le format `newc` (SVR4) qui est celui de l'image `initrd.img`

```SHELL
file /boot/initrd.img-5.15.0-119-generic
/boot/initrd.img-5.15.0-119-generic: ASCII cpio archive (SVR4 with no CRC)
```

Lors de l'exécution, les logs indiquent que l'exécutable `init` est utilisé
"Run /init as init process".

Après 30 secondes, le message "Kernel panic - not syncing: Attempted to kill init!" est affiché.

Cela s'explique car init est le processus parent de tous les autres processus et n'est donc pas censé se terminer avant l'arrêt de la machine.

## 3.4 - Busybox

On obtient l'erreur suivante:

`rcS: No such file or directory`

Puis on boucle sur:

`/dev/tty4: No such file or directory.`

Si on ajoute le fichier /dev/tty4, on peut ensuite utiliser sh et exécuter les commandes builtin et disponibles dans le PATH.

## 3.5 - Fstab

Le fichier /etc/fstab contient les 6 colonnes suivantes:

- Device: l'uuid du périphérique monté
- Mount Point: Le répertoire correspondant au point de montage
- File system type: le type de filesystem utilisé
- Options: les options de montage
- Backup operations (valeure binaire): 1 si un archivage est réalisé par dump, 0 sinon
- File System Check Order: Indique a fcsk (utilitaire de réparation) dans quel ordre vérifier les partitions


```SHELL
cat /etc/fstab
...
# <file system> <mount point>   <type>  <options>       <dump>  <pass>
# / was on /dev/nvme0n1p6 during installation
UUID=29ab125e-b1dd-4281-b40a-ac3d383112ab /               ext4    errors=remount-ro 0       1
# /boot/efi was on /dev/nvme0n1p1 during installation
UUID=D264-4AA6  /boot/efi       vfat    umask=0077      0       1
/swapfile
```

Fichier `/etc/init.d/rsC`:

```SHELL
#!/bin/sh

mount -a

#mount -t proc proc /proc
#mount -t tmpfs tmpfs /tmp
#mount -t sysfs sysfs /sys
#mount -t tmpfs tmpfs /dev

mdev -s

echo "Coucou le kernel ! :D"
```

On utilise `mount -a` pour lire directement le fichier /etc/fstab, mais on peut également le faire à la main avec `mount -t`.

`mdev -s` (scan) parcourt `/sys` et crée tous les nœuds de périphériques correspondants dans /`dev`.

`tty4` est donc créé et les erreurs le concernant disparaissent et on peut ensuite utiliser `sh` et exécuter les commandes builtin et disponibles dans le PATH (ici /sbin:/usr/sbin:/bin:/usr/bin).
