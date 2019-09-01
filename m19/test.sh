tests=../tests/$1*.m19;
GREEN='\033[0;32m';
RED='\033[0;31m';
NC='\033[0m';
PASSED=0;
FAILED=0;
rts=$HOME/compiladores/root/usr/lib;


for i in $tests; do 
  echo ${i#../tests/}; 
  ./m19 -g $i; 
  j=${i%.m19}; 
  yasm -felf32 $j.asm; 
  z=${j#../tests/}; 
  ld -m elf_i386 -o $z $z.o -L$rts -lrts; 
  ./$z > $z.out;
        result=`diff -Z <(tr -d ' \n\t'<../tests/expected/$z.out) <( tr -d ' \n\t'<$z.out)`; 
  if [ "$result" != "" ]; 
    then printf "${RED}FAILED$NC\n$result\n"; FAILED=$((FAILED+1))
  else printf "${GREEN}PASSED$NC\n"; PASSED=$((PASSED+1))

  fi; 
  #rm $z*; #$j.asm; 
done;

printf "\n***TOTAL: $((PASSED+FAILED))***\n\n";

printf "${GREEN}PASSED: $NC$PASSED\n";
printf "${RED}FAILED: $NC$FAILED\n";
