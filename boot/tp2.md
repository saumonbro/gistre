# TP2 - Boot - Simon Bousquet

# 1 - Partitions

Pour cet exercice, j'ai utilisé à la fois GNU Poke et hexdump.

## 1.1 - Type de table

GNU Poke fournit plusieurs fichiers définissant des types usuels. On trouve notamment `mbr.pk` et `gpt.pk`.

On peut donc essayer de charger le contenu du fichier avec les types définis :

```SHELL
(poke) load mbr
(poke) var mbr = MBR @0#B
(poke) mbr
```

On part ici du debut du fichier, donc du secteur 0 (LBA0). Selon la documentation d'uefi.org, LBA0 contient soit:

- Un legacy Master Boot Record (MBR)
- Un protective MBR


Par défaut, toutes les valeurs sont dumpées en décimal, mais on peut utiliser `.set obase` pour changer la base.

```SHELL
(poke) mbr
(poke) .set obase 16
(poke) mbr.magic
0xaa55UH
```

On reconnaît ici la valeur `0xaa55UH` qui correspond à la boot signature du MBR, à l'offset `510`. On a donc un header MBR. Il s'agit en l'occurence ici d'un Protective MBR

On reconnaît ensuite à l'offset 512 la signature `EFI PART`, correspondant à un header GPT.

```SHELL
(poke) load gpt
(poke) var gpt = GPT_Header @512#B
(poke) gpt.signature
[0x45UB,0x46UB,0x49UB,0x20UB,0x50UB,0x41UB,0x52UB,0x54UB]
```

Il s'agit donc ici d'une partition GPT.

## 1.2 - Context

On reconnais a l'addresse 0x400 (addresse de la premiere partition mappee selon la question 1.3) le GUID correspondant a une partition UEFI: `C12A7328-F81F-11D2-BA4B-00A0C93EC93B`.

Aussi, le disque a un format GPT, il est donc tres certainement utilisé par un UEFI. Cela se confirme par la signature à l'offset 512 : `EFI PART`.

## 1.3 - Nombre de partitions

L'offset 80 à partir du header GPT contient l'attribut `NumberOfPartitionEntries`, qui est ici égal à :

```SHELL
dump :from 0x250#B
00000250: 8000 0000 8000 0000
```

En convertissant de little à big endian, on obtient `0x00000080 = 128`.

On a donc 128 partitions possibles.

En regardant l'attribut `PartitionEntryLBA` :

```BASH
(poke) gpt.partition_entries_start_lba
0x0000000000000002UL#4096
```

On obtient la valeur 2, ce qui correspond à l'offset `0x400`, puisque la taille d'un secteur est de `0x200` (= 512).

En connaissant la taille d'une partition (128 bytes = `mbr.partition_entry_size`), on peut sauter de partition en partition et vérifier lesquelles possèdent des `PartitionTypeGUID` non nuls.

```SHELL
(poke) dump :from (0x400+128*0)#B
00000400: 2873 2ac1 1ff8 d211 ba4b 00a0 c93e c93b  (s*......K...>.;
(poke) dump :from (0x400+128*1)#B
00000480: af3d c60f 8384 7247 8e79 3d69 d847 7de4
(poke) dump :from (0x400+128*2)#B
00000500: af3d c60f 8384 7247 8e79 3d69 d847 7de4
```

On a donc 3 partitions sur le système.

## 1.4 - Blocs

On peut utiliser le champ `LastUsableLBA` ou `backup_lba`.

```BASH
(poke) gpt.last_usable_lba 
0x00000000773bd28eUL#4096
(poke) gpt.backup_lba 
0x00000000773bd2afUL#4096
```

Après conversion, on a :

- `0x00000000773bd28eUL = 2000409230`
- `0x00000000773bd2afUL = 2000409263`

Puisqu'il s'agit d'un indice, on l'incremente de 1, et en utilisant `gpt.backup_lba`, on a donc 2000409263 + 1 bloc = 2000409264 blocs.

# 2 - Qemu

## 2.1 - Composants

