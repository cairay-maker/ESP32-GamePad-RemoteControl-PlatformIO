for file in *.h *.cpp; do
  if [ -f "$file" ]; then
    echo "===== $file =====" >> 00_All_Code.txt
    cat "$file" >> 00_All_Code.txt
    echo -e "\n\n" >> 00_All_Code.txt
  fi
done

echo "All files merged into 00_All_Code.txt"
