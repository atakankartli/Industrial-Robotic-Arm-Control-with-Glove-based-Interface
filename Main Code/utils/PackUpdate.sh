#!/bin/bash 
#Simple shell script to pack the update files.
  

#Buraya eklenecek dosyalar home/timeserver'a atılacaktır.


#Update'e eklenecek dosyaları buraya ekleyin ex:file1 file2 file3
#File ya da directory olması farketmiyor.
FILE1="dog"
FILE2="cat"
FILE3="bird"
SCRIPT="UpdateScript.sh"

#update.tar ismi değiştirilmemeli aksi takdirde bütün cihazlara
#UnpackUpdate.sh güncellemesi yapılması gerekir.

tar -cvf update.tar "$FILE1" "$FILE2" "$FILE3" "$SCRIPT"
sha256sum update.tar > update.hash
tar -cvzf update.tar.gz update.tar update.hash
gpg --always-trust --output update.tar.gz.gpg --encrypt -r atakan.kartli@tualcom.com.tr update.tar.gz

sudo rm update.tar
sudo rm update.hash
sudo rm update.tar.gz
