# Sistem Operasi by sOSis

## Penjelasan Singkat
printString diimplementasikan dengan menggunakan interrupt 0x10.
Interrupt 0x10 memiliki 3 parameter yang diperlukan, sebagai contoh int interupt(int num, int AX, int BX, int CX, int DX), num merupakan fungsi interrupt yang akan dipanggilnya, AX merupakan satu string yang akan diprintnya. 0xe merupakan tempat untuk menaruh kode fungsi string yang akan diprint, semisal ingin memprint A (0x41), maka AX akan diisi dengan 0xe41 atau 0xe + 'A'. BX merupakan parameter yang diisikan dengan variable pointer char dari printString. untuk CX dan DX pada printstring parameternya tidak dibutuhkan, namun karena sebuah parameter tidak boleh bernilai Null, maka kedua parameter ini perlu diisi dengan 0 saja.
pada printString, input (Read) dari pengguna dianggap sebuah String sehingga fungsi printString akan diulang hingga String mencapai '\0'.

Kemudian, readString akan menggunakan interrupt 0x16, dengan menggunakan beberapa kasus, jika input dari pengguna adalah backspace, enter, dan carriage agar kembali ke line pertama.

Cara kerja dari printLogo ialah menggunakan putIntoMemory seperti yang telah pada putIntoMemory pada file spesifikasi tugas untuk membentuk logo.

## Author:
* 13519176 (Made Kharisma J.)
* 13519182 (Farrell Abieza Z.)
* 13519221 (Allief Nuriman)
