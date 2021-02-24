# Sistem Operasi by sOSis

## Penjelasan Singkat
printString diimplementasikan dengan menggunakan interrupt 0x10, pada printString, input (Read) dari pengguna dianggap sebuah String sehingga fungsi printString akan diulang hingga String mencapai '\0'.

Kemudian, readString akan menggunakan interrupt 0x16, dengan menggunakan beberapa kasus, jika input dari pengguna adalah backspace, enter, dan carriage agar kembali ke line pertama.

Cara kerja dari printLogo ialah menggunakan putIntoMemory seperti yang telah pada putIntoMemory pada file spesifikasi tugas untuk membentuk logo.

## Author:
* 13519176 (Made Kharisma J.)
* 13519182 (Farrell Abieza Z.)
* 13519221 (Allief Nuriman)