```BASH
CTRL E G G #P #D #C  Device Name
==== = = = == == === =========================================================
  33 R - -  0  1   6 PciRoot(0x0)
  67 D - -  2  0   0 Primary Console Input Device
  68 D - -  2  0   0 Primary Console Output Device
  69 D - -  1  0   0 Primary Standard Error Device
  8C D - -  1  0   0 PciRoot(0x0)/Pci(0x0,0x0)
  8D B - -  1  1   3 PciRoot(0x0)/Pci(0x1,0x0)
  8E B - -  1  4   2 Sata Controller
  8F D - -  1  0   0 PciRoot(0x0)/Pci(0x1,0x3)
  90 B - -  1  1   1 QEMU Video PCI Adapter
  91 B - -  1  1   1 PciRoot(0x0)/Pci(0x3,0x0)
  93 B - -  1  3   1 PciRoot(0x0)/Pci(0x2,0x0)/AcpiAdr(0x80010100)
  97 B - -  1  1   1 PciRoot(0x0)/Pci(0x1,0x0)/Serial(0x0)
  98 D - -  1  0   0 PciRoot(0x0)/Pci(0x1,0x0)/Serial(0x1)
  99 B - -  1  3   1 PS/2 Keyboard Device
  9A B - -  1  1   1 SIO Serial Port #0
  9B B - -  1  5   3 VT-UTF8 Serial Console
  9C D - -  1  1   0 QEMU HARDDISK                           
  9D D - -  1  2   0 QEMU QEMU DVD-ROM
  9E B - -  1  1   1 iPXE 82540em (0000:00:03.0, 52:54:00:12:34:56)
  9F D - -  1  0   0 PciRoot(0x0)/Pci(0x3,0x0)/MAC(525400123456,0x1)/VenHw(1DEF6CC2-C289-22E6-46D4-C6C651202192)
```

On reconnaît notamment :

- PCI
- SIO Serial Port
- PS/2 Keyboard Device

## 2.2 - Version

On utilise `ver`.

```SHELL
Shell> ver
UEFI Interactive Shell v2.2
EDK II
UEFI v2.70 (Debian distribution of EDK II, 0x00010000)
```

Avec `bcfg boot dump`, on observe 3 possibilités de boot :

```SHELL
Shell> bcfg boot dump
Option: 00. Variable: Boot0000   
  Desc    - UiApp(0x0)/Pci(0x1,0x1)/Ata(0x0)
  DevPath - Fv(7CB8BDC9-F8EB-4F34-AAEA-3EE4AF6516A1)/FvFile(462CAA21-7614-4503-836E-8AB6F4662331)
  Optional- N
Option: 01. Variable: Boot0001   
  Desc    - UEFI QEMU HARDDISK QM00001 
  DevPath - PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)
  Optional- Y
Option: 02. Variable: Boot0002   
Shell>    - EFI Internal Shell
  DevPath - Fv(7CB8BDC9-F8EB-4F34-AAEA-3EE4AF6516A1)/FvFile(7C04A583-9E3E-4F1C-AD65-E05268D0B4D1)
  Optional- N
```

## 2.3 - Modification de l’ordre de boot

On peut utiliser :

```SHELL
bcfg boot mv <n> <m>
```

Pour déplacer une entrée de boot. Par exemple :

```SHELL
bcfg boot mv 3 7
```

Déplace l'option de boot 3 sur l'option 7.

## 2.4 - Systeme de fichiers

```BASH
Shell> map
Mapping table
      FS1: Alias(s):CD0c1b:;BLK7:
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/CDROM(0x1)/HD(1,MBR,0x00000000)
      FS0: Alias(s):HD0a1:;BLK1:
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/HD(1,GPT,E487D5A5-3E54-445E-AA92-2BD098B80A36)
     BLK0: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)
     BLK4: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)
     BLK5: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/CDROM(0x0)
     BLK6: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/CDROM(0x1)
     BLK2: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/HD(2,GPT,43D64D90-53DC-423C-B742-3A70D8AEE9B1)
     BLK3: Alias(s):
          PciRoot(0x0)/Pci(0x1,0x1)/Ata(0x0)/HD(3,GPT,243374B9-60F1-4A86-A36E-A393565A16D3)
```

On retrouve plusieurs systèmes de fichiers, dont :

- `FS0` (Partition GPT)
- `FS1` (Lecteur CD/DVD (MBR))
- `BLK0`, `BLK2`, `BLK3`, `BLK4`, `BLK5`, `BLK6`

## 2.5 - Changement

Avec la commande :

```SHELL
Shell> BLK0:
Shell>
```

Rien ne se produit.

## 2.6 - FS0

```SHELL
Shell> FS0:
FS0:\>
```

Le prompt PS1 change pour nous indiquer un changement de système de fichiers.

## 2.7 - Lancement

On se déplace dans le dossier `EFI/boot` et on exécute `bootx64.efi` :

```SHELL
FS0:\> cd EFI/boot
FS0:\EFI\boot\> bootx64.efi
```